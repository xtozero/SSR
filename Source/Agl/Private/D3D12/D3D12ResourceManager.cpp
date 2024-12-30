#include "D3D12ResourceManager.h"

#include "D3D12Api.h"
#include "D3D12BlendState.h"
#include "D3D12Buffer.h"
#include "D3D12DepthStencilState.h"
#include "D3D12FlagConvertor.h"
#include "D3D12PipelineState.h"
#include "D3D12Query.h"
#include "D3D12RasterizerState.h"
#include "D3D12SamplerState.h"
#include "D3D12Shaders.h"
#include "D3D12Texture.h"
#include "D3D12VertexLayout.h"
#include "D3D12Viewport.h"

#include "DefaultAglConfig.h"

#include "DxgiFlagConvertor.h"
#include "DxgiSwapchain.h"

#include <d3dx12.h>
#include <cassert>

namespace agl
{
	void D3D12ResourceManager::Shutdown()
	{
	}

	Texture* D3D12ResourceManager::CreateTexture( const TextureTrait& trait, const char* debugName, ResourceState initialState, const ResourceInitData* initData ) const
	{
		Texture* newTexture = nullptr;
		if ( IsTexture2D( trait ) )
		{
			newTexture = new D3D12Texture2D( trait, debugName, initialState, initData );
		}
		else if ( IsTexture3D( trait ) )
		{
			newTexture = new D3D12Texture3D( trait, debugName, initialState, initData );
		}
		else
		{
			return nullptr;
		}

		return newTexture;
	}

