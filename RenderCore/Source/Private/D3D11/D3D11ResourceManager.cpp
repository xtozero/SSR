#include "stdafx.h"
#include "D3D11/D3D11ResourceManager.h"

#include "common.h"
#include "D3D11/D3D11RenderState.h"
#include "D3D11/D3D11Resource.h"
#include "DataStructure/EnumStringMap.h"
#include "DataStructure/KeyValueReader.h"
#include "Util.h"

#include <cstddef>
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
				trait.m_bindType |= static_cast<D3D11_USAGE>( GetEnumStringMap( ).GetEnum( type, RESOURCE_BIND_TYPE::SHADER_RESOURCE ) );
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

	template <typename T, int RE_HANDLE_TYPE>
	RE_HANDLE CreateShader( ID3D11Device& device, const String& fileName, const TCHAR* pFilePath, const char* pProfile, T& shader, std::vector<T>& shaders, std::map<String, RE_HANDLE>& lut )
	{
		bool result = false;
		CShaderByteCode byteCode = GetCompiledByteCode( pFilePath );

		if ( byteCode.GetBufferSize( ) > 0 )
		{
			result = shader.CreateShader( device, fileName, byteCode.GetBufferPointer( ), byteCode.GetBufferSize( ) );
		}
		else if ( Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = GetShaderBlob( pFilePath, pProfile ) )
		{
			result = shader.CreateShader( device, fileName, shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ) );
		}

		if ( result )
		{
			shader.SetName( fileName );

			std::size_t idx = ( &shader - &shaders.front( ) );
			RE_HANDLE handle = MakeResourceHandle( RE_HANDLE_TYPE, idx );

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
	using namespace RESOURCE_TYPE;

	TCHAR pPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, pPath );
	::SetCurrentDirectory( DEFAULT_TEXTURE_FILE_PATH );

	D3DX11_IMAGE_INFO info;
	HRESULT hr;
	D3DX11GetImageInfoFromFile( fileName.c_str( ), nullptr, &info, &hr );

	CD3D11Texture* newTexture = nullptr;
	if ( m_freeResourceList[TEXTURE] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[TEXTURE], &newTexture );
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
					PushFrontInPlaceList( &m_freeResourceList[TEXTURE], newTexture );
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

	std::size_t idx = ( newTexture - &m_textures.front( ) );
	RE_HANDLE handle = MakeResourceHandle( TEXTURE_HANDLE, idx );
	m_resourceLUT[TEXTURE].emplace( fileName, handle );

	::SetCurrentDirectory( pPath );
	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture1D( TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData )
{
	using namespace RESOURCE_TYPE;

	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}
	
	CD3D11Texture* newTexture = nullptr;
	if ( m_freeResourceList[TEXTURE] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[TEXTURE], &newTexture );
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

	if ( newTexture->Create( *m_pDevice, textureName, TEXTURE_TYPE::TEXTURE_1D, trait, initData ) )
	{
		std::size_t idx = ( newTexture - &m_textures.front() );
		RE_HANDLE handle = MakeResourceHandle( TEXTURE_HANDLE, idx );

		m_resourceLUT[TEXTURE].emplace( textureName, handle );
		return handle;
	}
	else
	{
		PushFrontInPlaceList( &m_freeResourceList[TEXTURE], newTexture );
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
	using namespace RESOURCE_TYPE;

	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}

	CD3D11Texture* newTexture = nullptr;
	if ( m_freeResourceList[TEXTURE] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[TEXTURE], &newTexture );
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

	if ( newTexture->Create( *m_pDevice, textureName, TEXTURE_TYPE::TEXTURE_2D, trait, initData ) )
	{
		std::size_t idx = ( newTexture - &m_textures.front( ) );
		RE_HANDLE handle = MakeResourceHandle( TEXTURE_HANDLE, idx );

		m_resourceLUT[TEXTURE].emplace( textureName, handle );
		return handle;
	}
	else
	{
		PushFrontInPlaceList( &m_freeResourceList[TEXTURE], newTexture );
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

RE_HANDLE CD3D11ResourceManager::CreateTexture3D( TEXTURE_TRAIT & trait, const String & textureName, const RESOURCE_INIT_DATA * initData )
{
	using namespace RESOURCE_TYPE;

	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}

	CD3D11Texture* newTexture = nullptr;
	if ( m_freeResourceList[TEXTURE] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[TEXTURE], &newTexture );
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

	if ( newTexture->Create( *m_pDevice, textureName, TEXTURE_TYPE::TEXTURE_3D, trait, initData ) )
	{
		std::size_t idx = ( newTexture - &m_textures.front( ) );
		RE_HANDLE handle = MakeResourceHandle( TEXTURE_HANDLE, idx );

		m_resourceLUT[TEXTURE].emplace( textureName, handle );
		return handle;
	}
	else
	{
		PushFrontInPlaceList( &m_freeResourceList[TEXTURE], newTexture );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture3D( const String & descName, const String & textureName, const RESOURCE_INIT_DATA * initData )
{
	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}

	auto trait = m_textureTraits.find( descName );
	if ( trait != m_textureTraits.end( ) )
	{
		return CreateTexture3D( trait->second, textureName, initData );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::RegisterTexture2D( const String& textureName, void* pTexture, bool isAppSizeDependent )
{
	using namespace RESOURCE_TYPE;

	RE_HANDLE found = FindTexture( textureName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}

	CD3D11Texture* newTexture = nullptr;
	if ( m_freeResourceList[TEXTURE] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[TEXTURE], &newTexture );
	}
	else
	{
		m_textures.emplace_back( );
		newTexture = &m_textures.back( );
	}

	Microsoft::WRL::ComPtr<ID3D11Resource> texture = static_cast<ID3D11Resource*>( pTexture );
	newTexture->SetTexture( texture, isAppSizeDependent );

	std::size_t idx = ( newTexture - &m_textures.front( ) );
	RE_HANDLE handle = MakeResourceHandle( TEXTURE_HANDLE, idx );

	m_resourceLUT[TEXTURE].emplace( textureName, handle );
	return handle;
}

RE_HANDLE CD3D11ResourceManager::FindTexture( const String& textureName ) const
{
	using namespace RESOURCE_TYPE;

	auto found = m_resourceLUT[TEXTURE].find( textureName );

	if ( found != m_resourceLUT[TEXTURE].end( ) )
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
	using namespace RESOURCE_TYPE;

	m_frameBufferSize.first = nWndWidth;
	m_frameBufferSize.second = nWndHeight;

	auto iter = m_resourceLUT[DEPTH_STENCIL].begin( );
	RE_HANDLE handle = INVALID_HANDLE;

	while ( iter != m_resourceLUT[DEPTH_STENCIL].end( ) )
	{
		handle = iter->second;
		const CD3D11DepthStencil& dsv = GetDepthstencil( iter->second );
		if ( dsv.IsAppSizeDependency() )
		{
			iter = m_resourceLUT[DEPTH_STENCIL].erase( iter );
			int index = handle & RE_INDEX_MASK;
			m_depthStencils[index].Free( );

			PushFrontInPlaceList( &m_freeResourceList[DEPTH_STENCIL], &m_depthStencils[index] );
		}
		else
		{
			++iter;
		}
	}

	iter = m_resourceLUT[RENDER_TARGET].begin( );
	while ( iter != m_resourceLUT[RENDER_TARGET].end( ) )
	{
		handle = iter->second;
		const CD3D11RenderTarget& rtv = GetRendertarget( iter->second );
		if ( rtv.IsAppSizeDependency( ) )
		{
			iter = m_resourceLUT[RENDER_TARGET].erase( iter );
			int index = handle & RE_INDEX_MASK;
			m_renderTargets[index].Free( );

			PushFrontInPlaceList( &m_freeResourceList[RENDER_TARGET], &m_renderTargets[index] );
		}
		else
		{
			++iter;
		}
	}

	iter = m_resourceLUT[SHADER_RESOURCE].begin( );
	while ( iter != m_resourceLUT[SHADER_RESOURCE].end( ) )
	{
		handle = iter->second;
		const CD3D11ShaderResource& srv = GetShaderResource( iter->second );
		if ( srv.IsAppSizeDependency( ) )
		{
			iter = m_resourceLUT[SHADER_RESOURCE].erase( iter );
			int index = handle & RE_INDEX_MASK;
			m_shaderResources[index].Free( );

			PushFrontInPlaceList( &m_freeResourceList[SHADER_RESOURCE], &m_shaderResources[index] );
		}
		else
		{
			++iter;
		}
	}

	iter = m_resourceLUT[RANDOM_ACCESS].begin( );
	while ( iter != m_resourceLUT[RANDOM_ACCESS].end( ) )
	{
		handle = iter->second;
		const CD3D11RandomAccessResource& rav = GetRandomAccess( iter->second );
		if ( rav.IsAppSizeDependency( ) )
		{
			iter = m_resourceLUT[RANDOM_ACCESS].erase( iter );
			int index = handle & RE_INDEX_MASK;
			m_randomAccessResource[index].Free( );

			PushFrontInPlaceList( &m_freeResourceList[RANDOM_ACCESS], &m_randomAccessResource[index] );
		}
		else
		{
			++iter;
		}
	}

	iter = m_resourceLUT[TEXTURE].begin( );
	while ( iter != m_resourceLUT[TEXTURE].end( ) )
	{
		handle = iter->second;
		const ITexture& tex = GetTexture( iter->second );
		if ( tex.IsAppSizeDependency( ) )
		{
			iter = m_resourceLUT[TEXTURE].erase( iter );
			int index = handle & RE_INDEX_MASK;
			m_textures[index].Free( );

			PushFrontInPlaceList( &m_freeResourceList[TEXTURE], &m_textures[index] );
		}
		else
		{
			++iter;
		}
	}
}

RE_HANDLE CD3D11ResourceManager::CreateBuffer( const BUFFER_TRAIT& trait )
{
	using namespace RESOURCE_TYPE;

	CD3D11Buffer* pBuffer = nullptr;

	if ( m_freeResourceList[BUFFER] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[BUFFER], &pBuffer );
	}
	else
	{
		m_buffers.emplace_back( );
		pBuffer = &m_buffers.back( );
	}

	if ( pBuffer->Create( *m_pDevice, trait ) )
	{
		std::size_t idx = ( pBuffer - &m_buffers.front() );
		return MakeResourceHandle( BUFFER_HANDLE, idx );
	}
	else
	{
		PushFrontInPlaceList( &m_freeResourceList[BUFFER], pBuffer );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateVertexShader( const TCHAR* pFilePath, const char* pProfile, const VERTEX_LAYOUT* layoutOrNull, int layoutSize )
{
	using namespace RESOURCE_TYPE;

	String fileName = UTIL::GetFileName( pFilePath );
	auto found = m_resourceLUT[VERTEX_SHADER].find( fileName );
	if ( found != m_resourceLUT[VERTEX_SHADER].end( ) )
	{
		return found->second;
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
		result = vs.CreateShader( *m_pDevice, fileName, byteCode.GetBufferPointer( ), byteCode.GetBufferSize( ), inputDesc, layoutSize );
	}
	else if ( Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = GetShaderBlob( pFilePath, pProfile ) )
	{
		result = vs.CreateShader( *m_pDevice, fileName, shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ), inputDesc, layoutSize );
	}

	if ( result )
	{
		vs.SetName( fileName );
		m_vertexShaders.emplace_back( std::move( vs ) );

		int idx = static_cast<int>( m_vertexShaders.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( VS_HANDLE, idx );

		m_resourceLUT[VERTEX_SHADER].emplace( fileName, handle );
		return handle;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateGeometryShader( const TCHAR* pFilePath, const char* pProfile )
{
	using namespace RESOURCE_TYPE;

	String fileName = UTIL::GetFileName( pFilePath );
	auto found = m_resourceLUT[GEOMETRY_SHADER].find( fileName );
	if ( found != m_resourceLUT[GEOMETRY_SHADER].end() )
	{
		return found->second;
	}

	CD3D11GeometryShader* gs = nullptr;
	if ( m_freeResourceList[GEOMETRY_SHADER] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[GEOMETRY_SHADER], &gs );
	}
	else
	{
		m_geometryShaders.emplace_back( );
		gs = &m_geometryShaders.back( );
	}

	return CreateShader<CD3D11GeometryShader, GS_HANDLE>( *m_pDevice, fileName, pFilePath, pProfile, *gs, m_geometryShaders, m_resourceLUT[GEOMETRY_SHADER] );
}

RE_HANDLE CD3D11ResourceManager::CreatePixelShader( const TCHAR* pFilePath, const char* pProfile )
{
	using namespace RESOURCE_TYPE;

	String fileName = UTIL::GetFileName( pFilePath );
	auto found = m_resourceLUT[PIXEL_SHADER].find( fileName );
	if ( found != m_resourceLUT[PIXEL_SHADER].end( ) )
	{
		return found->second;
	}

	CD3D11PixelShader* ps = nullptr;
	if ( m_freeResourceList[PIXEL_SHADER] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[PIXEL_SHADER], &ps );
	}
	else
	{
		m_pixelShaders.emplace_back( );
		ps = &m_pixelShaders.back( );
	}

	return CreateShader<CD3D11PixelShader, PS_HANDLE>( *m_pDevice, fileName, pFilePath, pProfile, *ps, m_pixelShaders, m_resourceLUT[PIXEL_SHADER] );
}

RE_HANDLE CD3D11ResourceManager::CreateComputeShader( const TCHAR* pFilePath, const char* pProfile )
{
	using namespace RESOURCE_TYPE;

	String fileName = UTIL::GetFileName( pFilePath );
	auto found = m_resourceLUT[COMPUTE_SHADER].find( fileName );
	if ( found != m_resourceLUT[COMPUTE_SHADER].end( ) )
	{
		return found->second;
	}

	CD3D11ComputeShader* cs = nullptr;
	if ( m_freeResourceList[COMPUTE_SHADER] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[COMPUTE_SHADER], &cs );
	}
	else
	{
		m_computeShaders.emplace_back( );
		cs = &m_computeShaders.back( );
	}

	return CreateShader<CD3D11ComputeShader, CS_HANDLE>( *m_pDevice, fileName, pFilePath, pProfile, *cs, m_computeShaders, m_resourceLUT[COMPUTE_SHADER] );
}

RE_HANDLE CD3D11ResourceManager::FindGraphicsShaderByName( const TCHAR* pName )
{
	using namespace RESOURCE_TYPE;

	if ( !pName )
	{
		return INVALID_HANDLE;
	}

	for ( int i = VERTEX_SHADER; i < COMPUTE_SHADER; ++i )
	{
		auto found = m_resourceLUT[i].find( pName );
		if ( found != m_resourceLUT[i].end( ) )
		{
			return found->second;
		}
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::FindComputeShaderByName( const TCHAR* pName )
{
	using namespace RESOURCE_TYPE;

	if ( !pName )
	{
		return INVALID_HANDLE;
	}

	auto found = m_resourceLUT[COMPUTE_SHADER].find( pName );
	if ( found != m_resourceLUT[COMPUTE_SHADER].end( ) )
	{
		return found->second;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateRenderTarget( RE_HANDLE texhandle, const String& renderTargetName, const TEXTURE_TRAIT* trait )
{
	using namespace RESOURCE_TYPE;

	assert( texhandle != INVALID_HANDLE );
	RE_HANDLE found = FindRenderTarget( renderTargetName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11RenderTarget* newRenderTarget = nullptr;
	if ( m_freeResourceList[RENDER_TARGET] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[RENDER_TARGET], &newRenderTarget );
	}
	else
	{
		m_renderTargets.emplace_back( );
		newRenderTarget = &m_renderTargets.back( );
	}

	const CD3D11Texture& texture = GetTexture( texhandle );

	if ( newRenderTarget->CreateRenderTarget( *m_pDevice, renderTargetName, texture, trait ) )
	{
		std::size_t idx = ( newRenderTarget - &m_renderTargets.front( ) );
		RE_HANDLE handle = MakeResourceHandle( RENDER_TARGET_HANDLE, idx );

		m_resourceLUT[RENDER_TARGET].emplace( renderTargetName, handle );
		return handle;
	}
	else
	{
		PushFrontInPlaceList( &m_freeResourceList[RENDER_TARGET], newRenderTarget );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateDepthStencil( RE_HANDLE texhandle, const String& depthStencilName, const TEXTURE_TRAIT* trait )
{
	using namespace RESOURCE_TYPE;

	assert( texhandle != INVALID_HANDLE );
	RE_HANDLE found = FindDepthStencil( depthStencilName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11DepthStencil* newDepthStencil = nullptr;
	if ( m_freeResourceList[DEPTH_STENCIL] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[DEPTH_STENCIL], &newDepthStencil );
	}
	else
	{
		m_depthStencils.emplace_back( );
		newDepthStencil = &m_depthStencils.back( );
	}
	
	const CD3D11Texture& texture = GetTexture( texhandle );

	if ( newDepthStencil->CreateDepthStencil( *m_pDevice, depthStencilName, texture, trait ) )
	{
		std::size_t idx = ( newDepthStencil - &m_depthStencils.front() );
		RE_HANDLE handle = MakeResourceHandle( DEPTH_STENCIL_HANDLE, idx );

		m_resourceLUT[DEPTH_STENCIL].emplace( depthStencilName, handle );
		return handle;
	}
	else
	{
		PushFrontInPlaceList( &m_freeResourceList[DEPTH_STENCIL], newDepthStencil );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::FindRenderTarget( const String& renderTargetName ) const
{
	using namespace RESOURCE_TYPE;

	auto found = m_resourceLUT[RENDER_TARGET].find( renderTargetName );

	if ( found != m_resourceLUT[RENDER_TARGET].end( ) )
	{
		return found->second;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::FindDepthStencil( const String& depthStencilName ) const
{
	using namespace RESOURCE_TYPE;

	auto found = m_resourceLUT[DEPTH_STENCIL].find( depthStencilName );

	if ( found != m_resourceLUT[DEPTH_STENCIL].end( ) )
	{
		return found->second;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateShaderResourceFromFile( const String& fileName )
{
	using namespace RESOURCE_TYPE;

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
	if ( m_freeResourceList[SHADER_RESOURCE] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[SHADER_RESOURCE], &newShaderResource );
	}
	else
	{
		m_shaderResources.emplace_back( );
		newShaderResource = &m_shaderResources.back( );
	}
	
	if ( newShaderResource->CreateShaderResourceFromFile( *m_pDevice, fileName ) )
	{
		std::size_t idx = ( newShaderResource - &m_shaderResources.front( ) );
		handle = MakeResourceHandle( SHADER_RESOURCE_HANDLE, idx );

		m_resourceLUT[SHADER_RESOURCE].emplace( fileName, handle );
	}
	else
	{
		PushFrontInPlaceList( &m_freeResourceList[SHADER_RESOURCE], newShaderResource );
	}

	textureFile.close( );

	::SetCurrentDirectory( pPath );

	return handle;
}

RE_HANDLE CD3D11ResourceManager::FindShaderResource( const String& fileName ) const
{
	using namespace RESOURCE_TYPE;

	auto found = m_resourceLUT[SHADER_RESOURCE].find( fileName );

	if ( found != m_resourceLUT[SHADER_RESOURCE].end( ) )
	{
		return found->second;
	}
	
	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateTextureShaderResource( RE_HANDLE texHandle, const String& resourceName, const TEXTURE_TRAIT* trait )
{
	using namespace RESOURCE_TYPE;

	assert( texHandle != INVALID_HANDLE );
	assert( IsTextureHandle( texHandle ) );
	RE_HANDLE found = FindShaderResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11ShaderResource* newResource = nullptr;
	if ( m_freeResourceList[SHADER_RESOURCE] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[SHADER_RESOURCE], &newResource );
	}
	else
	{
		m_shaderResources.emplace_back( );
		newResource = &m_shaderResources.back( );
	}

	const CD3D11Texture& texture = GetTexture( texHandle );
	if ( newResource->CreateShaderResource( *m_pDevice, resourceName, texture, trait ) )
	{
		std::size_t idx = ( newResource - &m_shaderResources.front( ) );
		RE_HANDLE handle = MakeResourceHandle( SHADER_RESOURCE_HANDLE, idx );

		m_resourceLUT[SHADER_RESOURCE].emplace( resourceName, handle );

		return handle;
	}
	else
	{
		PushFrontInPlaceList( &m_freeResourceList[SHADER_RESOURCE], newResource );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateBufferShaderResource( RE_HANDLE bufHandle, const String& resourceName, const BUFFER_TRAIT* trait )
{
	using namespace RESOURCE_TYPE;

	assert( bufHandle != INVALID_HANDLE );
	assert( IsBufferHandle( bufHandle ) );
	RE_HANDLE found = FindShaderResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11ShaderResource* newResource = nullptr;
	if ( m_freeResourceList[SHADER_RESOURCE] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[SHADER_RESOURCE], &newResource );
	}
	else
	{
		m_shaderResources.emplace_back( );
		newResource = &m_shaderResources.back( );
	}

	const CD3D11Buffer& buffer = GetBuffer( bufHandle );
	if ( newResource->CreateShaderResource( *m_pDevice, resourceName, buffer, trait ) )
	{
		std::size_t idx = ( newResource - &m_shaderResources.front( ) );
		RE_HANDLE handle = MakeResourceHandle( SHADER_RESOURCE_HANDLE, idx );

		m_resourceLUT[SHADER_RESOURCE].emplace( resourceName, handle );

		return handle;
	}
	else
	{
		PushFrontInPlaceList( &m_freeResourceList[SHADER_RESOURCE], newResource );
	}

	return INVALID_HANDLE;
}

void CD3D11ResourceManager::RegisterShaderResource( const String& resourceName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srView )
{
	using namespace RESOURCE_TYPE;

	RE_HANDLE found = FindShaderResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		__debugbreak( );
	}

	CD3D11ShaderResource* newResource = nullptr;
	if ( m_freeResourceList[SHADER_RESOURCE] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[SHADER_RESOURCE], &newResource );
	}
	else
	{
		m_shaderResources.emplace_back( );
		newResource = &m_shaderResources.back( );
	}

	newResource->SetShaderResourceView( srView );

	std::size_t idx = ( newResource - &m_shaderResources.front( ) );
	RE_HANDLE handle = MakeResourceHandle( SHADER_RESOURCE_HANDLE, idx );

	m_resourceLUT[SHADER_RESOURCE].emplace( resourceName, handle );
}

RE_HANDLE CD3D11ResourceManager::FindRandomAccessResource( const String& resourceName ) const
{
	using namespace RESOURCE_TYPE;

	auto found = m_resourceLUT[RANDOM_ACCESS].find( resourceName );

	if ( found != m_resourceLUT[RANDOM_ACCESS].end( ) )
	{
		return found->second;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateTextureRandomAccess( RE_HANDLE texHandle, const String& resourceName, const TEXTURE_TRAIT* trait )
{
	using namespace RESOURCE_TYPE;

	assert( texHandle != INVALID_HANDLE );
	assert( IsTextureHandle( texHandle ) );
	RE_HANDLE found = FindRandomAccessResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11RandomAccessResource* newResource = nullptr;
	if ( m_freeResourceList[RANDOM_ACCESS] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[RANDOM_ACCESS], &newResource );
	}
	else
	{
		m_randomAccessResource.emplace_back( );
		newResource = &m_randomAccessResource.back( );
	}

	const CD3D11Texture& texture = GetTexture( texHandle );

	if ( newResource->CreateRandomAccessResource( *m_pDevice, resourceName, texture, trait ) )
	{
		std::size_t idx = ( newResource - &m_randomAccessResource.front( ) );
		RE_HANDLE handle = MakeResourceHandle( RANDOM_ACCESS_HANDLE, idx );

		m_resourceLUT[RANDOM_ACCESS].emplace( resourceName, handle );
		return handle;
	}
	else
	{
		PushFrontInPlaceList( &m_freeResourceList[RANDOM_ACCESS], newResource );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateBufferRandomAccess( RE_HANDLE bufHandle, const String& resourceName, const BUFFER_TRAIT* trait )
{
	using namespace RESOURCE_TYPE;

	assert( bufHandle != INVALID_HANDLE );
	assert( IsBufferHandle( bufHandle ) );
	RE_HANDLE found = FindRandomAccessResource( resourceName );
	if ( found != INVALID_HANDLE )
	{
		return found;
	}

	CD3D11RandomAccessResource* newResource = nullptr;
	if ( m_freeResourceList[RANDOM_ACCESS] != nullptr )
	{
		PopFrontInPlaceList( &m_freeResourceList[RANDOM_ACCESS], &newResource );
	}
	else
	{
		m_randomAccessResource.emplace_back( );
		newResource = &m_randomAccessResource.back( );
	}

	const CD3D11Buffer& buffer = GetBuffer( bufHandle );

	if ( newResource->CreateRandomAccessResource( *m_pDevice, resourceName, buffer, trait ) )
	{
		std::size_t idx = ( newResource - &m_randomAccessResource.front( ) );
		RE_HANDLE handle = MakeResourceHandle( RANDOM_ACCESS_HANDLE, idx );

		m_resourceLUT[RANDOM_ACCESS].emplace( resourceName, handle );
		return handle;
	}
	else
	{
		PushFrontInPlaceList( &m_freeResourceList[RANDOM_ACCESS], newResource );
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateSamplerState( const String& stateName )
{
	using namespace RESOURCE_TYPE;

	auto found = m_resourceLUT[SAMPLER_STATE].find( stateName );
	if ( found != m_resourceLUT[SAMPLER_STATE].end( ) )
	{
		return found->second;
	}

	CD3D11SamplerState* newState = m_SamplerFactory.GetSamplerState( *m_pDevice, stateName );
	if ( newState )
	{
		m_samplerStates.emplace_back( std::move( *newState ) );

		int idx = static_cast<int>( m_samplerStates.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( SAMPLER_STATE_HANDLE, idx );

		m_resourceLUT[SAMPLER_STATE].emplace( stateName, handle );

		return handle;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateRasterizerState( const String& stateName )
{
	using namespace RESOURCE_TYPE;

	auto found = m_resourceLUT[RASTERIZER_STATE].find( stateName );
	if ( found != m_resourceLUT[RASTERIZER_STATE].end( ) )
	{
		return found->second;
	}

	CD3D11RasterizerState* newState = m_RasterizerFactory.GetRasterizerState( *m_pDevice, stateName );
	if ( newState )
	{
		m_rasterizerStates.emplace_back( std::move( *newState ) );

		int idx = static_cast<int>( m_rasterizerStates.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( RASTERIZER_STATE_HANDLE, idx );

		m_resourceLUT[RASTERIZER_STATE].emplace( stateName, handle );

		return handle;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateBlendState( const String& stateName )
{
	using namespace RESOURCE_TYPE;

	auto found = m_resourceLUT[BLEND_STATE].find( stateName );
	if ( found != m_resourceLUT[BLEND_STATE].end( ) )
	{
		return found->second;
	}

	CD3D11BlendState* newState = m_BlendFactory.GetBlendState( *m_pDevice, stateName );
	if ( newState )
	{
		m_blendStates.emplace_back( std::move( *newState ) );

		int idx = static_cast<int>( m_blendStates.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( BLEND_STATE_HANDLE, idx );

		m_resourceLUT[BLEND_STATE].emplace( stateName, handle );

		return handle;
	}

	return INVALID_HANDLE;
}

RE_HANDLE CD3D11ResourceManager::CreateDepthStencilState( const String& stateName )
{
	using namespace RESOURCE_TYPE;

	auto found = m_resourceLUT[DEPTH_STENCIL_STATE].find( stateName );
	if ( found != m_resourceLUT[DEPTH_STENCIL_STATE].end( ) )
	{
		return found->second;
	}

	CD3D11DepthStencilState* newState = m_DepthStencilFactory.GetDepthStencilState( *m_pDevice, stateName );
	if ( newState )
	{
		m_depthStencilStates.emplace_back( std::move( *newState ) );
		
		int idx = static_cast<int>( m_depthStencilStates.size( ) ) - 1;
		RE_HANDLE handle = MakeResourceHandle( DEPTH_STENCIL_STATE_HANDLE, idx );

		m_resourceLUT[DEPTH_STENCIL_STATE].emplace( stateName, handle );

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
		trait.m_bindType |= RESOURCE_BIND_TYPE::SHADER_RESOURCE;
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

void CD3D11ResourceManager::UpdateResourceFromMemory( RE_HANDLE dest, void* src, UINT srcRowPitch, UINT srcDepthPitch, const RESOURCE_REGION* destRegionOrNull )
{
	ID3D11Resource* pDest = GetD3D11ResourceGeneric( dest );

	D3D11_BOX destBox = {};
	UINT destSubresouce = 0;
	if ( destRegionOrNull != nullptr )
	{
		destBox = { destRegionOrNull->m_left, destRegionOrNull->m_top, destRegionOrNull->m_front, destRegionOrNull->m_right, destRegionOrNull->m_bottom, destRegionOrNull->m_back };
		destSubresouce = destRegionOrNull->m_subResource;
	}

	m_pDeviceContext->UpdateSubresource( pDest, destSubresouce, destRegionOrNull ? &destBox : nullptr, src, srcRowPitch, srcDepthPitch );
}

void CD3D11ResourceManager::FreeResource( RE_HANDLE resourceHandle )
{
	using namespace RESOURCE_TYPE;

	if ( resourceHandle == INVALID_HANDLE )
	{
		return;
	}

	unsigned int resourceType = resourceHandle & RE_TYPE_MASK;
	int resourceIdx = resourceHandle & RE_INDEX_MASK;

	switch ( resourceType )
	{
	case BUFFER_HANDLE:
		{
			CD3D11Buffer& buffer = m_buffers[resourceIdx];
			buffer.~CD3D11Buffer( );
			PushFrontInPlaceList( &m_freeResourceList[BUFFER], &buffer );
		}
		break;
	case DEPTH_STENCIL_HANDLE:
		{
			CD3D11DepthStencil& depthStencil = m_depthStencils[resourceIdx];
			m_resourceLUT[DEPTH_STENCIL].erase( depthStencil.GetName( ) );
			depthStencil.~CD3D11DepthStencil( );
			PushFrontInPlaceList( &m_freeResourceList[DEPTH_STENCIL], &depthStencil );
		}
		break;
	case RENDER_TARGET_HANDLE:
		{
			CD3D11RenderTarget& renderTarget = m_renderTargets[resourceIdx];
			m_resourceLUT[RENDER_TARGET].erase( renderTarget.GetName( ) );
			renderTarget.~CD3D11RenderTarget( );
			PushFrontInPlaceList( &m_freeResourceList[RENDER_TARGET], &renderTarget );
		}
		break;
	case SHADER_RESOURCE_HANDLE:
		{
			CD3D11ShaderResource& shaderResource = m_shaderResources[resourceIdx];
			m_resourceLUT[SHADER_RESOURCE].erase( shaderResource.GetName( ) );
			shaderResource.~CD3D11ShaderResource( );
			PushFrontInPlaceList( &m_freeResourceList[SHADER_RESOURCE], &shaderResource );
		}
		break;
	case RANDOM_ACCESS_HANDLE:
		{
			CD3D11RandomAccessResource& randomAccess = m_randomAccessResource[resourceIdx];
			m_resourceLUT[RANDOM_ACCESS].erase( randomAccess.GetName( ) );
			randomAccess.~CD3D11RandomAccessResource( );
			PushFrontInPlaceList( &m_freeResourceList[RANDOM_ACCESS], &randomAccess );
		}
		break;
	case TEXTURE_HANDLE:
		{
			CD3D11Texture& texture = m_textures[resourceIdx];
			m_resourceLUT[TEXTURE].erase( texture.GetName( ) );
			texture.~CD3D11Texture( );
			PushFrontInPlaceList( &m_freeResourceList[TEXTURE], &texture );
		}
		break;
	case VS_HANDLE:
		{
			CD3D11VertexShader& vs = m_vertexShaders[resourceIdx];
			m_resourceLUT[VERTEX_SHADER].erase( vs.GetName( ) );
			vs.~CD3D11VertexShader( );
			PushFrontInPlaceList( &m_freeResourceList[VERTEX_SHADER], &vs );
		}
		break;
	case GS_HANDLE:
		{
			CD3D11GeometryShader& gs = m_geometryShaders[resourceIdx];
			m_resourceLUT[GEOMETRY_SHADER].erase( gs.GetName( ) );
			gs.~CD3D11GeometryShader( );
			PushFrontInPlaceList( &m_freeResourceList[GEOMETRY_SHADER], &gs );
		}
		break;
	case PS_HANDLE:
		{
			CD3D11PixelShader& ps = m_pixelShaders[resourceIdx];
			m_resourceLUT[PIXEL_SHADER].erase( ps.GetName( ) );
			ps.~CD3D11PixelShader( );
			PushFrontInPlaceList( &m_freeResourceList[PIXEL_SHADER], &ps );
		}
		break;
	case CS_HANDLE:
		{
			CD3D11ComputeShader& cs = m_computeShaders[resourceIdx];
			m_resourceLUT[COMPUTE_SHADER].erase( cs.GetName( ) );
			cs.~CD3D11ComputeShader( );
			PushFrontInPlaceList( &m_freeResourceList[COMPUTE_SHADER], &cs );
		}
		break;
	case SAMPLER_STATE_HANDLE:
		{
			CD3D11SamplerState& samplerState = m_samplerStates[resourceIdx];
			m_resourceLUT[SAMPLER_STATE].erase( samplerState.GetName( ) );
			samplerState.~CD3D11SamplerState( );
			PushFrontInPlaceList( &m_freeResourceList[SAMPLER_STATE], &samplerState );
		}
		break;
	case RASTERIZER_STATE_HANDLE:
		{
			CD3D11RasterizerState& rasterizerState = m_rasterizerStates[resourceIdx];
			m_resourceLUT[RASTERIZER_STATE].erase( rasterizerState.GetName( ) );
			rasterizerState.~CD3D11RasterizerState( );
			PushFrontInPlaceList( &m_freeResourceList[RASTERIZER_STATE], &rasterizerState );
		}
		break;
	case BLEND_STATE_HANDLE:
		{
			CD3D11BlendState& blendState = m_blendStates[resourceIdx];
			m_resourceLUT[BLEND_STATE].erase( blendState.GetName( ) );
			blendState.~CD3D11BlendState( );
			PushFrontInPlaceList( &m_freeResourceList[BLEND_STATE], &blendState );
		}
		break;
	case DEPTH_STENCIL_STATE_HANDLE:
		{
			CD3D11DepthStencilState& depthStencilState = m_depthStencilStates[resourceIdx];
			m_resourceLUT[DEPTH_STENCIL_STATE].erase( depthStencilState.GetName( ) );
			depthStencilState.~CD3D11DepthStencilState( );
			PushFrontInPlaceList( &m_freeResourceList[DEPTH_STENCIL_STATE], &depthStencilState );
		}
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
	CleanUpFreeResourceList( );

	m_depthStencils.clear( );
	m_renderTargets.clear( );
	m_shaderResources.clear( );
	m_randomAccessResource.clear( );
	m_textures.clear( );
	m_buffers.clear( );
	m_vertexShaders.clear( );
	m_geometryShaders.clear( );
	m_pixelShaders.clear( );
	m_computeShaders.clear( );
	m_samplerStates.clear( );
	m_rasterizerStates.clear( );
	m_blendStates.clear( );
	m_depthStencilStates.clear( );

	for ( auto& freeResource : m_freeResourceList )
	{
		freeResource = nullptr;
	}

	for ( auto& lut : m_resourceLUT )
	{
		lut.clear( );
	}
}

void CD3D11ResourceManager::OnDeviceRestore( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	LoadShader( );
}

const CD3D11DepthStencil& CD3D11ResourceManager::GetDepthstencil( RE_HANDLE handle ) const
{
	assert( IsDepthStencilHandle( handle ) );
	return m_depthStencils[handle & RE_INDEX_MASK];
}

const CD3D11RenderTarget& CD3D11ResourceManager::GetRendertarget( RE_HANDLE handle ) const
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

CD3D11ResourceManager::~CD3D11ResourceManager( )
{
	CleanUpFreeResourceList( );
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

void CD3D11ResourceManager::CleanUpFreeResourceList( )
{
	using namespace RESOURCE_TYPE;

	ClearFreeList<CD3D11Buffer>( &m_freeResourceList[BUFFER] );
	ClearFreeList<CD3D11Texture>( &m_freeResourceList[TEXTURE] );
	ClearFreeList<CD3D11ShaderResource>( &m_freeResourceList[SHADER_RESOURCE] );
	ClearFreeList<CD3D11RandomAccessResource>( &m_freeResourceList[RANDOM_ACCESS] );
	ClearFreeList<CD3D11RenderTarget>( &m_freeResourceList[RENDER_TARGET] );
	ClearFreeList<CD3D11DepthStencil>( &m_freeResourceList[DEPTH_STENCIL] );

	ClearFreeList<CD3D11VertexShader>( &m_freeResourceList[VERTEX_SHADER] );
	ClearFreeList<CD3D11GeometryShader>( &m_freeResourceList[GEOMETRY_SHADER] );
	ClearFreeList<CD3D11PixelShader>( &m_freeResourceList[PIXEL_SHADER] );
	ClearFreeList<CD3D11ComputeShader>( &m_freeResourceList[COMPUTE_SHADER] );

	ClearFreeList<CD3D11SamplerState>( &m_freeResourceList[SAMPLER_STATE] );
	ClearFreeList<CD3D11RasterizerState>( &m_freeResourceList[RASTERIZER_STATE] );
	ClearFreeList<CD3D11BlendState>( &m_freeResourceList[BLEND_STATE] );
	ClearFreeList<CD3D11DepthStencilState>( &m_freeResourceList[DEPTH_STENCIL_STATE] );
}
