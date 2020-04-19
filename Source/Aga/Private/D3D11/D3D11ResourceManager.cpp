#include "stdafx.h"
#include "D3D11/D3D11ResourceManager.h"

#include "common.h"
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

namespace
{
	class CShaderByteCode
	{
	public:
		inline explicit CShaderByteCode( const size_t size ) :
			m_pBuffer( nullptr ),
			m_size( size )
		{
			if ( m_size > 0 )
			{
				m_pBuffer = new BYTE[m_size];
			}
		}

		CShaderByteCode( CShaderByteCode&& byteCode ) :
			m_pBuffer( byteCode.m_pBuffer ),
			m_size( byteCode.m_size )
		{
			byteCode.m_pBuffer = nullptr;
			byteCode.m_size = 0;
		}

		CShaderByteCode& operator=( CShaderByteCode&& byteCode )
		{
			m_pBuffer = byteCode.m_pBuffer;
			m_size = byteCode.m_size;
			byteCode.m_pBuffer = nullptr;
			byteCode.m_size = 0;
			return *this;
		}

		~CShaderByteCode( )
		{
			delete[] m_pBuffer;
			m_pBuffer = nullptr;
			m_size = 0;
		}

		CShaderByteCode( const CShaderByteCode& byteCode ) = delete;
		CShaderByteCode& operator=( const CShaderByteCode& byteCode ) = delete;

		BYTE* GetBufferPointer( ) const { return m_pBuffer; }
		size_t GetBufferSize( ) const { return m_size; }
	private:
		BYTE* m_pBuffer;
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
		static String baseByteCodePath( _T( "./Shaders/" ) );
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

	template <typename T, GraphicsResourceType Resource>
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
			RE_HANDLE handle( Resource, idx );

			lut.emplace( fileName, handle );
			return handle;
		}

		return RE_HANDLE::InValidHandle( );
	}
}

