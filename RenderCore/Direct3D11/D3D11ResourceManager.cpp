#include "stdafx.h"
#include "D3D11ResourceManager.h"

#include "D3D11RenderState.h"
#include "D3D11Resource.h"

#include "../common.h"
#include "../../Engine/EnumStringMap.h"
#include "../../Engine/KeyValueReader.h"
#include "../../shared/Util.h"

#include <D3Dcompiler.h>
#include <D3DX11.h>
#include <DXGI.h>
#include <fstream>

#pragma comment( lib, "d3dcompiler.lib" )

using namespace RE_HANDLE_TYPE;

namespace
{
	constexpr TCHAR* DEFAULT_TEXTURE_FILE_PATH = _T( "../Texture/" );
	constexpr TCHAR* TEXTURE_DESC_SCRIPT_FILE_NAME = _T( "../Script/TextureDesc.txt" );
	constexpr TCHAR* FRAME_BUFFER_SIZE_KEYWORD = _T( "FRAME_BUFFER" );

	void SizeHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			if ( keyValue->GetValue( ) == FRAME_BUFFER_SIZE_KEYWORD )
			{
				trait.m_width = 0;
				trait.m_height = 0;
				trait.m_depth = 1;
				trait.m_miscFlag |= RESOURCE_MISC::APP_SIZE_DEPENDENT;
			}
			else
			{
				Stringstream sStream( keyValue->GetValue( ) );
				sStream >> trait.m_width >> trait.m_height >> trait.m_depth;
			}
		}
	}

	void MipLevelHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			Stringstream sStream( keyValue->GetValue( ) );
			sStream >> trait.m_mipLevels;
		}
	}

	void FormatHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			trait.m_format = GetEnumStringMap( ).GetEnum( keyValue->GetValue( ), RESOURCE_FORMAT::UNKNOWN );
		}
	}

	void SampleDescHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			Stringstream sStream( keyValue->GetValue( ) );
			sStream >> trait.m_sampleCount >> trait.m_sampleQuality;
		}
	}

	void AccessHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			std::vector<String> accessFlags;
			UTIL::Split( keyValue->GetValue( ), accessFlags, _T( '|' ) );

			for ( const auto& flag : accessFlags )
			{
				trait.m_access |= static_cast<UINT>( GetEnumStringMap( ).GetEnum( flag, 0 ) );
			}
		}
	}

	void TypeHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			std::vector<String> resourceTypes;
			UTIL::Split( keyValue->GetValue( ), resourceTypes, _T( '|' ) );

			for ( const auto& type : resourceTypes )
			{
				trait.m_type |= static_cast<D3D11_USAGE>( GetEnumStringMap( ).GetEnum( type, RESOURCE_TYPE::SHADER_RESOURCE ) );
			}
		}
	}

	void MiscHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			std::vector<String> miscFlags;
			UTIL::Split( keyValue->GetValue( ), miscFlags, _T( '|' ) );

			for ( const auto& flag : miscFlags )
			{
				trait.m_miscFlag |= static_cast<UINT>( GetEnumStringMap( ).GetEnum( flag, 0 ) );
			}
		}
	}

	class CShaderByteCode
	{
	public:
		inline explicit CShaderByteCode( const size_t size ) :
			m_buffer( nullptr ),
			m_size( size )
		{
			if ( m_size > 0 )
			{
				m_buffer = new BYTE[m_size];
			}
		}

		CShaderByteCode( CShaderByteCode&& byteCode ) :
			m_buffer( byteCode.m_buffer ),
			m_size( byteCode.m_size )
		{
			byteCode.m_buffer = nullptr;
			byteCode.m_size = 0;
		}

		CShaderByteCode& operator=( CShaderByteCode&& byteCode )
		{
			m_buffer = byteCode.m_buffer;
			m_size = byteCode.m_size;
			byteCode.m_buffer = nullptr;
			byteCode.m_size = 0;
			return *this;
		}

		~CShaderByteCode( )
		{
			delete[] m_buffer;
			m_buffer = nullptr;
			m_size = 0;
		}

		CShaderByteCode( const CShaderByteCode& byteCode ) = delete;
		CShaderByteCode& operator=( const CShaderByteCode& byteCode ) = delete;

		BYTE* GetBufferPointer( ) const { return m_buffer; }
		size_t GetBufferSize( ) const { return m_size; }
	private:
		BYTE* m_buffer;
		size_t m_size;
	};

	inline Microsoft::WRL::ComPtr<ID3DBlob> GetShaderBlob( const TCHAR* pFilePath, const char* pProfile )
	{
		Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob( nullptr );
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob( nullptr );

		HRESULT hr = E_FAIL;

		int flag = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
		flag |= D3DCOMPILE_DEBUG;
#endif

		std::ifstream shaderfile( pFilePath, std::ios::in | std::ios::ate );

		if ( shaderfile )
		{
			size_t size = static_cast<size_t>( shaderfile.tellg( ) );
			shaderfile.seekg( 0 );

			char* buffer = new char[size];
			shaderfile.read( buffer, size );

			D3DCompile( buffer,
				size,
				nullptr,
				nullptr,
				nullptr,
				"main",
				pProfile,
				flag,
				0,
				&shaderBlob,
				&errorBlob );

			if ( errorBlob )
			{
				char* errorMessage = static_cast<char*>( errorBlob->GetBufferPointer( ) );
				if ( errorMessage )
				{
					OutputDebugStringA( errorMessage );
				}
			}

			delete[] buffer;
		}

		return SUCCEEDED( hr ) ? shaderBlob : nullptr;
	}

	inline CShaderByteCode GetCompiledByteCode( const TCHAR* pFilePath )
	{
		static String baseByteCodePath( _T( "../bin/Shader/" ) );
		String fileName = UTIL::GetFileName( pFilePath );
		String compiledByteCodePath = baseByteCodePath + fileName + String( _T( ".cso" ) );

		std::ifstream shaderfile( compiledByteCodePath, std::ios::in | std::ios::ate | std::ios::binary );

		if ( shaderfile )
		{
			size_t size = static_cast<size_t>( shaderfile.tellg( ) );
			shaderfile.seekg( 0 );

			CShaderByteCode byteCode( size );
			shaderfile.read( (char*)byteCode.GetBufferPointer( ), size );
			return byteCode;
		}

		return CShaderByteCode( 0 );
	}
}

