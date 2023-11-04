#include "D3D12ResourceManager.h"

#include "Config/DefaultAglConfig.h"

#include "D3D12Api.h"
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

#include "DxgiFlagConvertor.h"
#include "DxgiSwapchain.h"

namespace agl
{
	void D3D12ResourceManager::Shutdown()
	{
	}

	Texture* D3D12ResourceManager::CreateTexture( const TextureTrait& trait, const char* debugName, const ResourceInitData* initData )
	{
		Texture* newTexture = nullptr;
		if ( IsTexture1D( trait ) )
		{
			newTexture = new D3D12BaseTexture1D( trait, debugName, initData );
		}
		else if ( IsTexture2D( trait ) )
		{
			newTexture = new D3D12BaseTexture2D( trait, debugName, initData );
		}
		else if ( IsTexture3D( trait ) )
		{
			newTexture = new D3D12BaseTexture3D( trait, debugName, initData );
		}
		else
		{
			return nullptr;
		}

		return newTexture;
	}

	Buffer* D3D12ResourceManager::CreateBuffer( const BufferTrait& trait, const char* debugName, const void* initData )
	{
		Buffer* newBuffer = nullptr;
		if ( HasAnyFlags( trait.m_bindType, ResourceBindType::ConstantBuffer ) )
		{
			newBuffer = new D3D12ConstantBuffer( trait, debugName, initData );
		}
		else if ( HasAnyFlags( trait.m_bindType, ResourceBindType::IndexBuffer ) )
		{
			newBuffer = new D3D12IndexBuffer( trait, debugName, initData );
		}
		else if ( HasAnyFlags( trait.m_bindType, ResourceBindType::VertexBuffer ) )
		{
			newBuffer = new D3D12VertexBuffer( trait, debugName, initData );
		} 
		else
		{
			newBuffer = new D3D12Buffer( trait, debugName, initData );
		}

		return newBuffer;
	}

	VertexLayout* D3D12ResourceManager::CreateVertexLayout( [[maybe_unused]] const VertexShader* vs, const VertexLayoutTrait* trait, uint32 size )
	{
		return new D3D12VertexLayout( trait, size );
	}

	ComputeShader* D3D12ResourceManager::CreateComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return new D3D12ComputeShader( byteCode, byteCodeSize, paramInfo );
	}

	VertexShader* D3D12ResourceManager::CreateVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return new D3D12VertexShader( byteCode, byteCodeSize, paramInfo );
	}

	GeometryShader* D3D12ResourceManager::CreateGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return new D3D12GeometryShader( byteCode, byteCodeSize, paramInfo );
	}

	PixelShader* D3D12ResourceManager::CreatePixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return new D3D12PixelShader( byteCode, byteCodeSize, paramInfo );
	}

	BlendState* D3D12ResourceManager::CreateBlendState( const BlendStateTrait& trait )
	{
		return new D3D12BlendState( trait );
	}

	DepthStencilState* D3D12ResourceManager::CreateDepthStencilState( const DepthStencilStateTrait& trait )
	{
		return new D3D12DepthStencilState( trait );
	}

	RasterizerState* D3D12ResourceManager::CreateRasterizerState( const RasterizerStateTrait& trait )
	{
		return new D3D12RasterizerState( trait );
	}

	SamplerState* D3D12ResourceManager::CreateSamplerState( const SamplerStateTrait& trait )
	{
		return new D3D12SamplerState( trait );
	}

	GraphicsPipelineState* D3D12ResourceManager::CreatePipelineState( const GraphicsPipelineStateInitializer& initializer )
	{
		auto cached = m_graphicsPipelineStateCache.find( initializer );
		if ( cached != m_graphicsPipelineStateCache.end() )
		{
			return cached->second;
		}

		auto pipelineState = new D3D12GraphicsPipelineState( initializer );
		m_graphicsPipelineStateCache.emplace( initializer, pipelineState );

		EnqueueRenderTask(
			[state = pipelineState]()
			{
				state->Init();
			} );

		return pipelineState;
	}

	ComputePipelineState* D3D12ResourceManager::CreatePipelineState( const ComputePipelineStateInitializer& initializer )
	{
		auto cached = m_computePipelineStateCache.find( initializer );
		if ( cached != m_computePipelineStateCache.end() )
		{
			return cached->second;
		}

		auto pipelineState = new D3D12ComputePipelineState( initializer );
		m_computePipelineStateCache.emplace( initializer, pipelineState );

		EnqueueRenderTask(
			[state = pipelineState]()
			{
				state->Init();
			} );

		return pipelineState;
	}

	Canvas* D3D12ResourceManager::CreateCanvas( uint32 width, uint32 height, void* hWnd, ResourceFormat format )
	{
		return new DxgiSwapchain<AglType::D3D12>( D3D12DirectCommandQueue(), D3D12Factory(), width, height, DefaultAgl::GetBufferCount(), hWnd, ConvertFormatToDxgiFormat( format ) );
	}

	Viewport* D3D12ResourceManager::CreateViewport( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor )
	{
		return new D3D12Viewport( width, height, ConvertFormatToDxgiFormat( format ), bgColor );
	}

	Viewport* D3D12ResourceManager::CreateViewport( Canvas& canvas )
	{
		return new D3D12Viewport( *reinterpret_cast<DxgiSwapchain<AglType::D3D12>*>( &canvas ) );
	}

	ID3D12PipelineState* D3D12ResourceManager::FindOrCreate( GraphicsPipelineState* pipelineState, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc )
	{
		D3D12PipelineStateKey key( pipelineState, desc );

		{
			std::shared_lock<std::shared_mutex> lock( m_d3d12PipelineMutex );
			auto found = m_d3d12PipelineState.find( key );
			if ( found != std::end( m_d3d12PipelineState ) )
			{
				return found->second.Get();
			}
		}

		{
			std::unique_lock<std::shared_mutex> lock( m_d3d12PipelineMutex );
			auto found = m_d3d12PipelineState.find( key );
			if ( found != std::end( m_d3d12PipelineState ) )
			{
				return found->second.Get();
			}

			Microsoft::WRL::ComPtr<ID3D12PipelineState> newPipelineState;
			[[maybe_unused]] HRESULT hr = D3D12Device().CreateGraphicsPipelineState( &desc, IID_PPV_ARGS( newPipelineState.GetAddressOf() ) );
			assert( SUCCEEDED( hr ) );

			m_d3d12PipelineState.emplace( key, newPipelineState );

			return newPipelineState.Get();
		}
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
