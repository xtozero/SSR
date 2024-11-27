#include "D3D12PipelineState.h"

#include "D3D12Api.h"
#include "D3D12FlagConvertor.h"

namespace agl
{
	const GraphicsPipelineStateDesc& D3D12GraphicsPipelineState::GetDesc() const
	{
		return m_desc;
	}

	D3D12RootSignature* D3D12GraphicsPipelineState::GetRootSignature() const
	{
		return m_desc.m_rootSignature.Get();
	}

	D3D12_PRIMITIVE_TOPOLOGY D3D12GraphicsPipelineState::GetPrimitiveTopology() const
	{
		return ConvertPrimToD3D12Prim( m_desc.m_primitiveType );
	}

	D3D12GraphicsPipelineState::D3D12GraphicsPipelineState( const GraphicsPipelineStateInitializer& initializer )
	{
		m_desc = {
			.m_rootSignature = new D3D12RootSignature( initializer ),
			.m_vertexShader = static_cast<D3D12VertexShader*>( initializer.m_vertexShader ),
			.m_geometryShader = static_cast<D3D12GeometryShader*>( initializer.m_geometryShader ),
			.m_pixelShader = static_cast<D3D12PixelShader*>( initializer.m_piexlShader ),
			.m_amplificationShader = static_cast<D3D12AmplificationShader*>( initializer.m_amplificationShader ),
			.m_meshShader = static_cast<D3D12MeshShader*>( initializer.m_meshShader ),
			.m_blendState = static_cast<D3D12BlendState*>( initializer.m_blendState ),
			.m_rasterizerState = static_cast<D3D12RasterizerState*>( initializer.m_rasterizerState ),
			.m_depthStencilState = static_cast<D3D12DepthStencilState*>( initializer.m_depthStencilState ),
			.m_vertexLayout = static_cast<D3D12VertexLayout*>( initializer.m_vertexLayout ),
			.m_primitiveType = initializer.m_primitiveType,
		};
	}

	void D3D12GraphicsPipelineState::InitResource()
	{
		m_desc.m_rootSignature->Init();
	}

	void D3D12GraphicsPipelineState::FreeResource()
	{
		m_desc.m_rootSignature = nullptr;
	}

	ID3D12PipelineState* D3D12ComputePipelineState::Resource() const
	{
		return m_pipelineState;
	}

	D3D12RootSignature* D3D12ComputePipelineState::GetRootSignature() const
	{
		return m_rootSignature;
	}

	D3D12ComputePipelineState::D3D12ComputePipelineState( const ComputePipelineStateInitializer& initializer, const BinaryChunk* cachedPSO )
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
				.pCachedBlob = cachedPSO ? cachedPSO->Data() : nullptr,
				.CachedBlobSizeInBytes = cachedPSO ? cachedPSO->Size() : 0
			},
			.Flags = D3D12_PIPELINE_STATE_FLAG_NONE
		};
	}

	void D3D12ComputePipelineState::InitResource()
	{
		m_rootSignature->Init();
		m_desc.pRootSignature = m_rootSignature->Resource();

		HRESULT hr = D3D12Device().CreateComputePipelineState( &m_desc, IID_PPV_ARGS( &m_pipelineState ) );
		if ( FAILED( hr ) )
		{
			m_desc.CachedPSO.pCachedBlob = nullptr;
			m_desc.CachedPSO.CachedBlobSizeInBytes = 0;

			hr = D3D12Device().CreateComputePipelineState( &m_desc, IID_PPV_ARGS( &m_pipelineState ) );

			assert( SUCCEEDED( hr ) && "CreateComputePipelineState failed" );
		}
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
