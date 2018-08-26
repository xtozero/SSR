#include "stdafx.h"
#include "D3D11/D3D11ResourceManager.h"

#include "common.h"
#include "D3D11/D3D11RenderState.h"
#include "D3D11/D3D11Resource.h"
#include "DataStructure/EnumStringMap.h"
#include "DataStructure/KeyValueReader.h"
#include "Util.h"

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

			hr = D3DCompile( buffer,
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

	template <typename T, int RESOURCE_TYPE>
	RE_HANDLE CreateShader( ID3D11Device& device, const String& fileName, const TCHAR* pFilePath, const char* pProfile, std::vector<T>& shaders, std::map<String, RE_HANDLE>& lut )
	{
		T shader;
		bool result = false;
		CShaderByteCode byteCode = GetCompiledByteCode( pFilePath );

		if ( byteCode.GetBufferSize( ) > 0 )
		{
			result = shader.CreateShader( device, byteCode.GetBufferPointer( ), byteCode.GetBufferSize( ) );
		}
		else if ( Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = GetShaderBlob( pFilePath, pProfile ) )
		{
			result = shader.CreateShader( device, shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ) );
		}

		if ( result )
		{
			shaders.emplace_back( std::move( shader ) );

			int idx = static_cast<int>( shaders.size( ) ) - 1;
			RE_HANDLE handle = MakeResourceHandle( RESOURCE_TYPE, idx );

			lut.emplace( fileName, handle );
			return handle;
		}

		return INVALID_HANDLE;
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

	return true;
}

