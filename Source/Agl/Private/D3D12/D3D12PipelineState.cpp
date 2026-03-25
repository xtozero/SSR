#include "D3D12PipelineState.h"

#include "D3D12Api.h"
#include "D3D12FlagConvertor.h"

namespace agl
{
	const D3D12GraphicsPipelineStateDesc& D3D12GraphicsPipelineState::GetDesc() const
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

	D3D12GraphicsPipelineState::D3D12GraphicsPipelineState( const GraphicsPipelineStateDesc& desc )
	{
		m_desc = {
			.m_rootSignature = new D3D12RootSignature( desc ),
			.m_vertexShader = static_cast<D3D12VertexShader*>( desc.m_vertexShader ),
			.m_geometryShader = static_cast<D3D12GeometryShader*>( desc.m_geometryShader ),
			.m_pixelShader = static_cast<D3D12PixelShader*>( desc.m_piexlShader ),
			.m_meshShader = static_cast<D3D12MeshShader*>( desc.m_meshShader ),
			.m_amplificationShader = static_cast<D3D12AmplificationShader*>( desc.m_amplificationShader ),
			.m_blendState = static_cast<D3D12BlendState*>( desc.m_blendState ),
			.m_rasterizerState = static_cast<D3D12RasterizerState*>( desc.m_rasterizerState ),
			.m_depthStencilState = static_cast<D3D12DepthStencilState*>( desc.m_depthStencilState ),
			.m_vertexLayout = static_cast<D3D12VertexLayout*>( desc.m_vertexLayout ),
			.m_primitiveType = desc.m_primitiveType,
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

	D3D12RootSignature* D3D12ComputePipelineState::GetRootSignature() const
	{
		return m_rootSignature.Get();
	}

	D3D12ComputeShader* D3D12ComputePipelineState::GetComputeShader() const
	{
		return m_computeShader.Get();
	}

	D3D12ComputePipelineState::D3D12ComputePipelineState( const ComputePipelineStateDesc& desc )
		: m_computeShader( static_cast<D3D12ComputeShader*>( desc.m_computeShader ) )
	{
		m_rootSignature = new D3D12RootSignature( desc );
	}

	void D3D12ComputePipelineState::InitResource()
	{
		m_rootSignature->Init();
	}

	void D3D12ComputePipelineState::FreeResource()
	{
		m_rootSignature->Free();
	}
}