bool CD3D11ResourceManager::Bootup( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
{
	OnDeviceRestore( pDevice, pDeviceContext );
	return true;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture1D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	if ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
	{
		trait.m_width = m_frameBufferSize.first;
		trait.m_height = m_frameBufferSize.second;
	}

	RE_HANDLE handle( GraphicsResourceType::TEXTURE1D );

	auto found = std::find( m_texture1Ds.begin( ), m_texture1Ds.end( ), nullptr );
	if ( found != m_texture1Ds.end( ) )
	{
		handle.m_index = std::distance( m_texture1Ds.begin( ), found );
		*found = RefHandle( new CD3D11Texture1D( trait, initData ) );
	}
	else
	{
		handle.m_index = m_texture1Ds.size( );
		m_texture1Ds.emplace_back( new CD3D11Texture1D( trait, initData ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture2D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	if ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
	{
		trait.m_width = m_frameBufferSize.first;
		trait.m_height = m_frameBufferSize.second;
	}

	RE_HANDLE handle( GraphicsResourceType::TEXTURE2D );

	auto found = std::find( m_texture2Ds.begin( ), m_texture2Ds.end( ), nullptr );
	if ( found != m_texture2Ds.end( ) )
	{
		handle.m_index = std::distance( m_texture2Ds.begin( ), found );
		*found = RefHandle( new CD3D11Texture2D( trait, initData ) );
	}
	else
	{
		handle.m_index = m_texture2Ds.size( );
		m_texture2Ds.emplace_back( new CD3D11Texture2D( trait, initData ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture3D( TEXTURE_TRAIT & trait, const RESOURCE_INIT_DATA * initData )
{
	if ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
	{
		trait.m_width = m_frameBufferSize.first;
		trait.m_height = m_frameBufferSize.second;
	}

	RE_HANDLE handle( GraphicsResourceType::TEXTURE3D );

	auto found = std::find( m_texture3Ds.begin( ), m_texture3Ds.end( ), nullptr );
	if ( found != m_texture3Ds.end( ) )
	{
		handle.m_index = std::distance( m_texture3Ds.begin( ), found );
		*found = RefHandle( new CD3D11Texture3D( trait, initData ) );
	}
	else
	{
		handle.m_index = m_texture3Ds.size( );
		m_texture3Ds.emplace_back( new CD3D11Texture3D( trait, initData ) );
	}

	return handle;
}

void CD3D11ResourceManager::AppSizeChanged( UINT nWndWidth, UINT nWndHeight )
{
	m_frameBufferSize.first = nWndWidth;
	m_frameBufferSize.second = nWndHeight;

	RE_HANDLE handle = RE_HANDLE::InvalidHandle( );

	for ( auto depthStencil : m_depthStencils )
	{
		if ( depthStencil->IsAppSizeDependency() )
		{
			depthStencil->Free( );
		}
	}

	for ( auto renderTarget : m_renderTargets )
	{
		if ( renderTarget->IsAppSizeDependency( ) )
		{
			renderTarget->Free( );
		}
	}

	for ( auto shaderResource : m_shaderResources )
	{
		if ( shaderResource->IsAppSizeDependency( ) )
		{
			shaderResource->Free( );
		}
	}

	for ( auto randomAccessResource : m_randomAccessResources )
	{
		if ( randomAccessResource->IsAppSizeDependency( ) )
		{
			randomAccessResource->Free( );
		}
	}

	// 리사이즈 필요
	for ( auto texture : m_texture2Ds )
	{
		if ( texture->IsAppSizeDependency( ) )
		{
			texture->Free( );
		}
	}
}

RE_HANDLE CD3D11ResourceManager::CreateBuffer( const BUFFER_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	RE_HANDLE handle( GraphicsResourceType::BUFFER );

	auto found = std::find( m_buffers.begin( ), m_buffers.end( ), nullptr );
	if ( found != m_buffers.end( ) )
	{
		handle.m_index = std::distance( m_buffers.begin( ), found );
		*found = RefHandle( new CD3D11Buffer( trait, initData ) );
	}
	else
	{
		handle.m_index = m_buffers.size( );
		m_buffers.emplace_back( new CD3D11Buffer( trait, initData ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateVertexLayout( RE_HANDLE vsHandle, const VERTEX_LAYOUT* layoutOrNull, int layoutSize )
{
	RE_HANDLE handle( GraphicsResourceType::VERTEX_LAYOUT );

	RefHandle<CD3D11VertexShader>& vs = GetVertexShader( vsHandle );

	auto found = std::find( m_vertexLayouts.begin( ), m_vertexLayouts.end( ), nullptr );
	if ( found != m_vertexLayouts.end( ) )
	{
		handle.m_index = std::distance( m_vertexLayouts.begin( ), found );
		*found = RefHandle( new CD3D11VertexLayout( vs, layoutOrNull, layoutSize ) );
	}
	else
	{
		handle.m_index = m_vertexLayouts.size( );
		m_vertexLayouts.emplace_back( new CD3D11VertexLayout( vs, layoutOrNull, layoutSize ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateVertexShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	RE_HANDLE handle( GraphicsResourceType::VERTEX_SHADER );

	auto found = std::find( m_vertexShaders.begin( ), m_vertexShaders.end( ), nullptr );
	if ( found != m_vertexShaders.end( ) )
	{
		handle.m_index = std::distance( m_vertexShaders.begin( ), found );
		*found = RefHandle( new CD3D11VertexShader( byteCodePtr, byteCodeSize ) );
	}
	else
	{
		handle.m_index = m_vertexShaders.size( );
		m_vertexShaders.emplace_back( new CD3D11VertexShader( byteCodePtr, byteCodeSize ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateGeometryShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	RE_HANDLE handle( GraphicsResourceType::GEOMETRY_SHADER );

	auto found = std::find( m_geometryShaders.begin( ), m_geometryShaders.end( ), nullptr );
	if ( found != m_geometryShaders.end( ) )
	{
		handle.m_index = std::distance( m_geometryShaders.begin( ), found );
		*found = RefHandle( new CD3D11GeometryShader( byteCodePtr, byteCodeSize ) );
	}
	else
	{
		handle.m_index = m_geometryShaders.size( );
		m_geometryShaders.emplace_back( new CD3D11GeometryShader( byteCodePtr, byteCodeSize ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreatePixelShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	RE_HANDLE handle( GraphicsResourceType::PIXEL_SHADER );

	auto found = std::find( m_pixelShaders.begin( ), m_pixelShaders.end( ), nullptr );
	if ( found != m_pixelShaders.end( ) )
	{
		handle.m_index = std::distance( m_pixelShaders.begin( ), found );
		*found = RefHandle( new CD3D11PixelShader( byteCodePtr, byteCodeSize ) );
	}
	else
	{
		handle.m_index = m_pixelShaders.size( );
		m_pixelShaders.emplace_back( new CD3D11PixelShader( byteCodePtr, byteCodeSize ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateComputeShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	RE_HANDLE handle( GraphicsResourceType::COMPUTE_SHADER );

	auto found = std::find( m_computeShaders.begin( ), m_computeShaders.end( ), nullptr );
	if ( found != m_computeShaders.end( ) )
	{
		handle.m_index = std::distance( m_computeShaders.begin( ), found );
		*found = RefHandle( new CD3D11ComputeShader( byteCodePtr, byteCodeSize ) );
	}
	else
	{
		handle.m_index = m_computeShaders.size( );
		m_computeShaders.emplace_back( new CD3D11ComputeShader( byteCodePtr, byteCodeSize ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateRenderTarget( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle != RE_HANDLE::InvalidHandle( ) );
	assert( IsTexture2DHandle( texHandle ) );

	RE_HANDLE handle( GraphicsResourceType::RENDER_TARGET );

	RefHandle<CD3D11Texture2D>& texture = GetTexture2D( texHandle );

	auto found = std::find( m_renderTargets.begin( ), m_renderTargets.end( ), nullptr );
	if ( found != m_renderTargets.end( ) )
	{
		handle.m_index = std::distance( m_renderTargets.begin( ), found );
		*found = RefHandle( new CD3D11RenderTarget( texture, trait ) );
	}
	else
	{
		handle.m_index = m_renderTargets.size( );
		m_renderTargets.emplace_back( new CD3D11RenderTarget( texture, trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateDepthStencil( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle != RE_HANDLE::InvalidHandle( ) );
	assert( IsTexture2DHandle( texHandle ) );

	RE_HANDLE handle( GraphicsResourceType::DEPTH_STENCIL );

	RefHandle<CD3D11Texture2D>& texture = GetTexture2D( texHandle );

	auto found = std::find( m_depthStencils.begin( ), m_depthStencils.end( ), nullptr );
	if ( found != m_depthStencils.end( ) )
	{
		handle.m_index = std::distance( m_depthStencils.begin( ), found );
		*found = RefHandle( new CD3D11DepthStencil( texture, trait ) );
	}
	else
	{
		handle.m_index = m_depthStencils.size( );
		m_depthStencils.emplace_back( new CD3D11DepthStencil( texture, trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture1DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle != RE_HANDLE::InvalidHandle( ) );
	assert( IsTexture1DHandle( texHandle ) );

	RE_HANDLE handle( GraphicsResourceType::SHADER_RESOURCE );

	RefHandle<CD3D11Texture1D>& texture = GetTexture1D( texHandle );

	auto found = std::find( m_shaderResources.begin( ), m_shaderResources.end( ), nullptr );
	if ( found != m_shaderResources.end( ) )
	{
		handle.m_index = std::distance( m_shaderResources.begin( ), found );
		*found = RefHandle( new CD3D11ShaderResource( texture, trait ) );
	}
	else
	{
		handle.m_index = m_shaderResources.size( );
		m_shaderResources.emplace_back( new CD3D11ShaderResource( texture, trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture2DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle != RE_HANDLE::InvalidHandle( ) );
	assert( IsTexture1DHandle( texHandle ) );

	RE_HANDLE handle( GraphicsResourceType::SHADER_RESOURCE );

	RefHandle<CD3D11Texture2D>& texture = GetTexture2D( texHandle );

	auto found = std::find( m_shaderResources.begin( ), m_shaderResources.end( ), nullptr );
	if ( found != m_shaderResources.end( ) )
	{
		handle.m_index = std::distance( m_shaderResources.begin( ), found );
		*found = RefHandle( new CD3D11ShaderResource( texture, trait ) );
	}
	else
	{
		handle.m_index = m_shaderResources.size( );
		m_shaderResources.emplace_back( new CD3D11ShaderResource( texture, trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture3DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle != RE_HANDLE::InvalidHandle( ) );
	assert( IsTexture1DHandle( texHandle ) );

	RE_HANDLE handle( GraphicsResourceType::SHADER_RESOURCE );

	RefHandle<CD3D11Texture3D>& texture = GetTexture3D( texHandle );

	auto found = std::find( m_shaderResources.begin( ), m_shaderResources.end( ), nullptr );
	if ( found != m_shaderResources.end( ) )
	{
		handle.m_index = std::distance( m_shaderResources.begin( ), found );
		*found = RefHandle( new CD3D11ShaderResource( texture, trait ) );
	}
	else
	{
		handle.m_index = m_shaderResources.size( );
		m_shaderResources.emplace_back( new CD3D11ShaderResource( texture, trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateBufferShaderResource( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait )
{
	assert( bufHandle != RE_HANDLE::InvalidHandle( ) );
	assert( IsBufferHandle( bufHandle ) );

	RE_HANDLE handle( GraphicsResourceType::SHADER_RESOURCE );

	RefHandle<CD3D11Buffer>& buffer = GetBuffer( bufHandle );

	auto found = std::find( m_shaderResources.begin( ), m_shaderResources.end( ), nullptr );
	if ( found != m_shaderResources.end( ) )
	{
		handle.m_index = std::distance( m_shaderResources.begin( ), found );
		*found = RefHandle( new CD3D11ShaderResource( buffer, trait ) );
	}
	else
	{
		handle.m_index = m_shaderResources.size( );
		m_shaderResources.emplace_back( new CD3D11ShaderResource( buffer, trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture1DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle != RE_HANDLE::InvalidHandle( ) );
	assert( IsTexture1DHandle( texHandle ) );

	RE_HANDLE handle( GraphicsResourceType::RANDOM_ACCESS );

	RefHandle<CD3D11Texture1D>& texture = GetTexture1D( texHandle );

	auto found = std::find( m_randomAccessResources.begin( ), m_randomAccessResources.end( ), nullptr );
	if ( found != m_randomAccessResources.end( ) )
	{
		handle.m_index = std::distance( m_randomAccessResources.begin( ), found );
		*found = RefHandle( new CD3D11RandomAccessResource( texture, trait ) );
	}
	else
	{
		handle.m_index = m_randomAccessResources.size( );
		m_randomAccessResources.emplace_back( new CD3D11RandomAccessResource( texture, trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateTexture2DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle != RE_HANDLE::InvalidHandle( ) );
	assert( IsTexture2DHandle( texHandle ) );

	RE_HANDLE handle( GraphicsResourceType::RANDOM_ACCESS );

	RefHandle<CD3D11Texture2D>& texture = GetTexture2D( texHandle );

	auto found = std::find( m_randomAccessResources.begin( ), m_randomAccessResources.end( ), nullptr );
	if ( found != m_randomAccessResources.end( ) )
	{
		handle.m_index = std::distance( m_randomAccessResources.begin( ), found );
		*found = RefHandle( new CD3D11RandomAccessResource( texture, trait ) );
	}
	else
	{
		handle.m_index = m_randomAccessResources.size( );
		m_randomAccessResources.emplace_back( new CD3D11RandomAccessResource( texture, trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateBufferRandomAccess( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait )
{
	assert( bufHandle != RE_HANDLE::InvalidHandle( ) );
	assert( IsBufferHandle( bufHandle ) );

	RE_HANDLE handle( GraphicsResourceType::RANDOM_ACCESS );

	RefHandle<CD3D11Buffer>& buffer = GetBuffer( bufHandle );

	auto found = std::find( m_randomAccessResources.begin( ), m_randomAccessResources.end( ), nullptr );
	if ( found != m_randomAccessResources.end( ) )
	{
		handle.m_index = std::distance( m_randomAccessResources.begin( ), found );
		*found = RefHandle( new CD3D11RandomAccessResource( buffer, trait ) );
	}
	else
	{
		handle.m_index = m_randomAccessResources.size( );
		m_randomAccessResources.emplace_back( new CD3D11RandomAccessResource( buffer, trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateSamplerState( const SAMPLER_STATE_TRAIT& trait )
{
	RE_HANDLE handle( GraphicsResourceType::SAMPLER_STATE );

	auto found = std::find( m_samplerStates.begin( ), m_samplerStates.end( ), nullptr );
	if ( found != m_samplerStates.end( ) )
	{
		handle.m_index = std::distance( m_samplerStates.begin( ), found );
		*found = RefHandle( new CD3D11SamplerState( trait ) );
	}
	else
	{
		handle.m_index = m_samplerStates.size( );
		m_samplerStates.emplace_back( new CD3D11SamplerState( trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait )
{
	RE_HANDLE handle( GraphicsResourceType::RASTERIZER_STATE );

	auto found = std::find( m_rasterizerStates.begin( ), m_rasterizerStates.end( ), nullptr );
	if ( found != m_rasterizerStates.end( ) )
	{
		handle.m_index = std::distance( m_rasterizerStates.begin( ), found );
		*found = RefHandle( new CD3D11RasterizerState( trait ) );
	}
	else
	{
		handle.m_index = m_rasterizerStates.size( );
		m_rasterizerStates.emplace_back( new CD3D11RasterizerState( trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateBlendState( const BLEND_STATE_TRAIT& trait )
{
	RE_HANDLE handle( GraphicsResourceType::BLEND_STATE );

	auto found = std::find( m_blendStates.begin( ), m_blendStates.end( ), nullptr );
	if ( found != m_blendStates.end( ) )
	{
		handle.m_index = std::distance( m_blendStates.begin( ), found );
		*found = RefHandle( new CD3D11BlendState( trait ) );
	}
	else
	{
		handle.m_index = m_blendStates.size( );
		m_blendStates.emplace_back( new CD3D11BlendState( trait ) );
	}

	return handle;
}

RE_HANDLE CD3D11ResourceManager::CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait )
{
	RE_HANDLE handle( GraphicsResourceType::DEPTH_STENCIL_STATE );

	auto found = std::find( m_depthStencilStates.begin( ), m_depthStencilStates.end( ), nullptr );
	if ( found != m_depthStencilStates.end( ) )
	{
		handle.m_index = std::distance( m_depthStencilStates.begin( ), found );
		*found = RefHandle( new CD3D11DepthStencilState( trait ) );
	}
	else
	{
		handle.m_index = m_depthStencilStates.size( );
		m_depthStencilStates.emplace_back( new CD3D11DepthStencilState( trait ) );
	}

	return handle;
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

void CD3D11ResourceManager::FreeResource( RE_HANDLE handle )
{
	if ( handle == RE_HANDLE::InvalidHandle( ) )
	{
		return;
	}

	IDeviceDependant* graphicsResource = GetGraphicsResource( handle );
	if ( graphicsResource )
	{
		graphicsResource->Free( );
	}
}

void CD3D11ResourceManager::OnDeviceLost( )
{
	for ( auto depthStencil : m_depthStencils )
	{
		depthStencil->Free( );
	}

	for ( auto renderTarget : m_renderTargets )
	{
		renderTarget->Free( );
	}

	for ( auto shaderResource : m_shaderResources )
	{
		shaderResource->Free( );
	}

	for ( auto randomAccessResource : m_randomAccessResources )
	{
		randomAccessResource->Free( );
	}

	for ( auto texture : m_texture1Ds )
	{
		texture->Free( );
	}

	for ( auto texture : m_texture2Ds )
	{
		texture->Free( );
	}

	for ( auto texture : m_texture3Ds )
	{
		texture->Free( );
	}

	for ( auto buffer : m_buffers )
	{
		buffer->Free( );
	}

	m_vertexShaders.clear( );
	m_geometryShaders.clear( );
	m_pixelShaders.clear( );
	m_computeShaders.clear( );

	for ( auto samplerState : m_samplerStates )
	{
		samplerState->Free( );
	}

	for ( auto rasterizerState : m_rasterizerStates )
	{
		rasterizerState->Free( );
	}

	for ( auto blendState : m_blendStates )
	{
		blendState->Free( );
	}

	for ( auto depthStencilState : m_depthStencilStates )
	{
		depthStencilState->Free( );
	}
}

void CD3D11ResourceManager::OnDeviceRestore( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
}

RefHandle<CD3D11DepthStencil>& CD3D11ResourceManager::GetDepthstencil( RE_HANDLE handle )
{
	assert( IsDepthStencilHandle( handle ) );
	return m_depthStencils[handle.m_index];
}

RefHandle<CD3D11RenderTarget>& CD3D11ResourceManager::GetRendertarget( RE_HANDLE handle )
{
	assert( IsRenderTargetHandle( handle ) );
	return m_renderTargets[handle.m_index];
}

RefHandle<CD3D11ShaderResource>& CD3D11ResourceManager::GetShaderResource( RE_HANDLE handle )
{
	assert( IsShaderResourceHandle( handle ) );
	return m_shaderResources[handle.m_index];
}

RefHandle<CD3D11RandomAccessResource>& CD3D11ResourceManager::GetRandomAccess( RE_HANDLE handle )
{
	assert( IsRandomAccessHandle( handle ) );
	return m_randomAccessResources[handle.m_index];
}

RefHandle<CD3D11Texture1D>& CD3D11ResourceManager::GetTexture1D( RE_HANDLE handle )
{
	assert( IsTexture1DHandle( handle ) );
	return m_texture1Ds[handle.m_index];
}

RefHandle<CD3D11Texture2D>& CD3D11ResourceManager::GetTexture2D( RE_HANDLE handle )
{
	assert( IsTexture2DHandle( handle ) );
	return m_texture2Ds[handle.m_index];
}

RefHandle<CD3D11Texture3D>& CD3D11ResourceManager::GetTexture3D( RE_HANDLE handle )
{
	assert( IsTexture3DHandle( handle ) );
	return m_texture3Ds[handle.m_index];
}

RefHandle<CD3D11Buffer>& CD3D11ResourceManager::GetBuffer( RE_HANDLE handle )
{
	assert( IsBufferHandle( handle ) );
	return m_buffers[handle.m_index];
}

RefHandle<CD3D11VertexLayout>& CD3D11ResourceManager::GetVertexLayout( RE_HANDLE handle )
{
	assert( IsVertexLayout( handle ) );
	return m_vertexLayouts[handle.m_index];
}

RefHandle<CD3D11VertexShader>& CD3D11ResourceManager::GetVertexShader( RE_HANDLE handle )
{
	assert( IsVertexShaderHandle( handle ) );
	return m_vertexShaders[handle.m_index];
}

RefHandle<CD3D11GeometryShader>& CD3D11ResourceManager::GetGeometryShader( RE_HANDLE handle )
{
	assert( IsGeometryShaderHandle( handle ) );
	return m_geometryShaders[handle.m_index];
}

RefHandle<CD3D11PixelShader>& CD3D11ResourceManager::GetPixelShader( RE_HANDLE handle )
{
	assert( IsPixelShaderHandle( handle ) );
	return m_pixelShaders[handle.m_index];
}

RefHandle<CD3D11ComputeShader>& CD3D11ResourceManager::GetComputeShader( RE_HANDLE handle )
{
	assert( IsComputeShaderHandle( handle ) );
	return m_computeShaders[handle.m_index];
}

RefHandle<CD3D11SamplerState>& CD3D11ResourceManager::GetSamplerState( RE_HANDLE handle )
{
	assert( IsSamplerStateHandle( handle ) );
	return m_samplerStates[handle.m_index];
}

RefHandle<CD3D11RasterizerState>& CD3D11ResourceManager::GetRasterizerState( RE_HANDLE handle )
{
	assert( IsRasterizerStateHandle( handle ) );
	return m_rasterizerStates[handle.m_index];
}

RefHandle<CD3D11BlendState>& CD3D11ResourceManager::GetBlendState( RE_HANDLE handle )
{
	assert( IsBlendStateHandle( handle ) );
	return m_blendStates[handle.m_index];
}

RefHandle<CD3D11DepthStencilState>& CD3D11ResourceManager::GetDepthStencilState( RE_HANDLE handle )
{
	assert( IsDepthStencilStateHandle( handle ) );
	return m_depthStencilStates[handle.m_index];
}

ID3D11Resource* CD3D11ResourceManager::GetD3D11ResourceGeneric( RE_HANDLE handle )
{
	if ( IsTexture1DHandle( handle ) )
	{
		RefHandle<CD3D11Texture1D>& texture = GetTexture1D( handle );
		return texture->Get( );
	}
	else if ( IsTexture2DHandle( handle ) )
	{
		RefHandle<CD3D11Texture2D>& texture = GetTexture2D( handle );
		return texture->Get( );
	}
	else if ( IsTexture3DHandle( handle ) )
	{
		RefHandle<CD3D11Texture3D>& texture = GetTexture3D( handle );
		return texture->Get( );
	}
	else if ( IsBufferHandle( handle ) )
	{
		RefHandle<CD3D11Buffer>& buffer = GetBuffer( handle );
		return buffer->Get( );
	}

	return nullptr;
}

IDeviceDependant* CD3D11ResourceManager::GetGraphicsResource( RE_HANDLE handle ) const
{
	GraphicsResourceType resourceType = handle.m_type;
	std::size_t resourceIdx = handle.m_index;

	switch ( resourceType )
	{
	case GraphicsResourceType::BUFFER:
		{
			assert( m_buffers[resourceIdx].Get( ) );
			return m_buffers[resourceIdx].Get( );
		}
	case GraphicsResourceType::DEPTH_STENCIL:
		{
			assert( m_depthStencils[resourceIdx].Get( ) );
			return m_depthStencils[resourceIdx].Get( );
		}
	case GraphicsResourceType::RENDER_TARGET:
		{
			assert( m_renderTargets[resourceIdx].Get( ) );
			return m_renderTargets[resourceIdx].Get( );
		}
		break;
	case GraphicsResourceType::SHADER_RESOURCE:
		{
			assert( m_shaderResources[resourceIdx].Get( ) );
			return m_shaderResources[resourceIdx].Get( );
		}
		break;
	case GraphicsResourceType::RANDOM_ACCESS:
		{
			assert( m_randomAccessResources[resourceIdx].Get( ) );
			return m_randomAccessResources[resourceIdx].Get( );
		}
		break;
	case GraphicsResourceType::TEXTURE1D:
		{
			assert( m_texture1Ds[resourceIdx].Get( ) );
			return m_texture1Ds[resourceIdx].Get( );
		}
		break;
	case GraphicsResourceType::TEXTURE2D:
		{
			assert( m_texture2Ds[resourceIdx].Get( ) );
			return m_texture2Ds[resourceIdx].Get( );
		}
	break;
	case GraphicsResourceType::TEXTURE3D:
		{
			assert( m_texture3Ds[resourceIdx].Get( ) );
			return m_texture3Ds[resourceIdx].Get( );
		}
	break;
	case GraphicsResourceType::VERTEX_SHADER:
		{
			assert( m_vertexShaders[resourceIdx].Get( ) );
			return m_vertexShaders[resourceIdx].Get( );
		}
		break;
	case GraphicsResourceType::GEOMETRY_SHADER:
		{
			assert( m_geometryShaders[resourceIdx].Get( ) );
			return m_geometryShaders[resourceIdx].Get( );
		}
		break;
	case GraphicsResourceType::PIXEL_SHADER:
		{
			assert( m_pixelShaders[resourceIdx].Get( ) );
			return m_pixelShaders[resourceIdx].Get( );
		}
		break;
	case GraphicsResourceType::COMPUTE_SHADER:
		{
			assert( m_computeShaders[resourceIdx].Get( ) );
			return m_computeShaders[resourceIdx].Get( );
		}
		break;
	case GraphicsResourceType::SAMPLER_STATE:
		{
			assert( m_samplerStates[resourceIdx].Get( ) );
			return m_samplerStates[resourceIdx].Get( );
		}
		break;
	case GraphicsResourceType::RASTERIZER_STATE:
		{
			assert( m_rasterizerStates[resourceIdx].Get( ) );
			return m_rasterizerStates[resourceIdx].Get( );
		}
		break;
	case GraphicsResourceType::BLEND_STATE:
		{
			assert( m_blendStates[resourceIdx].Get( ) );
			return m_blendStates[resourceIdx].Get( );
		}
		break;
	case GraphicsResourceType::DEPTH_STENCIL_STATE:
		{
			assert( m_depthStencilStates[resourceIdx].Get( ) );
			return m_depthStencilStates[resourceIdx].Get( );
		}
		break;
	default:
		assert( false && "invalid resource handle" );
		break;
	}

	return nullptr;
}