RE_HANDLE CD3D11ResourceManager::LoadTextureFromFile( const String& fileName )
{
	TCHAR pPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, pPath );
	::SetCurrentDirectory( DEFAULT_TEXTURE_FILE_PATH );

	D3DX11_IMAGE_INFO info;
	HRESULT hr;
	D3DX11GetImageInfoFromFile( fileName.c_str( ), nullptr, &info, &hr );

	CD3D11Texture* newTexture = nullptr;
	if ( m_freeTexture != nullptr )
	{
		PopFrontInPlaceList( &m_freeTexture, &newTexture );
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
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			{
				if ( newTexture->LoadFromFile( *m_pDevice, fileName.c_str( ) ) == false )
				{
					PushFrontInPlaceList( &m_freeTexture, newTexture );
					return INVALID_HANDLE;
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
	RE_HANDLE handle = MakeResourceHandle( TEXTURE_HANDLE, idx );
	m_texLUT.emplace( fileName, handle );

	::SetCurrentDirectory( pPath );
	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture1D( TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData )
{
	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}
	
	CD3D11Texture* newTexture = nullptr;
	if ( m_freeTexture != nullptr )
	{
		PopFrontInPlaceList( &m_freeTexture, &newTexture );
	}
	else
	{
		m_textures.emplace_back();
		newTexture = &m_textures.back( );
	}

	if ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
	{
		trait.m_width = m_frameBufferSize.first;
		trait.m_height = m_frameBufferSize.second;
	}

	if ( newTexture->Create( *m_pDevice, TEXTURE_TYPE::TEXTURE_1D, trait, initData ) )
	{
		int idx = ( newTexture - &m_textures.front() );
		RE_HANDLE handle = MakeResourceHandle( TEXTURE_HANDLE, idx );

		m_texLUT.emplace( textureName, handle );
		return handle;
	}
	else
	{
		PushFrontInPlaceList( &m_freeTexture, newTexture );
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
		return CreateTexture1D( trait->second, textureName, initData );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture2D( TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData )
{
	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}

	CD3D11Texture* newTexture = nullptr;
	if ( m_freeTexture != nullptr )
	{
		PopFrontInPlaceList( &m_freeTexture, &newTexture );
	}
	else
	{
		m_textures.emplace_back( );
		newTexture = &m_textures.back( );
	}

	if ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
	{
		trait.m_width = m_frameBufferSize.first;
		trait.m_height = m_frameBufferSize.second;
	}

	if ( newTexture->Create( *m_pDevice, TEXTURE_TYPE::TEXTURE_2D, trait, initData ) )
	{
		int idx = ( newTexture - &m_textures.front( ) );
		RE_HANDLE handle = MakeResourceHandle( TEXTURE_HANDLE, idx );

		m_texLUT.emplace( textureName, handle );
		return handle;
	}
	else
	{
		PushFrontInPlaceList( &m_freeTexture, newTexture );
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

	CD3D11Texture* newTexture = nullptr;
	if ( m_freeTexture != nullptr )
	{
		PopFrontInPlaceList( &m_freeTexture, &newTexture );
	}
	else
	{
		m_textures.emplace_back( );
		newTexture = &m_textures.back( );
	}

	Microsoft::WRL::ComPtr<ID3D11Resource> texture = static_cast<ID3D11Resource*>( pTexture );
	newTexture->SetTexture( texture, isAppSizeDependent );

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

			PushFrontInPlaceList( &m_freeDepthStencil, &m_depthStencils[index] );
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
			m_renderTargets[index].Free( );

			PushFrontInPlaceList( &m_freeRenderTarget, &m_renderTargets[index] );
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
			m_shaderResources[index].Free( );

			PushFrontInPlaceList( &m_freeShaderResource, &m_shaderResources[index] );
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
			m_randomAccessResource[index].Free( );

			PushFrontInPlaceList( &m_freeRandomAccess, &m_randomAccessResource[index] );
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
			m_textures[index].Free( );

			PushFrontInPlaceList( &m_freeTexture, &m_textures[index] );
		}
		else
		{
			++iter;
		}
	}
}

RE_HANDLE CD3D11ResourceManager::CreateBuffer( const BUFFER_TRAIT& trait )
{
	CD3D11Buffer* pBuffer = nullptr;

	if ( m_freeBuffer != nullptr )
	{
		PopFrontInPlaceList( &m_freeBuffer, &pBuffer );
	}
	else
	{
		m_buffers.emplace_back( );
		pBuffer = &m_buffers.back( );
	}

	if ( pBuffer->Create( *m_pDevice, trait ) )
	{
		int idx = ( pBuffer - &m_buffers.front() );
		return MakeResourceHandle( BUFFER_HANDLE, idx );
	}
	else
	{
		PushFrontInPlaceList( &m_freeBuffer, pBuffer );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateVertexShader( const TCHAR* pFilePath, const char* pProfile, const VERTEX_LAYOUT* layoutOrNull, int layoutSize )
{
	String fileName = UTIL::GetFileName( pFilePath );
	RE_HANDLE found = FindGraphicsShaderByName( fileName.c_str( ) );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11VertexShader vs;
	D3D11_INPUT_ELEMENT_DESC inputDesc[16];
	if ( ( layoutSize > 0 ) && ( layoutOrNull != nullptr ) )
	{
		assert( layoutSize <= 16 );
		ConvertVertexLayoutToInputLayout( inputDesc, layoutOrNull, layoutSize );
	}

	bool result = false;
	CShaderByteCode byteCode = GetCompiledByteCode( pFilePath );
	if ( byteCode.GetBufferSize( ) > 0 )
	{
		result = vs.CreateShader( *m_pDevice, byteCode.GetBufferPointer( ), byteCode.GetBufferSize( ), inputDesc, layoutSize );
	}
	else if ( Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = GetShaderBlob( pFilePath, pProfile ) )
	{
		result = vs.CreateShader( *m_pDevice, shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ), inputDesc, layoutSize );
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

RE_HANDLE CD3D11ResourceManager::CreateGeometryShader( const TCHAR* pFilePath, const char* pProfile )
{
	String fileName = UTIL::GetFileName( pFilePath );
	RE_HANDLE found = FindGraphicsShaderByName( fileName.c_str( ) );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	return CreateShader<CD3D11GeometryShader, GS_HANDLE>( *m_pDevice, fileName, pFilePath, pProfile, m_geometryShaders, m_geometryShaderLUT );
}

RE_HANDLE CD3D11ResourceManager::CreatePixelShader( const TCHAR* pFilePath, const char* pProfile )
{
	String fileName = UTIL::GetFileName( pFilePath );
	RE_HANDLE found = FindGraphicsShaderByName( fileName.c_str( ) );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	return CreateShader<CD3D11PixelShader, PS_HANDLE>( *m_pDevice, fileName, pFilePath, pProfile, m_pixelShaders, m_pixelShaderLUT );
}

RE_HANDLE CD3D11ResourceManager::CreateComputeShader( const TCHAR* pFilePath, const char* pProfile )
{
	String fileName = UTIL::GetFileName( pFilePath );
	RE_HANDLE found = FindComputeShaderByName( fileName.c_str( ) );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	return CreateShader<CD3D11ComputeShader, CS_HANDLE>( *m_pDevice, fileName, pFilePath, pProfile, m_computeShaders, m_computeShaderLUT );
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

	found = m_geometryShaderLUT.find( pName );
	if ( found != m_geometryShaderLUT.end( ) )
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
		PopFrontInPlaceList( &m_freeRenderTarget, &newRenderTarget );
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
		PushFrontInPlaceList( &m_freeRenderTarget, newRenderTarget );
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
		PopFrontInPlaceList( &m_freeDepthStencil, &newDepthStencil );
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
		PushFrontInPlaceList( &m_freeDepthStencil, newDepthStencil );
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
		PopFrontInPlaceList( &m_freeShaderResource, &newShaderResource );
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
		PushFrontInPlaceList( &m_freeShaderResource, newShaderResource );
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
	assert( IsTextureHandle( texHandle ) );
	RE_HANDLE found = FindShaderResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11ShaderResource* newResource = nullptr;
	if ( m_freeShaderResource != nullptr )
	{
		PopFrontInPlaceList( &m_freeShaderResource, &newResource );
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
		PushFrontInPlaceList( &m_freeShaderResource, newResource );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateBufferShaderResource( RE_HANDLE bufHandle, const String& resourceName, const BUFFER_TRAIT* trait )
{
	assert( bufHandle != INVALID_HANDLE );
	assert( IsBufferHandle( bufHandle ) );
	RE_HANDLE found = FindShaderResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11ShaderResource* newResource = nullptr;
	if ( m_freeShaderResource != nullptr )
	{
		PopFrontInPlaceList( &m_freeShaderResource, &newResource );
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
		PushFrontInPlaceList( &m_freeShaderResource, newResource );
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
		PopFrontInPlaceList( &m_freeShaderResource, &newResource );
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
	assert( IsTextureHandle( texHandle ) );
	RE_HANDLE found = FindRandomAccessResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11RandomAccessResource* newResource = nullptr;
	if ( m_freeRandomAccess != nullptr )
	{
		PopFrontInPlaceList( &m_freeRandomAccess, &newResource );
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
		PushFrontInPlaceList( &m_freeRandomAccess, newResource );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateBufferRandomAccess( RE_HANDLE bufHandle, const String& resourceName, const BUFFER_TRAIT* trait )
{
	assert( bufHandle != INVALID_HANDLE );
	assert( IsBufferHandle( bufHandle ) );
	RE_HANDLE found = FindRandomAccessResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11RandomAccessResource* newResource = nullptr;
	if ( m_freeRandomAccess != nullptr )
	{
		PopFrontInPlaceList( &m_freeRandomAccess, &newResource );
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
		PushFrontInPlaceList( &m_freeRandomAccess, newResource );
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
	assert( IsTextureHandle( texHandle ) );
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
	ID3D11Resource* pDest = GetD3D11ResourceGeneric( dest );
	ID3D11Resource* pSrc = GetD3D11ResourceGeneric( src );

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

void CD3D11ResourceManager::FreeResource( RE_HANDLE resourceHandle )
{
	if ( resourceHandle == INVALID_HANDLE )
	{
		return;
	}

	unsigned int resourceType = resourceHandle & RE_TYPE_MASK;
	int resourceIdx = resourceHandle & RE_INDEX_MASK;

	switch ( resourceType )
	{
	case BUFFER_HANDLE:
		m_buffers[resourceIdx].~CD3D11Buffer( );
		PushFrontInPlaceList( &m_freeBuffer, &m_buffers[resourceIdx] );
		break;
	case DEPTH_STENCIL_HANDLE:
		m_depthStencils[resourceIdx].~CDepthStencil( );
		PushFrontInPlaceList( &m_freeDepthStencil, &m_depthStencils[resourceIdx] );
		break;
	case RENDER_TARGET_HANDLE:
		m_renderTargets[resourceIdx].~CRenderTarget( );
		PushFrontInPlaceList( &m_freeRenderTarget, &m_renderTargets[resourceIdx] );
		break;
	case SHADER_RESOURCE_HANDLE:
		m_shaderResources[resourceIdx].~CD3D11ShaderResource( );
		PushFrontInPlaceList( &m_freeShaderResource, &m_shaderResources[resourceIdx] );
		break;
	case RANDOM_ACCESS_HANDLE:
		break;
	case TEXTURE_HANDLE:
		m_textures[resourceIdx].~CD3D11Texture( );
		PushFrontInPlaceList( &m_freeTexture, &m_textures[resourceIdx] );
		break;
	case VS_HANDLE:
		break;
	case GS_HANDLE:
		break;
	case PS_HANDLE:
		break;
	case CS_HANDLE:
		break;
	case SAMPLER_STATE_HANDLE:
		break;
	case RASTERIZER_STATE_HANDLE:
		break;
	case BLEND_STATE_HANDLE:
		break;
	case DEPTH_STENCIL_STATE_HANDLE:
		break;
	default:
		assert( false && "invalid resource handle" );
		break;
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

	m_freeDepthStencil = nullptr;
	m_freeRenderTarget = nullptr;
	m_freeShaderResource = nullptr;
	m_freeRandomAccess = nullptr;
	m_freeTexture = nullptr;

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
	assert( IsTextureHandle( handle ) );
	return m_textures[handle & RE_INDEX_MASK];
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

const CD3D11GeometryShader& CD3D11ResourceManager::GetGeometryShader( RE_HANDLE handle ) const
{
	assert( IsGeometryShaderHandle( handle ) );
	return m_geometryShaders[handle & RE_INDEX_MASK];
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
	if ( IsTextureHandle( handle ) )
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
			const String& profile = shaderList->GetKey( );

			const String& shaderType = profile.substr( 0, 2 );
			char mbsProfile[256];
			size_t converted;
			wcstombs_s( &converted, mbsProfile, profile.c_str( ), 256 );

			if ( shaderType.compare( _T( "vs" ) ) == 0 )
			{
				if ( shaderList->GetValue( ).empty( ) )
				{
					result = CreateVertexShaderFromScript( shaderList, mbsProfile );
				}
				else
				{
					const String& filePath = shaderList->GetValue( );
					result = ( CreateVertexShader( filePath.c_str( ), mbsProfile ) != INVALID_HANDLE );
				}
			}
			else if ( shaderType == _T("gs") )
			{
				const String& filePath = shaderList->GetValue( );
				result = ( CreateGeometryShader( filePath.c_str( ), mbsProfile ) != INVALID_HANDLE );
			}
			else if ( shaderType.compare( _T( "ps" ) ) == 0 )
			{
				const String& filePath = shaderList->GetValue( );
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

bool CD3D11ResourceManager::CreateVertexShaderFromScript( const KeyValue* desc, const char* profile )
{
	const KeyValue* path = desc->Find( _T( "path" ) );
	int layoutSize = 0;
	VERTEX_LAYOUT vl[16] = {};
	if ( const KeyValue* layout = desc->Find( _T( "layout" ) ) )
	{
		for ( const KeyValue* elem = layout->GetChild( ); elem != nullptr; elem = elem->GetNext( ) )
		{
			char mbsPath[256];
			size_t converted;
			wcstombs_s( &converted, mbsPath, elem->GetKey().c_str( ), 256 );
			vl[layoutSize].m_name = mbsPath;

			std::vector<String> options;
			UTIL::Split( elem->GetValue( ), options, _T( ' ' ) );

			vl[layoutSize].m_index = _ttoi( options[0].c_str() );
			vl[layoutSize].m_format = GetEnumStringMap( ).GetEnum( options[1].c_str( ), RESOURCE_FORMAT::UNKNOWN );
			vl[layoutSize].m_slot = _ttoi( options[2].c_str( ) );
			vl[layoutSize].m_isInstanceData = ( _ttoi( options[3].c_str( ) ) == 1 );
			vl[layoutSize].m_instanceDataStep = _ttoi( options[4].c_str( ) );

			++layoutSize;
		}
	}
	
	return ( CreateVertexShader( path->GetValue( ).c_str( ), profile, vl, layoutSize ) != INVALID_HANDLE );
}
