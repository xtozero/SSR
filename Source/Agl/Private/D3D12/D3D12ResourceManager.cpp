#include "D3D12ResourceManager.h"

#include "AccelerationStructure.h"

#include "Config/DefaultAglConfig.h"

#include "D3D12Api.h"
#include "D3D12BlendState.h"
#include "D3D12Buffer.h"
#include "D3D12DepthStencilState.h"
#include "D3D12FlagConvertor.h"
#include "D3D12PipelineState.h"
#include "D3D12Query.h"
#include "D3D12RasterizerState.h"
#include "D3D12RaytracingPipelineState.h"
#include "D3D12SamplerState.h"
#include "D3D12Shaders.h"
#include "D3D12Texture.h"
#include "D3D12VertexLayout.h"
#include "D3D12Viewport.h"

#include "DxgiFlagConvertor.h"
#include "DxgiSwapchain.h"

#include "Platform/WindowPlatformEngine.h"

#include <cassert>
#include <directx/d3dx12_pipeline_state_stream.h>

namespace agl
{
	void D3D12ResourceManager::Shutdown()
	{
	}

	Texture* D3D12ResourceManager::CreateTexture( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData ) const
	{
		Texture* newTexture = nullptr;
		if ( IsTexture2D( desc ) )
		{
			newTexture = new D3D12Texture2D( desc, debugName, initialState, initData );
		}
		else if ( IsTexture3D( desc ) )
		{
			newTexture = new D3D12Texture3D( desc, debugName, initialState, initData );
		}
		else
		{
			return nullptr;
		}

		return newTexture;
	}

