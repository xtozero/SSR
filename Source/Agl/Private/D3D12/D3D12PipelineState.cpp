#include "D3D12PipelineState.h"

#include "D3D12Api.h"
#include "D3D12FlagConvertor.h"

namespace agl
{
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC& D3D12GraphicsPipelineState::GetDesc() const
	{
		return m_desc;
	}

	D3D12RootSignature* D3D12GraphicsPipelineState::GetRootSignature() const
	{
		return m_rootSignature;
	}

	D3D12_PRIMITIVE_TOPOLOGY D3D12GraphicsPipelineState::GetPrimitiveTopology() const
	{
		return m_primitiveTopology;
	}

	void D3D12GraphicsPipelineState::SetRTVFormat( const DXGI_FORMAT* formats, uint32 numFormat )
	{
		m_desc.NumRenderTargets = numFormat;
		std::memcpy( m_desc.RTVFormats, formats, numFormat );
	}

	void D3D12GraphicsPipelineState::SetDSVFormat( DXGI_FORMAT format )
	{
		m_desc.DSVFormat = format;
	}

	D3D12GraphicsPipelineState::D3D12GraphicsPipelineState( const GraphicsPipelineStateInitializer& initializer )
		: m_rootSignature( new D3D12RootSignature( initializer ) )
		, m_vertexShader( static_cast<D3D12VertexShader*>( initializer.m_vertexShader ) )
		, m_pixelShader( static_cast<D3D12PixelShader*>( initializer.m_piexlShader ) )
		// Reserved
		//, m_domainShader( nullptr )
		//, m_hullShader( nullptr )
		, m_geometryShader( static_cast<D3D12GeometryShader*>( initializer.m_geometryShader ) )
		, m_blendState( static_cast<D3D12BlendState*>( initializer.m_blendState ) )
		, m_rasterizerState( static_cast<D3D12RasterizerState*>( initializer.m_rasterizerState ) )
		, m_depthStencilState( static_cast<D3D12DepthStencilState*>( initializer.m_depthStencilState ) )
		, m_vertexLayout( static_cast<D3D12VertexLayout*>( initializer.m_vertexLayout ) )
		, m_primitiveTopology( ConvertPrimToD3D12Prim( initializer.m_primitiveType ) )
	{
		m_desc = {
			.pRootSignature = nullptr, // Assign later, See D3D12GraphicsPipelineState::InitResource
			.VS = {
				.pShaderBytecode = m_vertexShader ? m_vertexShader->ByteCode() : nullptr,
				.BytecodeLength = m_vertexShader ? m_vertexShader->ByteCodeSize() : 0
			},
			.PS = {
				.pShaderBytecode = m_pixelShader ? m_pixelShader->ByteCode() : nullptr,
				.BytecodeLength = m_pixelShader ? m_pixelShader->ByteCodeSize() : 0
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
				.pShaderBytecode = m_geometryShader ? m_geometryShader->ByteCode() : nullptr,
				.BytecodeLength = m_geometryShader ? m_geometryShader->ByteCodeSize() : 0
			},
			.StreamOutput = {
				.pSODeclaration = nullptr,
				.NumEntries = 0,
				.pBufferStrides = 0,
				.NumStrides = 0,
				.RasterizedStream = 0
			},
			.BlendState = m_blendState 
						? m_blendState->GetDesc() 
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
			.SampleMask = m_blendState ? m_blendState->SamplerMask() : D3D12_DEFAULT_SAMPLE_MASK,
			.RasterizerState = m_rasterizerState 
							? m_rasterizerState->GetDesc() 
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
			.DepthStencilState = m_depthStencilState 
								? m_depthStencilState->GetDesc() 
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
				.pInputElementDescs = m_vertexLayout ? m_vertexLayout->GetDesc().data() : nullptr,
				.NumElements = m_vertexLayout ? static_cast<uint32>( m_vertexLayout->GetDesc().size() ) : 0
			},
			.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
			.PrimitiveTopologyType = ConvertPrimToD3D12PrimType( initializer.m_primitiveType ),
			.NumRenderTargets = 0,
			.RTVFormats =
			{
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_UNKNOWN
			},
			.DSVFormat = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.NodeMask = 0,
			.CachedPSO = {
				.pCachedBlob = nullptr,
				.CachedBlobSizeInBytes = 0
			},
			.Flags = D3D12_PIPELINE_STATE_FLAG_NONE
		};
	}

	void D3D12GraphicsPipelineState::InitResource()
	{
		m_rootSignature->Init();
		m_desc.pRootSignature = m_rootSignature->Resource();
	}

	void D3D12GraphicsPipelineState::FreeResource()
	{
		m_rootSignature = nullptr;
	}

	ID3D12PipelineState* D3D12ComputePipelineState::Resource() const
	{
		return m_pipelineState;
	}

	D3D12RootSignature* D3D12ComputePipelineState::GetRootSignature() const
	{
		return m_rootSignature;
	}

	D3D12ComputePipelineState::D3D12ComputePipelineState( const ComputePipelineStateInitializer& initializer )
		: m_computeShader( static_cast<D3D12ComputeShader*>( initializer.m_computeShader ) )
	{
		m_rootSignature = new D3D12RootSignature( initializer );

		m_desc = {
			.pRootSignature = nullptr, // Assign later, See D3D12ComputePipelineState::InitResource
			.CS = {
				.pShaderBytecode = m_computeShader->ByteCode(),
				.BytecodeLength = m_computeShader->ByteCodeSize()
			},
			.NodeMask = 0,
			.CachedPSO = {
				.pCachedBlob = nullptr,
				.CachedBlobSizeInBytes = 0
			},
			.Flags = D3D12_PIPELINE_STATE_FLAG_NONE
		};
	}

	void D3D12ComputePipelineState::InitResource()
	{
		m_rootSignature->Init();
		m_desc.pRootSignature = m_rootSignature->Resource();

		HRESULT hr = D3D12Device().CreateComputePipelineState( &m_desc, IID_PPV_ARGS( &m_pipelineState ) );
		assert( SUCCEEDED( hr ) && "CreateComputePipelineState failed" );
	}

	void D3D12ComputePipelineState::FreeResource()
	{
		if ( m_pipelineState )
		{
			m_pipelineState->Release();
			m_pipelineState = nullptr;
		}

		m_rootSignature->Free();
	}
}
