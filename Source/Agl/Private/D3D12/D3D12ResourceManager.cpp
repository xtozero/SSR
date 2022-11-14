#include "D3D12ResourceManager.h"

#include "Config/DefaultAglConfig.h"

#include "D3D11FlagConvertor.h"
#include "D3D12BaseTexture.h"
#include "D3D12BlendState.h"
#include "D3D12Buffer.h"
#include "D3D12DepthStencilState.h"
#include "D3D12PipelineState.h"
#include "D3D12RasterizerState.h"
#include "D3D12SamplerState.h"
#include "D3D12Shaders.h"
#include "D3D12VertexLayout.h"
#include "D3D12Viewport.h"

namespace agl
{
	void D3D12ResourceManager::Shutdown()
	{
	}

	Texture* D3D12ResourceManager::CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
	{
		Texture* newTexture = nullptr;
		if ( IsTexture1D( trait ) )
		{
			newTexture = new D3D12BaseTexture1D( trait, initData );
		}
		else if ( IsTexture2D( trait ) )
		{
			newTexture = new D3D12BaseTexture2D( trait, initData );
		}
		else if ( IsTexture3D( trait ) )
		{
			newTexture = new D3D12BaseTexture3D( trait, initData );
		}
		else
		{
			return nullptr;
		}

		return newTexture;
	}

	Buffer* D3D12ResourceManager::CreateBuffer( const BUFFER_TRAIT& trait, const void* initData )
	{
		Buffer* newBuffer = new D3D12Buffer( trait, initData );

		return newBuffer;
	}

	VertexLayout* D3D12ResourceManager::CreateVertexLayout( [[maybe_unused]] const VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, uint32 size )
	{
		return new D3D12VertexLayout( trait, size );
	}

	ComputeShader* D3D12ResourceManager::CreateComputeShader( const void* byteCode, size_t byteCodeSize )
	{
		return new D3D12ComputeShader( byteCode, byteCodeSize );
	}

	VertexShader* D3D12ResourceManager::CreateVertexShader( const void* byteCode, size_t byteCodeSize )
	{
		return new D3D12VertexShader( byteCode, byteCodeSize );
	}

	GeometryShader* D3D12ResourceManager::CreateGeometryShader( const void* byteCode, size_t byteCodeSize )
	{
		return new D3D12GeometryShader( byteCode, byteCodeSize );
	}

	PixelShader* D3D12ResourceManager::CreatePixelShader( const void* byteCode, size_t byteCodeSize )
	{
		return new D3D12PixelShader( byteCode, byteCodeSize );
	}

	BlendState* D3D12ResourceManager::CreateBlendState( const BLEND_STATE_TRAIT& trait )
	{
		return new D3D12BlendState( trait );
	}

	DepthStencilState* D3D12ResourceManager::CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait )
	{
		return new D3D12DepthStencilState( trait );
	}

	RasterizerState* D3D12ResourceManager::CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait )
	{
		return new D3D12RasterizerState( trait );
	}

	SamplerState* D3D12ResourceManager::CreateSamplerState( const SAMPLER_STATE_TRAIT& trait )
	{
		return new D3D12SamplerState( trait );
	}

	PipelineState* D3D12ResourceManager::CreatePipelineState( const GraphicsPipelineStateInitializer& initializer )
	{
		return new D3D12GraphicsPipelineState( initializer );
	}

	PipelineState* D3D12ResourceManager::CreatePipelineState( const ComputePipelineStateInitializer& initializer )
	{
		return new D3D12ComputePipelineState( initializer );
	}

	Viewport* D3D12ResourceManager::CreateViewport( uint32 width, uint32 height, void* hWnd, ResourceFormat format )
	{
		uint32 bufferCount = DefaultAgl::GetBufferCount();
		return new D3D12Viewport( width, height, bufferCount, hWnd, ConvertFormatToDxgiFormat( format ) );
	}

	D3D12ResourceManager::~D3D12ResourceManager()
	{
		Shutdown();
	}

	Owner<IResourceManager*> CreateD3D12ResourceManager()
	{
		return new D3D12ResourceManager();
	}
}