	Buffer* D3D12ResourceManager::CreateBuffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData ) const
	{
		Buffer* newBuffer = nullptr;
		if ( HasAnyFlags( desc.m_bindType, ResourceBindType::ConstantBuffer ) )
		{
			if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Disposable ) )
			{
				newBuffer = new D3D12DisposableConstantBuffer( desc, debugName );
			}
			else
			{
				newBuffer = new D3D12ConstantBuffer( desc, debugName, initialState, initData );
			}
		}
		else if ( HasAnyFlags( desc.m_bindType, ResourceBindType::IndexBuffer ) )
		{
			newBuffer = new D3D12IndexBuffer( desc, debugName, initialState, initData );
		}
		else if ( HasAnyFlags( desc.m_bindType, ResourceBindType::VertexBuffer ) )
		{
			newBuffer = new D3D12VertexBuffer( desc, debugName, initialState, initData );
		} 
		else
		{
			newBuffer = new D3D12Buffer( desc, debugName, initialState, initData );
		}

		return newBuffer;
	}

	VertexLayout* D3D12ResourceManager::CreateVertexLayout( [[maybe_unused]] const VertexShader* vs, const VertexLayoutData* layoutData, uint32 size ) const
	{
		return new D3D12VertexLayout( layoutData, size );
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

	RayGenerationShader* D3D12ResourceManager::CreateRayGenerationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
	{
		return new D3D12RayGenerationShader( byteCode, byteCodeSize, paramInfo, exportName );
	}

	IntersectionShader* D3D12ResourceManager::CreateIntersectionShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
	{
		return new D3D12IntersectionShader( byteCode, byteCodeSize, paramInfo, exportName );
	}

	AnyHitShader* D3D12ResourceManager::CreateAnyHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
	{
		return new D3D12AnyHitShader( byteCode, byteCodeSize, paramInfo, exportName );
	}

	ClosestHitShader* D3D12ResourceManager::CreateClosestHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
	{
		return new D3D12ClosestHitShader( byteCode, byteCodeSize, paramInfo, exportName );
	}

	MissShader* D3D12ResourceManager::CreateMissShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
	{
		return new D3D12MissShader( byteCode, byteCodeSize, paramInfo, exportName );
	}

	CallableShader* D3D12ResourceManager::CreateCallableShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
	{
		return new D3D12CallableShader( byteCode, byteCodeSize, paramInfo, exportName );
	}

	BlendState* D3D12ResourceManager::CreateBlendState( const BlendStateDesc& desc ) const
	{
		return new D3D12BlendState( desc );
	}

	DepthStencilState* D3D12ResourceManager::CreateDepthStencilState( const DepthStencilStateDesc& desc ) const
	{
		return new D3D12DepthStencilState( desc );
	}

	RasterizerState* D3D12ResourceManager::CreateRasterizerState( const RasterizerStateDesc& desc ) const
	{
		return new D3D12RasterizerState( desc );
	}

	SamplerState* D3D12ResourceManager::CreateSamplerState( const SamplerStateDesc& desc ) const
	{
		return new D3D12SamplerState( desc );
	}

	GraphicsPipelineState* D3D12ResourceManager::CreatePipelineState( const GraphicsPipelineStateDesc& desc )
	{
		auto cached = m_graphicsPipelineStateCache.find( desc );
		if ( cached != std::end( m_graphicsPipelineStateCache ) )
		{
			return cached->second.Get();
		}

		auto pipelineState = new D3D12GraphicsPipelineState( desc );
		m_graphicsPipelineStateCache.emplace( desc, pipelineState );

		EnqueueRenderTask(
			[state = pipelineState]()
			{
				state->Init();
			} );

		return pipelineState;
	}

	ComputePipelineState* D3D12ResourceManager::CreatePipelineState( const ComputePipelineStateDesc& desc )
	{
		auto cached = m_computePipelineStateCache.find( desc );
		if ( cached != std::end( m_computePipelineStateCache ) )
		{
			return cached->second.Get();
		}

		auto pipelineState = new D3D12ComputePipelineState( desc );
		m_computePipelineStateCache.emplace( desc, pipelineState );

		EnqueueRenderTask(
			[state = pipelineState]()
			{
				state->Init();
			} );

		return pipelineState;
	}

	Canvas* D3D12ResourceManager::CreateCanvas( uint32 width, uint32 height, const engine::PlatformWindowContext& windowCtx, ResourceFormat format, const float4& clearColor ) const
	{
		return new DxgiSwapchain<AglType::D3D12>( D3D12DirectCommandQueue(), D3D12Factory(), width, height, DefaultAgl::GetBufferCount(), windowCtx.m_nativeWindow, ConvertFormatToDxgiFormat( format ), clearColor );
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

	void D3D12ResourceManager::SetPSOCache( const BinaryChunk& psoCache )
	{
		if ( GetInterface<IAgl>()->SupportsPSOLibraryCache() == false )
		{
			return;
		}

		HRESULT hr = D3D12Device().CreatePipelineLibrary( psoCache.Data(), psoCache.Size(), IID_PPV_ARGS( m_d3d12PipelineLibrary.GetAddressOf() ) );
		if ( hr == D3D12_ERROR_DRIVER_VERSION_MISMATCH )
		{
			hr = D3D12Device().CreatePipelineLibrary( nullptr, 0, IID_PPV_ARGS( m_d3d12PipelineLibrary.GetAddressOf() ) );
		}
	}

	BinaryChunk D3D12ResourceManager::SerializePSOLibraryCache()
	{
		if ( m_d3d12PipelineLibrary.Get() == nullptr )
		{
			return BinaryChunk();
		}

		BinaryChunk serializedCache( static_cast<uint32>( m_d3d12PipelineLibrary->GetSerializedSize() ) );
		m_d3d12PipelineLibrary->Serialize( serializedCache.Data(), serializedCache.Size() );

		return serializedCache;
	}

	void D3D12ResourceManager::PostReloadShaders()
	{
		{
			std::unique_lock<std::shared_mutex> lock( m_d3d12PipelineMutex );
			m_d3d12PipelineState.clear();
		}

		{
			std::unique_lock<std::shared_mutex> lock( m_d3d12ComputePipelineMutex );
			m_d3d12ComputePipelineState.clear();
		}
	}

	BLAS* D3D12ResourceManager::CreateBLAS( const BLASDesc& desc, const char* debugName ) const
	{
		if ( desc.m_vertexBuffer.Get() == nullptr )
		{
			return nullptr;
		}

		return new D3D12BLAS( desc, debugName );
	}

	TLAS* D3D12ResourceManager::CreateTLAS( const TLASDesc& desc, const char* debugName ) const
	{
		return new D3D12TLAS( desc, debugName );
	}

	RaytracingPipelineState* D3D12ResourceManager::CreateRaytracingPipelineState( const RaytracingPipelineStateDesc& desc )
	{
		size_t pipelineStateHash = desc.GetHash();

		{
			std::shared_lock<std::shared_mutex> lock( m_d3d12RaytracingPipelineMutex );
			auto found = m_d3d12RaytracingPipelineStates.find( pipelineStateHash );
			if ( found != std::end( m_d3d12RaytracingPipelineStates ) )
			{
				return found->second.Get();
			}
		}

		RefHandle<D3D12RaytracingPipelineState> newPipelineState = new D3D12RaytracingPipelineState( desc );
		EnqueueRenderTask(
			[pipelineState = newPipelineState]()
			{
				pipelineState->Init();
			} );

		std::unique_lock<std::shared_mutex> lock( m_d3d12RaytracingPipelineMutex );
		m_d3d12RaytracingPipelineStates.emplace( pipelineStateHash, newPipelineState );

		return newPipelineState.Get();
	}

	D3D12HitGroup* D3D12ResourceManager::CreateHitGroup( const HitGroupDesc& desc )
	{
		size_t hitGroupHash = desc.GetHash();

		{
			std::shared_lock<std::shared_mutex> lock( m_d3d12HitGroupMutex );
			auto found = m_d3d12HitGroups.find( hitGroupHash );
			if ( found != std::end( m_d3d12HitGroups ) )
			{
				return found->second.Get();
			}
		}

		RefHandle<D3D12HitGroup> newHitGroup = new D3D12HitGroup( desc );
		EnqueueRenderTask(
			[hitGroup = newHitGroup]()
			{
				hitGroup->Init();
			} );

		std::unique_lock<std::shared_mutex> lock( m_d3d12HitGroupMutex );
		m_d3d12HitGroups.emplace( hitGroupHash, newHitGroup );

		return newHitGroup.Get();
	}

	D3D12RaytracingShaderTable* D3D12ResourceManager::CreateRaytracingShaderTable( const RaytracingShaderTableDesc& desc )
	{
		size_t shaderTableHash = desc.GetHash();

		{
			std::shared_lock<std::shared_mutex> lock( m_d3d12ShaderTableMutex );
			auto found = m_d3d12ShaderTables.find( shaderTableHash );
			if ( found != std::end( m_d3d12ShaderTables ) )
			{
				return found->second.Get();
			}
		}

		RefHandle<D3D12RaytracingShaderTable> newShaderTable = new D3D12RaytracingShaderTable( desc );
		EnqueueRenderTask(
			[shaderTable = newShaderTable]()
			{
				shaderTable->Init();
			} );

		std::unique_lock<std::shared_mutex> lock( m_d3d12ShaderTableMutex );
		m_d3d12ShaderTables.emplace( shaderTableHash, newShaderTable );

		return newShaderTable.Get();
	}

	ID3D12PipelineState* D3D12ResourceManager::FindOrCreate( const D3D12ComputePipelineState* pipelineState )
	{
		size_t psoHash = pipelineState->GetHash();

		{
			std::shared_lock<std::shared_mutex> lock( m_d3d12ComputePipelineMutex );
			auto found = m_d3d12ComputePipelineState.find( psoHash );
			if ( found != std::end( m_d3d12ComputePipelineState ) )
			{
				return found->second.Get();
			}
		}

		{
			std::unique_lock<std::shared_mutex> lock( m_d3d12ComputePipelineMutex );
			auto found = m_d3d12ComputePipelineState.find( psoHash );
			if ( found != std::end( m_d3d12ComputePipelineState ) )
			{
				return found->second.Get();
			}

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

			D3D12ComputeShader* computeShader = pipelineState->GetComputeShader();
			D3D12RootSignature* rootSignature = pipelineState->GetRootSignature();

			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {
				.pRootSignature = rootSignature ? rootSignature->Resource() : nullptr,
				.CS = {
					.pShaderBytecode = computeShader->ByteCode(),
					.BytecodeLength = computeShader->ByteCodeSize()
				},
				.NodeMask = 0,
				.CachedPSO = cachedPSO,
				.Flags = D3D12_PIPELINE_STATE_FLAG_NONE
			};

			Microsoft::WRL::ComPtr<ID3D12PipelineState> newPipelineState;
			if ( m_d3d12PipelineLibrary.Get() != nullptr )
			{
				auto hashString = std::to_wstring( psoHash );
				m_d3d12PipelineLibrary->LoadComputePipeline( hashString.c_str(), &desc, IID_PPV_ARGS( newPipelineState.GetAddressOf() ) );
			}

			bool encounterNewPSO = false;
			if ( newPipelineState == nullptr )
			{
				encounterNewPSO = ( desc.CachedPSO.CachedBlobSizeInBytes == 0 );

				HRESULT hr = D3D12Device().CreateComputePipelineState( &desc, IID_PPV_ARGS( newPipelineState.GetAddressOf() ) );
				if ( FAILED( hr ) )
				{
					desc.CachedPSO.pCachedBlob = nullptr;
					desc.CachedPSO.CachedBlobSizeInBytes = 0;

					hr = D3D12Device().CreateComputePipelineState( &desc, IID_PPV_ARGS( newPipelineState.GetAddressOf() ) );

					assert( SUCCEEDED( hr ) && "CreateComputePipelineState failed" );

					encounterNewPSO = true;
				}
			}

			m_d3d12ComputePipelineState.emplace( psoHash, newPipelineState );

			if ( encounterNewPSO )
			{
				UpdatePSOCache( psoHash, newPipelineState.Get() );
			}

			return newPipelineState.Get();
		}
	}

	ID3D12PipelineState* D3D12ResourceManager::FindOrCreate( const D3D12GraphicsPipelineState* pipelineState, const DXGI_FORMAT( &rtvFormats )[8], DXGI_FORMAT dsvFormat )
	{
		struct GraphicsPipelineStateHasher
		{
			size_t operator()( const GraphicsPipelineState* state, const DXGI_FORMAT (&rtvFormats)[8], DXGI_FORMAT dsvFormat ) const
			{
				size_t hash = state ? state->GetHash() : 0;

				constexpr int32 numRTVFormats = std::extent_v<std::remove_cvref_t<decltype( rtvFormats )>>;
				for ( int32 i = 0; i < numRTVFormats; ++i )
				{
					int32 salt = ( ( i + 1 ) * 19937 );
					HashCombine( hash, static_cast<int32>( rtvFormats[i] ) + salt );
				}

				constexpr int32 salt = ( ( numRTVFormats + 1 ) * 19937 );
				HashCombine( hash, static_cast<int32>( dsvFormat ) + salt );

				return hash;
			}
		} hasher;

		size_t psoHash = hasher( pipelineState, rtvFormats, dsvFormat );

		{
			std::shared_lock<std::shared_mutex> lock( m_d3d12PipelineMutex );
			auto found = m_d3d12PipelineState.find( psoHash );
			if ( found != std::end( m_d3d12PipelineState ) )
			{
				return found->second.Get();
			}
		}

		{
			std::unique_lock<std::shared_mutex> lock( m_d3d12PipelineMutex );
			auto found = m_d3d12PipelineState.find( psoHash );
			if ( found != std::end( m_d3d12PipelineState ) )
			{
				return found->second.Get();
			}

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
			const D3D12GraphicsPipelineStateDesc& desc = pipelineState->GetDesc();
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
								? desc.m_blendState->GetD3DDesc()
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
									? desc.m_rasterizerState->GetD3DDesc()
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
										? desc.m_depthStencilState->GetD3DDesc()
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
						.pBufferStrides = nullptr,
						.NumStrides = 0,
						.RasterizedStream = 0
					},
					.BlendState = desc.m_blendState.Get()
								? desc.m_blendState->GetD3DDesc()
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
									? desc.m_rasterizerState->GetD3DDesc()
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
										? desc.m_depthStencilState->GetD3DDesc()
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
						.pInputElementDescs = desc.m_vertexLayout.Get() ? desc.m_vertexLayout->GetD3DDescs().data() : nullptr,
						.NumElements = desc.m_vertexLayout.Get() ? static_cast<uint32>( desc.m_vertexLayout->GetD3DDescs().size() ) : 0
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

			if ( m_d3d12PipelineLibrary.Get() != nullptr )
			{
				auto hashString = std::to_wstring( psoHash );
				m_d3d12PipelineLibrary->LoadPipeline( hashString.c_str(), &pipelineStateStreamDesc, IID_PPV_ARGS( newPipelineState.GetAddressOf() ) );
			}

			bool encounterNewPSO = false;
			if ( newPipelineState.Get() == nullptr )
			{
				encounterNewPSO = ( cachedPSO.CachedBlobSizeInBytes == 0 );

				HRESULT hr = D3D12Device().CreatePipelineState( &pipelineStateStreamDesc, IID_PPV_ARGS( newPipelineState.GetAddressOf() ) );
				if ( FAILED( hr ) )
				{
					subobjectStream.CachedPSO = {};

					hr = D3D12Device().CreatePipelineState( &pipelineStateStreamDesc, IID_PPV_ARGS( newPipelineState.GetAddressOf() ) );
					assert( SUCCEEDED( hr ) );

					encounterNewPSO = true;
				}
			}

			assert( m_d3d12PipelineState.contains( psoHash ) == false );
			m_d3d12PipelineState.emplace( psoHash, newPipelineState );

			if ( encounterNewPSO )
			{
				EnqueueRenderTask(
					[this, psoHash, state = newPipelineState]()
					{
						UpdatePSOCache( psoHash, state.Get() );
					} );
			}

			return newPipelineState.Get();
		}
	}

	ID3D12CommandSignature* D3D12ResourceManager::FindOrCreate( IndirectCommandType type )
	{
		auto found = m_d3d12IndirectCommandSignature.find( type );
		if ( found != std::end( m_d3d12IndirectCommandSignature ) )
		{
			return found->second.Get();
		}

		D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {
			.Type = ConvertToIndirectArgumentType( type ),
		};

		D3D12_COMMAND_SIGNATURE_DESC desc = {
			.ByteStride = GetIndirectArgumentStride( type ),
			.NumArgumentDescs = 1,
			.pArgumentDescs = &argumentDesc,
			.NodeMask = 0,
		};

		Microsoft::WRL::ComPtr<ID3D12CommandSignature> newCommandSignature;
		[[maybe_unused]] HRESULT hr = D3D12Device().CreateCommandSignature( &desc,
			nullptr, // If the only command present is a draw or dispatch, the root signature parameter can be set to nullptr.
			IID_PPV_ARGS( newCommandSignature.GetAddressOf() ) );
		assert( SUCCEEDED( hr ) );

		m_d3d12IndirectCommandSignature.emplace( type, newCommandSignature.Get() );

		return newCommandSignature.Get();
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
		if ( pipelineState == nullptr )
		{
			return;
		}

		if ( m_d3d12PipelineLibrary.Get() != nullptr )
		{
			auto hashString = std::to_wstring( hash );
			m_d3d12PipelineLibrary->StorePipeline( hashString.c_str(), pipelineState );
		}

		if ( m_psoCache != nullptr )
		{
			Microsoft::WRL::ComPtr<ID3DBlob> cachedBlob;
			[[maybe_unused]] HRESULT hr = pipelineState->GetCachedBlob( cachedBlob.GetAddressOf() );
			assert( SUCCEEDED( hr ) );

			BinaryChunk cachedPSO( static_cast<uint32>( cachedBlob->GetBufferSize() ) );
			std::memcpy( cachedPSO.Data(), cachedBlob->GetBufferPointer(), cachedPSO.Size() );

			( *m_psoCache )[hash] = cachedPSO;
		}
	}

	Owner<IResourceManager*> CreateD3D12ResourceManager()
	{
		return new D3D12ResourceManager();
	}

	uint32 GetIndirectArgumentStride( IndirectCommandType type )
	{
		switch ( type )
		{
		case IndirectCommandType::Draw:
			return sizeof( D3D12_DRAW_ARGUMENTS );
		case IndirectCommandType::DrawIndexed:
			return sizeof( D3D12_DRAW_INDEXED_ARGUMENTS );
		case IndirectCommandType::Dispatch:
			return sizeof( D3D12_DISPATCH_ARGUMENTS );
		case IndirectCommandType::DispatchMesh:
			return sizeof( D3D12_DISPATCH_MESH_ARGUMENTS );
		}

		assert( false && "GetIndirectArgumentStride - Invalid IndirectCommandType" );
		return 0;
	}
}
