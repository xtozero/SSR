#include "D3D11PipelineState.h"

#include "D3D11FlagConvertor.h"

namespace agl
{
	ID3D11VertexShader* D3D11GraphicsPipelineState::VertexShader()
	{
		return m_vertexShader.Get() ? m_vertexShader->Resource() : nullptr;
	}

	const ID3D11VertexShader* D3D11GraphicsPipelineState::VertexShader() const
	{
		return m_vertexShader.Get() ? m_vertexShader->Resource() : nullptr;
	}

	ID3D11GeometryShader* D3D11GraphicsPipelineState::GeometryShader()
	{
		return m_geometryShader.Get() ? m_geometryShader->Resource() : nullptr;
	}

	const ID3D11GeometryShader* D3D11GraphicsPipelineState::GeometryShader() const
	{
		return m_geometryShader.Get() ? m_geometryShader->Resource() : nullptr;
	}

	ID3D11PixelShader* D3D11GraphicsPipelineState::PixelShader()
	{
		return m_pixelShader.Get() ? m_pixelShader->Resource() : nullptr;
	}

	const ID3D11PixelShader* D3D11GraphicsPipelineState::PixelShader() const
	{
		return m_pixelShader.Get() ? m_pixelShader->Resource() : nullptr;
	}

	ID3D11BlendState* D3D11GraphicsPipelineState::BlendState()
	{
		return m_blendState.Get() ? m_blendState->Resource() : nullptr;
	}

	const ID3D11BlendState* D3D11GraphicsPipelineState::BlendState() const
	{
		return m_blendState.Get() ? m_blendState->Resource() : nullptr;
	}

	ID3D11RasterizerState* D3D11GraphicsPipelineState::RasterizerState()
	{
		return m_rasterizerState.Get() ? m_rasterizerState->Resource() : nullptr;
	}

	const ID3D11RasterizerState* D3D11GraphicsPipelineState::RasterizerState() const
	{
		return m_rasterizerState.Get() ? m_rasterizerState->Resource() : nullptr;
	}

	ID3D11DepthStencilState* D3D11GraphicsPipelineState::DepthStencilState()
	{
		return m_depthStencilState.Get() ? m_depthStencilState->Resource() : nullptr;
	}

	const ID3D11DepthStencilState* D3D11GraphicsPipelineState::DepthStencilState() const
	{
		return m_depthStencilState.Get() ? m_depthStencilState->Resource() : nullptr;
	}

	ID3D11InputLayout* D3D11GraphicsPipelineState::InputLayout()
	{
		return m_inputLayout.Get() ? m_inputLayout->Resource() : nullptr;
	}

	const ID3D11InputLayout* D3D11GraphicsPipelineState::InputLayout() const
	{
		return m_inputLayout.Get() ? m_inputLayout->Resource() : nullptr;
	}

	D3D11_PRIMITIVE_TOPOLOGY D3D11GraphicsPipelineState::GetPrimitiveTopology() const
	{
		return m_primitiveTopology;
	}

	uint32 D3D11GraphicsPipelineState::SampleMask() const
	{
		return m_sampleMask;
	}

	D3D11GraphicsPipelineState::D3D11GraphicsPipelineState( const GraphicsPipelineStateInitializer& initializer ) : m_primitiveTopology( ConvertPrimToD3D11Prim( initializer.m_primitiveType ) )
	{
		m_vertexShader =  static_cast<D3D11VertexShader*>( initializer.m_vertexShader );
		m_geometryShader = static_cast<D3D11GeometryShader*>( initializer.m_geometryShader );
		m_pixelShader = static_cast<D3D11PixelShader*>( initializer.m_piexlShader );
		m_blendState = static_cast<D3D11BlendState*>( initializer.m_blendState );
		m_rasterizerState = static_cast<D3D11RasterizerState*>( initializer.m_rasterizerState );
		m_depthStencilState = static_cast<D3D11DepthStencilState*>( initializer.m_depthStencilState );
		m_inputLayout = static_cast<D3D11VertexLayout*>( initializer.m_vertexLayout );
	}

	void D3D11GraphicsPipelineState::FreeResource()
	{
		m_vertexShader = nullptr;
		m_geometryShader = nullptr;
		m_pixelShader = nullptr;
		m_blendState = nullptr;
		m_rasterizerState = nullptr;
		m_depthStencilState = nullptr;
		m_inputLayout = nullptr;
	}

	ID3D11ComputeShader* D3D11ComputePipelineState::ComputeShader()
	{
		return m_computeShader.Get() ? m_computeShader->Resource() : nullptr;
	}

	const ID3D11ComputeShader* D3D11ComputePipelineState::ComputeShader() const
	{
		return m_computeShader.Get() ? m_computeShader->Resource() : nullptr;
	}

	D3D11ComputePipelineState::D3D11ComputePipelineState( const ComputePipelineStateInitializer& initializer )
	{
		if ( auto cs = static_cast<D3D11ComputeShader*>( initializer.m_computeShader ) )
		{
			m_computeShader = cs;
		}
	}

	void D3D11ComputePipelineState::FreeResource()
	{
		m_computeShader = nullptr;
	}
}
