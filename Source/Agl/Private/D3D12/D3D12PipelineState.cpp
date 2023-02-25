#include "D3D12PipelineState.h"

#include "D3D12Api.h"

namespace agl
{
	D3D12GraphicsPipelineState::D3D12GraphicsPipelineState( const GraphicsPipelineStateInitializer& initializer )
		: m_vertexShader( static_cast<D3D12VertexShader*>( initializer.m_vertexShader ) )
		, m_pixelShader( static_cast<D3D12PixelShader*>( initializer.m_piexlShader ) )
		// Reserved
		//, m_domainShader( nullptr )
		//, m_hullShader( nullptr )
		, m_geometryShader( static_cast<D3D12GeometryShader*>( initializer.m_geometryShader ) )
		, m_blendState( static_cast<D3D12BlendState*>( initializer.m_blendState ) )
		, m_rasterizerState( static_cast<D3D12RasterizerState*>( initializer.m_rasterizerState ) )
		, m_depthStencilState( static_cast<D3D12DepthStencilState*>( initializer.m_depthStencilState ) )
		, m_vertexLayout( static_cast<D3D12VertexLayout*>( initializer.m_vertexLayout ) )
	{
		m_rootSignature = new D3D12RootSignature( initializer );
	}

	void D3D12GraphicsPipelineState::InitResource()
	{
		HRESULT hr = D3D12Device().CreateGraphicsPipelineState( &m_desc, IID_PPV_ARGS( &m_pipelineState ) );
		assert( SUCCEEDED( hr ) && "CreateGraphicsPipelineState failed" );

		m_rootSignature->Init();
	}

	void D3D12GraphicsPipelineState::FreeResource()
	{
		if ( m_pipelineState )
		{
			m_pipelineState->Release();
			m_pipelineState = nullptr;
		}

		m_rootSignature->Free();
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