bool CD3D11ResourceManager::Bootup( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
{
	OnDeviceRestore( pDevice, pDeviceContext );

	CKeyValueReader keyValueReader;
	std::shared_ptr<KeyValue> keyValue = keyValueReader.LoadKeyValueFromFile( TEXTURE_DESC_SCRIPT_FILE_NAME );

	if ( keyValue == nullptr )
	{
		return false;
	}

	for ( const KeyValue* texture = keyValue->GetChild( ); texture != nullptr; texture = texture->GetNext( ) )
	{
		auto ret = m_textureTraits.emplace( texture->GetKey( ), TEXTURE_TRAIT( ) );

		TEXTURE_TRAIT& newTrait = ret.first->second;

		if ( const KeyValue* pSize = texture->Find( _T( "Size" ) ) )
		{
			SizeHandler( pSize, newTrait );
		}
		
		if ( const KeyValue* pSample = texture->Find( _T( "Sample" ) ) )
		{
			SampleDescHandler( pSample, newTrait );
		}
		
		if ( const KeyValue* pMipLevels = texture->Find( _T( "MipLevels" ) ) )
		{
			MipLevelHandler( pMipLevels, newTrait );
		}
		
		if ( const KeyValue* pFormat = texture->Find( _T( "Format" ) ) )
		{
			FormatHandler( pFormat, newTrait );
		}
		
		if ( const KeyValue* pAccess = texture->Find( _T( "Access" ) ) )
		{
			AccessHandler( pAccess, newTrait );
		}
		
		if ( const KeyValue* pType = texture->Find( _T( "Type" ) ) )
		{
			TypeHandler( pType, newTrait );
		}
		
		if ( const KeyValue* pMisc = texture->Find( _T( "Misc" ) ) )
		{
			MiscHandler( pMisc, newTrait );
		}
	}

	// Load Render State Desc
	m_DepthStencilFactory.LoadDesc( );
	m_RasterizerFactory.LoadDesc( );
	m_SamplerFactory.LoadDesc( );
	m_BlendFactory.LoadDesc( );

	// Load Shader From Script
	

	return true;
}

bool CD3D11ResourceManager::LoadTextureFromFile( const String& fileName )
{
	TCHAR pPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, pPath );
	::SetCurrentDirectory( DEFAULT_TEXTURE_FILE_PATH );

	D3DX11_IMAGE_INFO info;
	HRESULT hr;
	D3DX11GetImageInfoFromFile( fileName.c_str( ), nullptr, &info, &hr );

	std::unique_ptr<CD3D11Texture>* newTexture = nullptr;
	if ( m_freeTexture != nullptr )
	{
		PopFrontFreeResourceList( &m_freeTexture, &newTexture );
	}
	else
	{
		m_textures.emplace_back( );
		newTexture = &m_textures.back( );
	}

	if ( SUCCEEDED( hr ) )
	{
		switch ( info.ResourceDimension )
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			{
				*newTexture = std::make_unique<CD3D11Texture1D>( );
				CD3D11Texture1D* texture1D = static_cast<CD3D11Texture1D*>( newTexture->get( ) );

				if ( texture1D->LoadFromFile( *m_pDevice, fileName.c_str( ) ) == false )
				{
					newTexture->reset( );
					PushFrontFreeResourceList( &m_freeTexture, newTexture );
				}
			}
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			{
				*newTexture = std::make_unique<CD3D11Texture2D>( );
				CD3D11Texture2D* texture2D = static_cast<CD3D11Texture2D*>( newTexture->get( ) );

				if ( texture2D->LoadFromFile( *m_pDevice, fileName.c_str( ) ) == false )
				{
					newTexture->reset( );
					PushFrontFreeResourceList( &m_freeTexture, newTexture );
				}
			}
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			{
				*newTexture = std::make_unique<CD3D11Texture3D>( );
				CD3D11Texture3D* texture3D = static_cast<CD3D11Texture3D*>( newTexture->get( ) );

				if ( texture3D->LoadFromFile( *m_pDevice, fileName.c_str( ) ) == false )
				{
					newTexture->reset( );
					PushFrontFreeResourceList( &m_freeTexture, newTexture );
				}
			}
			break;
		default:
			{
				__debugbreak( );
			}
			break;
		}
	}

	int idx = ( newTexture - &m_textures.front( ) );
	m_texLUT.emplace( fileName, MakeResourceHandle( TEXTURE_HANDLE, idx ) );

	::SetCurrentDirectory( pPath );
	return false;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture1D( const TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData )
{
	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}
	
	std::unique_ptr<CD3D11Texture>* newTexture = nullptr;
	if ( m_freeTexture != nullptr )
	{
		PopFrontFreeResourceList( &m_freeTexture, &newTexture );
	}
	else
	{
		m_textures.emplace_back();
		newTexture = &m_textures.back( );
	}

	*newTexture = std::make_unique<CD3D11Texture1D>( );
	CD3D11Texture1D* texture1D = static_cast<CD3D11Texture1D*>( newTexture->get() );

	if ( texture1D->Create( *m_pDevice, trait, initData ) )
	{
		int idx = ( newTexture - &m_textures.front() );
		RE_HANDLE handle = MakeResourceHandle( TEXTURE_HANDLE, idx );

		m_texLUT.emplace( textureName, handle );
		return handle;
	}
	else
	{
		newTexture->reset( );
		PushFrontFreeResourceList( &m_freeTexture, newTexture );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture1D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData )
{
	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}

	auto trait = m_textureTraits.find( descName );
	if ( trait != m_textureTraits.end( ) )
	{
		if ( trait->second.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
		{
			trait->second.m_width = m_frameBufferSize.first;
			trait->second.m_height = m_frameBufferSize.second;
		}
		
		return CreateTexture1D( trait->second, textureName, initData );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture2D( const TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData )
{
	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}

	std::unique_ptr<CD3D11Texture>* newTexture = nullptr;
	if ( m_freeTexture != nullptr )
	{
		PopFrontFreeResourceList( &m_freeTexture, &newTexture );
	}
	else
	{
		m_textures.emplace_back( );
		newTexture = &m_textures.back( );
	}

	*newTexture = std::make_unique<CD3D11Texture2D>( );
	CD3D11Texture2D* texture2D = static_cast<CD3D11Texture2D*>( newTexture->get( ) );

	if ( texture2D->Create( *m_pDevice, trait, initData ) )
	{
		int idx = ( newTexture - &m_textures.front( ) );
		RE_HANDLE handle = MakeResourceHandle( TEXTURE_HANDLE, idx );

		m_texLUT.emplace( textureName, handle );
		return handle;
	}
	else
	{
		newTexture->reset( );
		PushFrontFreeResourceList( &m_freeTexture, newTexture );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture2D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData )
{
	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}

	auto trait = m_textureTraits.find( descName );
	if ( trait != m_textureTraits.end( ) )
	{
		if ( trait->second.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
		{
			trait->second.m_width = m_frameBufferSize.first;
			trait->second.m_height = m_frameBufferSize.second;
		}

		return CreateTexture2D( trait->second, textureName, initData );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::RegisterTexture2D( const String& textureName, void* pTexture, bool isAppSizeDependent )
{
	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}

	std::unique_ptr<CD3D11Texture>* newTexture = nullptr;
	if ( m_freeTexture != nullptr )
	{
		PopFrontFreeResourceList( &m_freeTexture, &newTexture );
	}
	else
	{
		m_textures.emplace_back( );
		newTexture = &m_textures.back( );
	}

	*newTexture = std::make_unique<CD3D11Texture2D>( );
	CD3D11Texture2D* texture2D = static_cast<CD3D11Texture2D*>( newTexture->get( ) );

	Microsoft::WRL::ComPtr<ID3D11Resource> texture = static_cast<ID3D11Resource*>( pTexture );
	texture2D->SetTexture( texture, isAppSizeDependent );

	int idx = ( newTexture - &m_textures.front( ) );
	RE_HANDLE handle = MakeResourceHandle( TEXTURE_HANDLE, idx );

	m_texLUT.emplace( textureName, handle );
	return handle;
}

RE_HANDLE CD3D11ResourceManager::FindTexture( const String& textureName ) const
{
	auto found = m_texLUT.find( textureName );

	if ( found != m_texLUT.end( ) )
	{
		return found->second;
	}

	return INVALID_HANDLE;
}

const TEXTURE_TRAIT& CD3D11ResourceManager::GetTextureTrait( RE_HANDLE texhandle ) const
{
	return GetTexture( texhandle ).GetTrait( );
}

void CD3D11ResourceManager::AppSizeChanged( UINT nWndWidth, UINT nWndHeight )
{
	m_frameBufferSize.first = nWndWidth;
	m_frameBufferSize.second = nWndHeight;

	auto iter = m_dsvLUT.begin( );
	RE_HANDLE handle = INVALID_HANDLE;

	while ( iter != m_dsvLUT.end( ) )
	{
		handle = iter->second;
		const CDepthStencil& dsv = GetDepthstencil( iter->second );
		if ( dsv.IsAppSizeDependency() )
		{
			iter = m_dsvLUT.erase( iter );
			int index = handle & RE_INDEX_MASK;
			m_depthStencils[index].Free( );

			PushFrontFreeResourceList( &m_freeDepthStencil, &m_depthStencils[index] );
		}
		else
		{
			++iter;
		}
	}

	iter = m_rtvLUT.begin( );
	while ( iter != m_rtvLUT.end( ) )
	{
		handle = iter->second;
		const CRenderTarget& rtv = GetRendertarget( iter->second );
		if ( rtv.IsAppSizeDependency( ) )
		{
			iter = m_rtvLUT.erase( iter );
			int index = handle & RE_INDEX_MASK;
			m_renderTargets[index].Free();

			PushFrontFreeResourceList( &m_freeRenderTarget, &m_renderTargets[index] );
		}
		else
		{
			++iter;
		}
	}

	iter = m_srvLUT.begin( );
	while ( iter != m_srvLUT.end( ) )
	{
		handle = iter->second;
		const CD3D11ShaderResource& srv = GetShaderResource( iter->second );
		if ( srv.IsAppSizeDependency( ) )
		{
			iter = m_srvLUT.erase( iter );
			int index = handle & RE_INDEX_MASK;
			m_shaderResources[index].Free();

			PushFrontFreeResourceList( &m_freeShaderResource, &m_shaderResources[index] );
		}
		else
		{
			++iter;
		}
	}

	iter = m_ravLUT.begin( );
	while ( iter != m_ravLUT.end( ) )
	{
		handle = iter->second;
		const CD3D11RandomAccessResource& rav = GetRandomAccess( iter->second );
		if ( rav.IsAppSizeDependency( ) )
		{
			iter = m_ravLUT.erase( iter );
			int index = handle & RE_INDEX_MASK;
			m_randomAccessResource[index].Free();

			PushFrontFreeResourceList( &m_freeRandomAccess, &m_randomAccessResource[index] );
		}
		else
		{
			++iter;
		}
	}

	iter = m_texLUT.begin( );
	while ( iter != m_texLUT.end( ) )
	{
		handle = iter->second;
		const ITexture& tex = GetTexture( iter->second );
		if ( tex.IsAppSizeDependency( ) )
		{
			iter = m_texLUT.erase( iter );
			int index = handle & RE_INDEX_MASK;
			m_textures[index].reset( );

			PushFrontFreeResourceList( &m_freeTexture, &m_textures[index] );
		}
		else
		{
			++iter;
		}
	}
}

RE_HANDLE CD3D11ResourceManager::CreateBuffer( const BUFFER_TRAIT& trait )
{
	CD3D11Buffer buffer;

	if ( buffer.Create( *m_pDevice, trait ) )
	{
		m_buffers.emplace_back( std::move( buffer ) );

		int idx = static_cast<int>( m_buffers.size( ) ) - 1;
		return MakeResourceHandle( BUFFER_HANDLE, idx );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateVertexShader( const TCHAR* pFilePath, const char* pProfile )
{
	String fileName = UTIL::GetFileName( pFilePath );
	RE_HANDLE found = FindGraphicsShaderByName( fileName.c_str( ) );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11VertexShader vs;

	D3D11_INPUT_ELEMENT_DESC inputDesc[4];

	inputDesc[0].SemanticName = "POSITION";
	inputDesc[0].SemanticIndex = 0;
	inputDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[0].InputSlot = 0;
	inputDesc[0].AlignedByteOffset = 0;
	inputDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[0].InstanceDataStepRate = 0;

	inputDesc[1].SemanticName = "NORMAL";
	inputDesc[1].SemanticIndex = 0;
	inputDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[1].InputSlot = 0;
	inputDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[1].InstanceDataStepRate = 0;

	inputDesc[2].SemanticName = "COLOR";
	inputDesc[2].SemanticIndex = 0;
	inputDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[2].InputSlot = 0;
	inputDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[2].InstanceDataStepRate = 0;

	inputDesc[3].SemanticName = "TEXCOORD";
	inputDesc[3].SemanticIndex = 0;
	inputDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputDesc[3].InputSlot = 0;
	inputDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[3].InstanceDataStepRate = 0;

	bool result = false;
	CShaderByteCode byteCode = GetCompiledByteCode( pFilePath );
	if ( byteCode.GetBufferSize( ) > 0 )
	{
		result = vs.CreateShader( *m_pDevice, byteCode.GetBufferPointer( ), byteCode.GetBufferSize( ), inputDesc, 4 );
	}
	else if ( Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = GetShaderBlob( pFilePath, pProfile ) )
	{
		result = vs.CreateShader( *m_pDevice, shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ), inputDesc, 4 );
	}

	if ( result )
	{
		m_vertexShaders.emplace_back( std::move( vs ) );

		int idx = static_cast<int>( m_vertexShaders.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( VS_HANDLE, idx );

		m_vertexShaderLUT.emplace( fileName, handle );
		return handle;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreatePixelShader( const TCHAR* pFilePath, const char* pProfile )
{
	String fileName = UTIL::GetFileName( pFilePath );
	RE_HANDLE found = FindGraphicsShaderByName( fileName.c_str( ) );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11PixelShader ps;
	bool result = false;
	CShaderByteCode byteCode = GetCompiledByteCode( pFilePath );

	if ( byteCode.GetBufferSize( ) > 0 )
	{
		result = ps.CreateShader( *m_pDevice, byteCode.GetBufferPointer( ), byteCode.GetBufferSize( ) );
	}
	else if ( Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = GetShaderBlob( pFilePath, pProfile ) )
	{
		result = ps.CreateShader( *m_pDevice, shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ) );
	}

	if ( result )
	{
		m_pixelShaders.emplace_back( std::move( ps ) );

		int idx = static_cast<int>( m_pixelShaders.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( PS_HANDLE, idx );

		m_pixelShaderLUT.emplace( fileName, handle );
		return handle;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateComputeShader( const TCHAR* pFilePath, const char* pProfile )
{
	String fileName = UTIL::GetFileName( pFilePath );
	RE_HANDLE found = FindComputeShaderByName( fileName.c_str( ) );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11ComputeShader cs;
	bool result = false;
	CShaderByteCode byteCode = GetCompiledByteCode( pFilePath );

	if ( byteCode.GetBufferSize( ) > 0 )
	{
		result = cs.CreateShader( *m_pDevice, byteCode.GetBufferPointer( ), byteCode.GetBufferSize( ) );
	}
	else if ( Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = GetShaderBlob( pFilePath, pProfile ) )
	{
		result = cs.CreateShader( *m_pDevice, shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ) );
	}

	if ( result )
	{
		m_computeShaders.emplace_back( std::move( cs ) );

		int idx = static_cast<int>( m_computeShaders.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( CS_HANDLE, idx );

		m_computeShaderLUT.emplace( fileName, handle );
		return handle;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::FindGraphicsShaderByName( const TCHAR* pName )
{
	if ( !pName )
	{
		return INVALID_HANDLE;
	}

	auto found = m_vertexShaderLUT.find( pName );
	if ( found != m_vertexShaderLUT.end( ) )
	{
		return found->second;
	}

	found = m_pixelShaderLUT.find( pName );
	if ( found != m_pixelShaderLUT.end( ) )
	{
		return found->second;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::FindComputeShaderByName( const TCHAR* pName )
{
	if ( !pName )
	{
		return INVALID_HANDLE;
	}

	auto found = m_computeShaderLUT.find( pName );

	if ( found != m_computeShaderLUT.end( ) )
	{
		return found->second;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateRenderTarget( RE_HANDLE texhandle, const String& renderTargetName, const TEXTURE_TRAIT* trait )
{
	assert( texhandle != INVALID_HANDLE );
	RE_HANDLE found = FindRenderTarget( renderTargetName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CRenderTarget* newRenderTarget = nullptr;
	if ( m_freeRenderTarget != nullptr )
	{
		PopFrontFreeResourceList( &m_freeRenderTarget, &newRenderTarget );
	}
	else
	{
		m_renderTargets.emplace_back( );
		newRenderTarget = &m_renderTargets.back( );
	}

	const CD3D11Texture& texture = GetTexture( texhandle );

	if ( newRenderTarget->CreateRenderTarget( *m_pDevice, texture, trait ) )
	{
		int idx = ( newRenderTarget - &m_renderTargets.front( ) );
		RE_HANDLE handle = MakeResourceHandle( RENDER_TARGET_HANDLE, idx );

		m_rtvLUT.emplace( renderTargetName, handle );
		return handle;
	}
	else
	{
		PushFrontFreeResourceList( &m_freeRenderTarget, newRenderTarget );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateDepthStencil( RE_HANDLE texhandle, const String& depthStencilName, const TEXTURE_TRAIT* trait )
{
	assert( texhandle != INVALID_HANDLE );
	RE_HANDLE found = FindDepthStencil( depthStencilName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CDepthStencil* newDepthStencil = nullptr;
	if ( m_freeDepthStencil != nullptr )
	{
		PopFrontFreeResourceList( &m_freeDepthStencil, &newDepthStencil );
	}
	else
	{
		m_depthStencils.emplace_back( );
		newDepthStencil = &m_depthStencils.back( );
	}
	
	const CD3D11Texture& texture = GetTexture( texhandle );

	if ( newDepthStencil->CreateDepthStencil( *m_pDevice, texture, trait ) )
	{
		int idx = ( newDepthStencil - &m_depthStencils.front() );
		RE_HANDLE handle = MakeResourceHandle( DEPTH_STENCIL_HANDLE, idx );

		m_dsvLUT.emplace( depthStencilName, handle );
		return handle;
	}
	else
	{
		PushFrontFreeResourceList( &m_freeDepthStencil, newDepthStencil );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::FindRenderTarget( const String& renderTargetName ) const
{
	auto found = m_rtvLUT.find( renderTargetName );

	if ( found != m_rtvLUT.end( ) )
	{
		return found->second;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::FindDepthStencil( const String& depthStencilName ) const
{
	auto found = m_dsvLUT.find( depthStencilName );

	if ( found != m_dsvLUT.end( ) )
	{
		return found->second;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateShaderResourceFromFile( const String& fileName )
{
	RE_HANDLE handle = FindShaderResource( fileName );
	if ( handle != INVALID_HANDLE )
	{
		return handle;
	}

	TCHAR pPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, pPath );
	::SetCurrentDirectory( DEFAULT_TEXTURE_FILE_PATH );

	Ifstream textureFile;
	textureFile.open( fileName, 0 );

	if ( !textureFile.is_open( ) )
	{
		::SetCurrentDirectory( pPath );
		return handle;
	}

	CD3D11ShaderResource* newShaderResource = nullptr;
	if ( m_freeRenderTarget != nullptr )
	{
		PopFrontFreeResourceList( &m_freeShaderResource, &newShaderResource );
	}
	else
	{
		m_shaderResources.emplace_back( );
		newShaderResource = &m_shaderResources.back( );
	}
	
	if ( newShaderResource->CreateShaderResourceFromFile( *m_pDevice, fileName ) )
	{
		int idx = ( newShaderResource - &m_shaderResources.front( ) );
		handle = MakeResourceHandle( SHADER_RESOURCE_HANDLE, idx );

		m_srvLUT.emplace( fileName, handle );
	}
	else
	{
		PushFrontFreeResourceList( &m_freeShaderResource, newShaderResource );
	}

	textureFile.close( );

	::SetCurrentDirectory( pPath );

	return handle;
}

RE_HANDLE CD3D11ResourceManager::FindShaderResource( const String& fileName ) const
{
	auto found = m_srvLUT.find( fileName );

	if ( found != m_srvLUT.end( ) )
	{
		return found->second;
	}
	
	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateTextureShaderResource( RE_HANDLE texHandle, const String& resourceName, const TEXTURE_TRAIT* trait )
{
	assert( texHandle != INVALID_HANDLE );
	RE_HANDLE found = FindShaderResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11ShaderResource* newResource = nullptr;
	if ( m_freeShaderResource != nullptr )
	{
		PopFrontFreeResourceList( &m_freeShaderResource, &newResource );
	}
	else
	{
		m_shaderResources.emplace_back( );
		newResource = &m_shaderResources.back( );
	}

	const CD3D11Texture& texture = GetTexture( texHandle );

	if ( newResource->CreateShaderResource( *m_pDevice, texture, trait ) )
	{
		int idx = ( newResource - &m_shaderResources.front( ) );
		RE_HANDLE handle = MakeResourceHandle( SHADER_RESOURCE_HANDLE, idx );

		m_srvLUT.emplace( resourceName, handle );

		return handle;
	}
	else
	{
		PushFrontFreeResourceList( &m_freeShaderResource, newResource );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateBufferShaderResource( RE_HANDLE bufHandle, const String& resourceName, const BUFFER_TRAIT* trait )
{
	assert( bufHandle != INVALID_HANDLE );
	RE_HANDLE found = FindShaderResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11ShaderResource* newResource = nullptr;
	if ( m_freeShaderResource != nullptr )
	{
		PopFrontFreeResourceList( &m_freeShaderResource, &newResource );
	}
	else
	{
		m_shaderResources.emplace_back( );
		newResource = &m_shaderResources.back( );
	}

	const CD3D11Buffer& buffer = GetBuffer( bufHandle );

	if ( newResource->CreateShaderResource( *m_pDevice, buffer, trait ) )
	{
		int idx = ( newResource - &m_shaderResources.front( ) );
		RE_HANDLE handle = MakeResourceHandle( SHADER_RESOURCE_HANDLE, idx );

		m_srvLUT.emplace( resourceName, handle );

		return handle;
	}
	else
	{
		PushFrontFreeResourceList( &m_freeShaderResource, newResource );
	}

	return INVALID_HANDLE;
}

void CD3D11ResourceManager::RegisterShaderResource( const String& resourceName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srView )
{
	RE_HANDLE found = FindShaderResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}

	CD3D11ShaderResource* newResource = nullptr;
	if ( m_freeShaderResource != nullptr )
	{
		PopFrontFreeResourceList( &m_freeShaderResource, &newResource );
	}
	else
	{
		m_shaderResources.emplace_back( );
		newResource = &m_shaderResources.back( );
	}

	newResource->SetShaderResourceView( srView );

	int idx = ( newResource - &m_shaderResources.front( ) );
	RE_HANDLE handle = MakeResourceHandle( SHADER_RESOURCE_HANDLE, idx );

	m_srvLUT.emplace( resourceName, handle );
}

RE_HANDLE CD3D11ResourceManager::FindRandomAccessResource( const String& resourceName ) const
{
	auto found = m_ravLUT.find( resourceName );

	if ( found != m_ravLUT.end( ) )
	{
		return found->second;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateTextureRandomAccess( RE_HANDLE texHandle, const String& resourceName, const TEXTURE_TRAIT* trait )
{
	assert( texHandle != INVALID_HANDLE );
	RE_HANDLE found = FindRandomAccessResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11RandomAccessResource* newResource = nullptr;
	if ( m_freeRandomAccess != nullptr )
	{
		PopFrontFreeResourceList( &m_freeRandomAccess, &newResource );
	}
	else
	{
		m_randomAccessResource.emplace_back( );
		newResource = &m_randomAccessResource.back( );
	}

	const CD3D11Texture& texture = GetTexture( texHandle );

	if ( newResource->CreateRandomAccessResource( *m_pDevice, texture, trait ) )
	{
		int idx = ( newResource - &m_randomAccessResource.front( ) );
		RE_HANDLE handle = MakeResourceHandle( RANDOM_ACCESS_HANDLE, idx );

		m_ravLUT.emplace( resourceName, handle );
		return handle;
	}
	else
	{
		PushFrontFreeResourceList( &m_freeRandomAccess, newResource );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateBufferRandomAccess( RE_HANDLE bufHandle, const String& resourceName, const BUFFER_TRAIT* trait )
{
	assert( bufHandle != INVALID_HANDLE );
	RE_HANDLE found = FindRandomAccessResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11RandomAccessResource* newResource = nullptr;
	if ( m_freeRandomAccess != nullptr )
	{
		PopFrontFreeResourceList( &m_freeRandomAccess, &newResource );
	}
	else
	{
		m_randomAccessResource.emplace_back( );
		newResource = &m_randomAccessResource.back( );
	}

	const CD3D11Buffer& buffer = GetBuffer( bufHandle );

	if ( newResource->CreateRandomAccessResource( *m_pDevice, buffer, trait ) )
	{
		int idx = ( newResource - &m_randomAccessResource.front( ) );
		RE_HANDLE handle = MakeResourceHandle( RANDOM_ACCESS_HANDLE, idx );

		m_ravLUT.emplace( resourceName, handle );
		return handle;
	}
	else
	{
		PushFrontFreeResourceList( &m_freeRandomAccess, newResource );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateSamplerState( const String& stateName )
{
	auto found = m_samplerStateLUT.find( stateName );
	if ( found != m_samplerStateLUT.end( ) )
	{
		return found->second;
	}

	CD3D11SamplerState* newState = m_SamplerFactory.GetSamplerState( *m_pDevice, stateName );
	if ( newState )
	{
		m_samplerStates.emplace_back( std::move( *newState ) );

		int idx = static_cast<int>( m_samplerStates.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( SAMPLER_STATE_HANDLE, idx );

		m_samplerStateLUT.emplace( stateName, handle );

		return handle;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateRasterizerState( const String& stateName )
{
	auto found = m_rasterizerStateLUT.find( stateName );
	if ( found != m_rasterizerStateLUT.end( ) )
	{
		return found->second;
	}

	CD3D11RasterizerState* newState = m_RasterizerFactory.GetRasterizerState( *m_pDevice, stateName );
	if ( newState )
	{
		m_rasterizerStates.emplace_back( std::move( *newState ) );

		int idx = static_cast<int>( m_rasterizerStates.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( RASTERIZER_STATE_HANDLE, idx );

		m_rasterizerStateLUT.emplace( stateName, handle );

		return handle;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateBlendState( const String& stateName )
{
	auto found = m_blendStateLUT.find( stateName );
	if ( found != m_blendStateLUT.end( ) )
	{
		return found->second;
	}

	CD3D11BlendState* newState = m_BlendFactory.GetBlendState( *m_pDevice, stateName );
	if ( newState )
	{
		m_blendStates.emplace_back( std::move( *newState ) );

		int idx = static_cast<int>( m_blendStates.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( BLEND_STATE_HANDLE, idx );

		m_blendStateLUT.emplace( stateName, handle );

		return handle;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateDepthStencilState( const String& stateName )
{
	auto found = m_depthStencilStateLUT.find( stateName );
	if ( found != m_depthStencilStateLUT.end( ) )
	{
		return found->second;
	}

	CD3D11DepthStencilState* newState = m_DepthStencilFactory.GetDepthStencilState( *m_pDevice, stateName );
	if ( newState )
	{
		m_depthStencilStates.emplace_back( std::move( *newState ) );
		
		int idx = static_cast<int>( m_depthStencilStates.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( DEPTH_STENCIL_STATE_HANDLE, idx );

		m_depthStencilStateLUT.emplace( stateName, handle );

		return handle;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateCloneTexture( RE_HANDLE texHandle, const String& textureName )
{
	assert( texHandle != INVALID_HANDLE );
	const ITexture& src = GetTexture( texHandle );

	switch ( src.GetType( ) )
	{
	case TEXTURE_TYPE::TEXTURE_1D:
		break;
	case TEXTURE_TYPE::TEXTURE_2D:
	{
		TEXTURE_TRAIT trait = src.GetTrait( );

		// Applications can't specify NULL for pInitialData when creating IMMUTABLE resources
		trait.m_type |= RESOURCE_TYPE::SHADER_RESOURCE;
		trait.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE;

		RE_HANDLE newHandle = CreateTexture2D( trait, textureName );
		if ( newHandle != INVALID_HANDLE )
		{
			trait.m_format = ( trait.m_format == RESOURCE_FORMAT::R24G8_TYPELESS ) ? RESOURCE_FORMAT::R24_UNORM_X8_TYPELESS : trait.m_format;

			CreateTextureShaderResource( newHandle, textureName, &trait );

			return newHandle;
		}
	}
	break;
	case TEXTURE_TYPE::TEXTURE_3D:
		break;
	default:
		break;
	}

	return INVALID_HANDLE;
}

void CD3D11ResourceManager::CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull )
{
	ID3D11Resource* pDest = nullptr;
	ID3D11Resource* pSrc = nullptr;

	pDest = GetD3D11ResourceGeneric( dest );
	pSrc = GetD3D11ResourceGeneric( src );

	assert( pDest != nullptr && pSrc != nullptr );

	if ( destRegionOrNull == nullptr || srcRegionOrNull == nullptr )
	{
		m_pDeviceContext->CopyResource( pDest, pSrc );
	}
	else
	{
		const RESOURCE_REGION& destRegion = *destRegionOrNull;
		const RESOURCE_REGION& srcRegion = *srcRegionOrNull;

		D3D11_BOX box = { srcRegion.m_left,  srcRegion.m_top,  srcRegion.m_front,  srcRegion.m_right,  srcRegion.m_bottom,  srcRegion.m_back };

		m_pDeviceContext->CopySubresourceRegion( pDest, destRegion.m_subResource, destRegion.m_left, destRegion.m_top, destRegion.m_front, pSrc, srcRegion.m_subResource, &box );
	}
}

void CD3D11ResourceManager::TakeSnapshot( const String& sourceTextureName, const String& destTextureName )
{
	RE_HANDLE srcHandle = FindTexture( sourceTextureName );

	if ( srcHandle == INVALID_HANDLE )
	{
		DebugWarning( "Snapshot Error - Source Texture is nullptr\n" );
		__debugbreak( );
	}

	RE_HANDLE destHandle = FindTexture( destTextureName );

	if ( destHandle == INVALID_HANDLE )
	{
		destHandle = CreateCloneTexture( srcHandle, destTextureName );
		if ( destHandle == INVALID_HANDLE )
		{
			DebugWarning( "Snapshot Error - Fail Create DestTexture \n" );
			__debugbreak( );
		}
	}

	CopyResource( destHandle, nullptr, srcHandle, nullptr );
}

void CD3D11ResourceManager::OnDeviceLost( )
{
	m_depthStencils.clear( );
	m_renderTargets.clear( );
	m_shaderResources.clear( );
	m_randomAccessResource.clear( );
	m_textures.clear( );
	m_buffers.clear( );
	m_vertexShaders.clear( );
	m_pixelShaders.clear( );
	m_computeShaders.clear( );
	m_samplerStates.clear( );
	m_rasterizerStates.clear( );
	m_blendStates.clear( );
	m_depthStencilStates.clear( );

	m_dsvLUT.clear( );
	m_rtvLUT.clear( );
	m_srvLUT.clear( );
	m_ravLUT.clear( );
	m_texLUT.clear( );
	m_vertexShaderLUT.clear( );
	m_pixelShaderLUT.clear( );
	m_computeShaderLUT.clear( );
	m_samplerStateLUT.clear( );
	m_rasterizerStateLUT.clear( );
	m_blendStateLUT.clear( );
	m_depthStencilStateLUT.clear( );
}

void CD3D11ResourceManager::OnDeviceRestore( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	LoadShader( );
}

const CDepthStencil& CD3D11ResourceManager::GetDepthstencil( RE_HANDLE handle ) const
{
	assert( IsDepthStencilHandle( handle ) );
	return m_depthStencils[handle & RE_INDEX_MASK];
}

const CRenderTarget& CD3D11ResourceManager::GetRendertarget( RE_HANDLE handle ) const
{
	assert( IsRenderTargetHandle( handle ) );
	return m_renderTargets[handle & RE_INDEX_MASK];
}

const CD3D11ShaderResource& CD3D11ResourceManager::GetShaderResource( RE_HANDLE handle ) const
{
	assert( IsShaderResourceHandle( handle ) );
	return m_shaderResources[handle & RE_INDEX_MASK];
}

const CD3D11RandomAccessResource& CD3D11ResourceManager::GetRandomAccess( RE_HANDLE handle ) const
{
	assert( IsRandomAccessHandle( handle ) );
	return m_randomAccessResource[handle & RE_INDEX_MASK];
}

const CD3D11Texture& CD3D11ResourceManager::GetTexture( RE_HANDLE handle ) const
{
	assert( IsTexutreHandle( handle ) );
	return *m_textures[handle & RE_INDEX_MASK].get();
}

const CD3D11Buffer& CD3D11ResourceManager::GetBuffer( RE_HANDLE handle ) const
{
	assert( IsBufferHandle( handle ) );
	return m_buffers[handle & RE_INDEX_MASK];
}

const CD3D11VertexShader& CD3D11ResourceManager::GetVertexShader( RE_HANDLE handle ) const
{
	assert( IsVertexShaderHandle( handle ) );
	return m_vertexShaders[handle & RE_INDEX_MASK];
}

const CD3D11PixelShader& CD3D11ResourceManager::GetPixelShader( RE_HANDLE handle ) const
{
	assert( IsPixelShaderHandle( handle ) );
	return m_pixelShaders[handle & RE_INDEX_MASK];
}

const CD3D11ComputeShader& CD3D11ResourceManager::GetComputeShader( RE_HANDLE handle ) const
{
	assert( IsComputeShaderHandle( handle ) );
	return m_computeShaders[handle & RE_INDEX_MASK];
}

const CD3D11SamplerState& CD3D11ResourceManager::GetSamplerState( RE_HANDLE handle ) const
{
	assert( IsSamplerStateHandle( handle ) );
	return m_samplerStates[handle & RE_INDEX_MASK];
}

const CD3D11RasterizerState& CD3D11ResourceManager::GetRasterizerState( RE_HANDLE handle ) const
{
	assert( IsRasterizerStateHandle( handle ) );
	return m_rasterizerStates[handle & RE_INDEX_MASK];
}

const CD3D11BlendState& CD3D11ResourceManager::GetBlendState( RE_HANDLE handle ) const
{
	assert( IsBlendStateHandle( handle ) );
	return m_blendStates[handle & RE_INDEX_MASK];
}

const CD3D11DepthStencilState& CD3D11ResourceManager::GetDepthStencilState( RE_HANDLE handle ) const
{
	assert( IsDepthStencilStateHandle( handle ) );
	return m_depthStencilStates[handle & RE_INDEX_MASK];
}

ID3D11Resource* CD3D11ResourceManager::GetD3D11ResourceGeneric( RE_HANDLE handle ) const
{
	if ( IsTexutreHandle( handle ) )
	{
		const CD3D11Texture& texture = GetTexture( handle );
		return texture.Get( );
	}
	else if ( IsBufferHandle( handle ) )
	{
		const CD3D11Buffer& buffer = GetBuffer( handle );
		return buffer.Get( );
	}

	return nullptr;
}

bool CD3D11ResourceManager::LoadShader( )
{
	CKeyValueReader scrpitReader;
	std::unique_ptr<KeyValue> pKeyValues = scrpitReader.LoadKeyValueFromFile( _T( "../Script/ShaderList.txt" ) );

	if ( pKeyValues )
	{
		bool result = false;

		for ( const KeyValue* shaderList = pKeyValues->GetChild( ); shaderList != nullptr; shaderList = shaderList->GetNext( ) )
		{
			const String& filePath = shaderList->GetKey( );
			const String& profile = shaderList->GetValue( );

			const String& shaderType = profile.substr( 0, 2 );
			char mbsProfile[256];
			size_t converted;
			wcstombs_s( &converted, mbsProfile, profile.c_str( ), 256 );

			if ( shaderType.compare( _T( "vs" ) ) == 0 )
			{
				result = ( CreateVertexShader( filePath.c_str( ), mbsProfile ) != INVALID_HANDLE );
			}
			else if ( shaderType.compare( _T( "ps" ) ) == 0 )
			{
				result = ( CreatePixelShader( filePath.c_str( ), mbsProfile ) != INVALID_HANDLE );
			}

			if ( result == false )
			{
				return false;
			}
		}
	}

	return true;
}