	Buffer* D3D12ResourceManager::CreateBuffer( const BufferTrait& trait, const char* debugName, ResourceState initialState, const void* initData ) const
	{
		Buffer* newBuffer = nullptr;
		if ( HasAnyFlags( trait.m_bindType, ResourceBindType::ConstantBuffer ) )
		{
			if ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Disposable ) )
			{
				newBuffer = new D3D12DisposableConstantBuffer( trait, debugName );
			}
			else
			{
				newBuffer = new D3D12ConstantBuffer( trait, debugName, initialState, initData );
			}
		}
		else if ( HasAnyFlags( trait.m_bindType, ResourceBindType::IndexBuffer ) )
		{
			newBuffer = new D3D12IndexBuffer( trait, debugName, initialState, initData );
		}
		else if ( HasAnyFlags( trait.m_bindType, ResourceBindType::VertexBuffer ) )
		{
			newBuffer = new D3D12VertexBuffer( trait, debugName, initialState, initData );
		} 
		else
		{
			newBuffer = new D3D12Buffer( trait, debugName, initialState, initData );
		}

		return newBuffer;
	}

	VertexLayout* D3D12ResourceManager::CreateVertexLayout( [[maybe_unused]] const VertexShader* vs, const VertexLayoutTrait* trait, uint32 size ) const
	{
		return new D3D12VertexLayout( trait, size );
	}

	ComputeShader* D3D12ResourceManager::CreateComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
	{
		return new D3D12ComputeShader( byteCode, byteCodeSize, paramInfo );
	}

	VertexShader* D3D12ResourceManager::CreateVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
	{
		return new D3D12VertexShader( byteCode, byteCodeSize, paramInfo );
	}

	GeometryShader* D3D12ResourceManager::CreateGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
	{
		return new D3D12GeometryShader( byteCode, byteCodeSize, paramInfo );
	}

	PixelShader* D3D12ResourceManager::CreatePixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
	{
		return new D3D12PixelShader( byteCode, byteCodeSize, paramInfo );
	}

	MeshShader* D3D12ResourceManager::CreateMeshShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
	{
		return new D3D12MeshShader( byteCode, byteCodeSize, paramInfo );
	}

	AmplificationShader* D3D12ResourceManager::CreateAmplificationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
	{
		return new D3D12AmplificationShader( byteCode, byteCodeSize, paramInfo );
	}

	BlendState* D3D12ResourceManager::CreateBlendState( const BlendStateTrait& trait ) const
	{
		return new D3D12BlendState( trait );
	}

	DepthStencilState* D3D12ResourceManager::CreateDepthStencilState( const DepthStencilStateTrait& trait ) const
	{
		return new D3D12DepthStencilState( trait );
	}

	RasterizerState* D3D12ResourceManager::CreateRasterizerState( const RasterizerStateTrait& trait ) const
	{
		return new D3D12RasterizerState( trait );
	}

	SamplerState* D3D12ResourceManager::CreateSamplerState( const SamplerStateTrait& trait ) const
	{
		return new D3D12SamplerState( trait );
	}

	GraphicsPipelineState* D3D12ResourceManager::CreatePipelineState( const GraphicsPipelineStateInitializer& initializer )
	{
		auto cached = m_graphicsPipelineStateCache.find( initializer );
		if ( cached != std::end( m_graphicsPipelineStateCache ) )
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
		if ( cached != std::end( m_computePipelineStateCache ) )
		{
			return cached->second;
		}

		size_t psoHash = initializer.GetHash();
		const BinaryChunk* cachedPSO = nullptr;
		if ( m_psoCache )
		{
			auto found = m_psoCache->find( psoHash );
			if ( found != std::end( *m_psoCache ) )
			{
				cachedPSO = &found->second;
			}
		}

		auto pipelineState = new D3D12ComputePipelineState( initializer, cachedPSO );
		m_computePipelineStateCache.emplace( initializer, pipelineState );

		EnqueueRenderTask(
			[this, psoHash, state = pipelineState]()
			{
				state->Init();
				UpdatePSOCache( psoHash, state->Resource() );
			} );

		return pipelineState;
	}

	Canvas* D3D12ResourceManager::CreateCanvas( uint32 width, uint32 height, void* hWnd, ResourceFormat format ) const
	{
		return new DxgiSwapchain<AglType::D3D12>( D3D12DirectCommandQueue(), D3D12Factory(), width, height, DefaultAgl::GetBufferCount(), hWnd, ConvertFormatToDxgiFormat( format ) );
	}

	Viewport* D3D12ResourceManager::CreateViewport( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor ) const
	{
		return new D3D12Viewport( width, height, ConvertFormatToDxgiFormat( format ), bgColor );
	}

	Viewport* D3D12ResourceManager::CreateViewport( Canvas& canvas ) const
	{
		return new D3D12Viewport( *reinterpret_cast<DxgiSwapchain<AglType::D3D12>*>( &canvas ) );
	}

	GpuTimer* D3D12ResourceManager::CreateGpuTimer() const
	{
		return new D3D12GpuTimer();
	}

	OcclusionQuery* D3D12ResourceManager::CreateOcclusionQuery() const
	{
		return new D3D12OcclusionTest();
	}

	PipelineStatistics* D3D12ResourceManager::CreatePipelineStatistics() const
	{
		return new D3D12PipelineStatistics();
	}

	void D3D12ResourceManager::SetPSOCache( std::map<uint64, BinaryChunk>& psoCache )
	{
		m_psoCache = &psoCache;
	}

	ID3D12PipelineState* D3D12ResourceManager::FindOrCreate( D3D12GraphicsPipelineState* pipelineState, const DXGI_FORMAT( &rtvFormats )[8], DXGI_FORMAT dsvFormat )
	{
		D3D12PipelineStateKey key( pipelineState, rtvFormats, dsvFormat );

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

			size_t psoHash = key.GetHash();
			D3D12_CACHED_PIPELINE_STATE cachedPSO = {};
			if ( m_psoCache )
			{
				auto cashedPSO = m_psoCache->find( psoHash );
				if ( cashedPSO != std::end( *m_psoCache ) )
				{
					cachedPSO.pCachedBlob = cashedPSO->second.Data();
					cachedPSO.CachedBlobSizeInBytes = cashedPSO->second.Size();
				}
			}

			CD3DX12_PIPELINE_STATE_STREAM2 subobjectStream;
			Microsoft::WRL::ComPtr<ID3D12PipelineState> newPipelineState;
			const GraphicsPipelineStateDesc& desc = pipelineState->GetDesc();
			if ( desc.m_meshShader.Get() )
			{
				D3DX12_MESH_SHADER_PIPELINE_STATE_DESC meshShaderPipelineStateDesc = {
					.pRootSignature = desc.m_rootSignature.Get() ? desc.m_rootSignature->Resource() : nullptr,
					.AS = {
						.pShaderBytecode = desc.m_amplificationShader.Get() ? desc.m_amplificationShader->ByteCode() : nullptr,
						.BytecodeLength = desc.m_amplificationShader.Get() ? desc.m_amplificationShader->ByteCodeSize() : 0
					},
					.MS = {
						.pShaderBytecode = desc.m_meshShader.Get() ? desc.m_meshShader->ByteCode() : nullptr,
						.BytecodeLength = desc.m_meshShader.Get() ? desc.m_meshShader->ByteCodeSize() : 0
					},
					.PS = {
						.pShaderBytecode = desc.m_pixelShader.Get() ? desc.m_pixelShader->ByteCode() : nullptr,
						.BytecodeLength = desc.m_pixelShader.Get() ? desc.m_pixelShader->ByteCodeSize() : 0
					},
					.BlendState = desc.m_blendState.Get()
								? desc.m_blendState->GetDesc()
								: D3D12_BLEND_DESC{
									.AlphaToCoverageEnable = false,
									.IndependentBlendEnable = false,
									.RenderTarget = {
										{
											.BlendEnable = false,
											.LogicOpEnable = false,
											.SrcBlend = D3D12_BLEND_ONE,
											.DestBlend = D3D12_BLEND_ZERO,
											.BlendOp = D3D12_BLEND_OP_ADD,
											.SrcBlendAlpha = D3D12_BLEND_ONE,
											.DestBlendAlpha = D3D12_BLEND_ZERO,
											.BlendOpAlpha = D3D12_BLEND_OP_ADD,
											.LogicOp = D3D12_LOGIC_OP_NOOP,
											.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
										}
									}
								},
					.SampleMask = desc.m_blendState.Get() ? desc.m_blendState->SamplerMask() : D3D12_DEFAULT_SAMPLE_MASK,
					.RasterizerState = desc.m_rasterizerState.Get()
									? desc.m_rasterizerState->GetDesc()
									: D3D12_RASTERIZER_DESC{
										.FillMode = D3D12_FILL_MODE_SOLID,
										.CullMode = D3D12_CULL_MODE_BACK,
										.FrontCounterClockwise = false,
										.DepthBias = 0,
										.DepthBiasClamp = 0.f,
										.SlopeScaledDepthBias = 0.f,
										.DepthClipEnable = true,
										.MultisampleEnable = false,
										.AntialiasedLineEnable = false,
										.ForcedSampleCount = 0,
										.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
									},
					.DepthStencilState = desc.m_depthStencilState.Get()
										? desc.m_depthStencilState->GetDesc()
										: D3D12_DEPTH_STENCIL_DESC{
											.DepthEnable = true,
											.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
											.DepthFunc = D3D12_COMPARISON_FUNC_LESS,
											.StencilEnable = false,
											.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
											.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
											.FrontFace = {
												.StencilFailOp = D3D12_STENCIL_OP_KEEP,
												.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
												.StencilPassOp = D3D12_STENCIL_OP_KEEP,
												.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS
											},
											.BackFace = {
												.StencilFailOp = D3D12_STENCIL_OP_KEEP,
												.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
												.StencilPassOp = D3D12_STENCIL_OP_KEEP,
												.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS
											},
										},
					.PrimitiveTopologyType = ConvertPrimToD3D12PrimType( desc.m_primitiveType ),
					.NumRenderTargets = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT,
					.RTVFormats =
					{
						rtvFormats[0],
						rtvFormats[1],
						rtvFormats[2],
						rtvFormats[3],
						rtvFormats[4],
						rtvFormats[5],
						rtvFormats[6],
						rtvFormats[7]
					},
					.DSVFormat = dsvFormat,
					.SampleDesc = {
						.Count = 1,
						.Quality = 0
					},
					.NodeMask = 0,
					.CachedPSO = cachedPSO,
					.Flags = D3D12_PIPELINE_STATE_FLAG_NONE
				};

				std::construct_at( &subobjectStream, meshShaderPipelineStateDesc );
			}
			else
			{
				D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc = {
					.pRootSignature = desc.m_rootSignature.Get() ? desc.m_rootSignature->Resource() : nullptr,
					.VS = {
						.pShaderBytecode = desc.m_vertexShader.Get() ? desc.m_vertexShader->ByteCode() : nullptr,
						.BytecodeLength = desc.m_vertexShader.Get() ? desc.m_vertexShader->ByteCodeSize() : 0
					},
					.PS = {
						.pShaderBytecode = desc.m_pixelShader.Get() ? desc.m_pixelShader->ByteCode() : nullptr,
						.BytecodeLength = desc.m_pixelShader.Get() ? desc.m_pixelShader->ByteCodeSize() : 0
					},
					.DS = {
						.pShaderBytecode = nullptr,
						.BytecodeLength = 0
					},
					.HS = {
						.pShaderBytecode = nullptr,
						.BytecodeLength = 0
					},
					.GS = {
						.pShaderBytecode = desc.m_geometryShader.Get() ? desc.m_geometryShader->ByteCode() : nullptr,
						.BytecodeLength = desc.m_geometryShader.Get() ? desc.m_geometryShader->ByteCodeSize() : 0
					},
					.StreamOutput = {
						.pSODeclaration = nullptr,
						.NumEntries = 0,
						.pBufferStrides = 0,
						.NumStrides = 0,
						.RasterizedStream = 0
					},
					.BlendState = desc.m_blendState.Get()
								? desc.m_blendState->GetDesc()
								: D3D12_BLEND_DESC{
									.AlphaToCoverageEnable = false,
									.IndependentBlendEnable = false,
									.RenderTarget = {
										{
											.BlendEnable = false,
											.LogicOpEnable = false,
											.SrcBlend = D3D12_BLEND_ONE,
											.DestBlend = D3D12_BLEND_ZERO,
											.BlendOp = D3D12_BLEND_OP_ADD,
											.SrcBlendAlpha = D3D12_BLEND_ONE,
											.DestBlendAlpha = D3D12_BLEND_ZERO,
											.BlendOpAlpha = D3D12_BLEND_OP_ADD,
											.LogicOp = D3D12_LOGIC_OP_NOOP,
											.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
										}
									}
								},
					.SampleMask = desc.m_blendState.Get() ? desc.m_blendState->SamplerMask() : D3D12_DEFAULT_SAMPLE_MASK,
					.RasterizerState = desc.m_rasterizerState.Get()
									? desc.m_rasterizerState->GetDesc()
									: D3D12_RASTERIZER_DESC{
										.FillMode = D3D12_FILL_MODE_SOLID,
										.CullMode = D3D12_CULL_MODE_BACK,
										.FrontCounterClockwise = false,
										.DepthBias = 0,
										.DepthBiasClamp = 0.f,
										.SlopeScaledDepthBias = 0.f,
										.DepthClipEnable = true,
										.MultisampleEnable = false,
										.AntialiasedLineEnable = false,
										.ForcedSampleCount = 0,
										.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
									},
					.DepthStencilState = desc.m_depthStencilState.Get()
										? desc.m_depthStencilState->GetDesc()
										: D3D12_DEPTH_STENCIL_DESC{
											.DepthEnable = true,
											.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
											.DepthFunc = D3D12_COMPARISON_FUNC_LESS,
											.StencilEnable = false,
											.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
											.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
											.FrontFace = {
												.StencilFailOp = D3D12_STENCIL_OP_KEEP,
												.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
												.StencilPassOp = D3D12_STENCIL_OP_KEEP,
												.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS
											},
											.BackFace = {
												.StencilFailOp = D3D12_STENCIL_OP_KEEP,
												.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
												.StencilPassOp = D3D12_STENCIL_OP_KEEP,
												.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS
											},
										},
					.InputLayout = {
						.pInputElementDescs = desc.m_vertexLayout.Get() ? desc.m_vertexLayout->GetDesc().data() : nullptr,
						.NumElements = desc.m_vertexLayout.Get() ? static_cast<uint32>( desc.m_vertexLayout->GetDesc().size() ) : 0
					},
					.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
					.PrimitiveTopologyType = ConvertPrimToD3D12PrimType( desc.m_primitiveType ),
					.NumRenderTargets = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT,
					.RTVFormats =
					{
						rtvFormats[0],
						rtvFormats[1],
						rtvFormats[2],
						rtvFormats[3],
						rtvFormats[4],
						rtvFormats[5],
						rtvFormats[6],
						rtvFormats[7]
					},
					.DSVFormat = dsvFormat,
					.SampleDesc = {
						.Count = 1,
						.Quality = 0
					},
					.NodeMask = 0,
					.CachedPSO = cachedPSO,
					.Flags = D3D12_PIPELINE_STATE_FLAG_NONE
				};

				std::construct_at( &subobjectStream, graphicsPipelineStateDesc );
			}

			D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
				.SizeInBytes = sizeof( subobjectStream ),
				.pPipelineStateSubobjectStream = &subobjectStream,
			};

			HRESULT hr = D3D12Device().CreatePipelineState( &pipelineStateStreamDesc, IID_PPV_ARGS( newPipelineState.GetAddressOf() ) );
			if ( FAILED( hr ) )
			{
				subobjectStream.CachedPSO = {};

				hr = D3D12Device().CreatePipelineState( &pipelineStateStreamDesc, IID_PPV_ARGS( newPipelineState.GetAddressOf() ) );
				assert( SUCCEEDED( hr ) );
			}

			m_d3d12PipelineState.emplace( key, newPipelineState );

			EnqueueRenderTask(
				[this, psoHash, state = newPipelineState]()
				{
					UpdatePSOCache( psoHash, state.Get() );
				} );

			return newPipelineState.Get();
		}
	}

	D3D12DisposableConstantBufferPool& D3D12ResourceManager::GetDisposableConstantBufferPool()
	{
		m_d3d12DisposbleConstantBufferPool.resize( DefaultAgl::GetBufferCount() );
		return m_d3d12DisposbleConstantBufferPool[GetFrameIndex()];
	}

	void D3D12ResourceManager::Prepare()
	{
		GetDisposableConstantBufferPool().Prepare();
	}

	D3D12ResourceManager::~D3D12ResourceManager()
	{
		Shutdown();
	}

	void D3D12ResourceManager::UpdatePSOCache( size_t hash, ID3D12PipelineState* pipelineState )
	{
		if ( m_psoCache == nullptr
			|| pipelineState == nullptr )
		{
			return;
		}

		Microsoft::WRL::ComPtr<ID3DBlob> cachedBlob;
		[[maybe_unused]] HRESULT hr = pipelineState->GetCachedBlob( cachedBlob.GetAddressOf() );
		assert( SUCCEEDED( hr ) );

		BinaryChunk cachedPSO( static_cast<uint32>( cachedBlob->GetBufferSize() ) );
		std::memcpy( cachedPSO.Data(), cachedBlob->GetBufferPointer(), cachedPSO.Size() );

		( *m_psoCache )[hash] = cachedPSO;
	}

	Owner<IResourceManager*> CreateD3D12ResourceManager()
	{
		return new D3D12ResourceManager();
	}
}
