#include "stdafx.h"
#include "D3D11PipelineState.h"

#include "D3D11BlendState.h"
#include "D3D11DepthStencilState.h"
#include "D3D11FlagConvertor.h"
#include "D3D11RasterizerState.h"
#include "D3D11Shaders.h"
#include "D3D11VetexLayout.h"
#include "IAgl.h"
#include "InterfaceFactories.h"

namespace agl
{
	ID3D11VertexShader* D3D11GraphicsPipelineState::VertexShader()
	{
		return m_vertexShader;
	}

	const ID3D11VertexShader* D3D11GraphicsPipelineState::VertexShader() const
	{
		return m_vertexShader;
	}

	ID3D11GeometryShader* D3D11GraphicsPipelineState::GeometryShader()
	{
		return m_geometryShader;
	}

	const ID3D11GeometryShader* D3D11GraphicsPipelineState::GeometryShader() const
	{
		return m_geometryShader;
	}

	ID3D11PixelShader* D3D11GraphicsPipelineState::PixelShader()
	{
		return m_pixelShader;
	}

	const ID3D11PixelShader* D3D11GraphicsPipelineState::PixelShader() const
	{
		return m_pixelShader;
	}

	ID3D11BlendState* D3D11GraphicsPipelineState::BlendState()
	{
		return m_blendState;
	}

	const ID3D11BlendState* D3D11GraphicsPipelineState::BlendState() const
	{
		return m_blendState;
	}

	ID3D11RasterizerState* D3D11GraphicsPipelineState::RasterizerState()
	{
		return m_rasterizerState;
	}

	const ID3D11RasterizerState* D3D11GraphicsPipelineState::RasterizerState() const
	{
		return m_rasterizerState;
	}

	ID3D11DepthStencilState* D3D11GraphicsPipelineState::DepthStencilState()
	{
		return m_depthStencilState;
	}

	const ID3D11DepthStencilState* D3D11GraphicsPipelineState::DepthStencilState() const
	{
		return m_depthStencilState;
	}

	ID3D11InputLayout* D3D11GraphicsPipelineState::InputLayout()
	{
		return m_inputLayout;
	}

	const ID3D11InputLayout* D3D11GraphicsPipelineState::InputLayout() const
	{
		return m_inputLayout;
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
		if ( auto vs = static_cast<D3D11VertexShader*>( initializer.m_vertexShader ) )
		{
			m_vertexShader = vs->Resource();
			if ( m_vertexShader )
			{
				m_vertexShader->AddRef();
			}
		}

		if ( auto gs = static_cast<D3D11GeometryShader*>( initializer.m_geometryShader ) )
		{
			m_geometryShader = gs->Resource();
			if ( m_geometryShader )
			{
				m_geometryShader->AddRef();
			}
		}

		if ( auto ps = static_cast<D3D11PixelShader*>( initializer.m_piexlShader ) )
		{
			m_pixelShader = ps->Resource();
			if ( m_pixelShader )
			{
				m_pixelShader->AddRef();
			}
		}

		if ( auto blend = static_cast<D3D11BlendState*>( initializer.m_blendState ) )
		{
			m_blendState = blend->Resource();
			if ( m_blendState )
			{
				m_blendState->AddRef();
			}
		}

		if ( auto rasterizer = static_cast<D3D11RasterizerState*>( initializer.m_rasterizerState ) )
		{
			m_rasterizerState = rasterizer->Resource();
			if ( m_rasterizerState )
			{
				m_rasterizerState->AddRef();
			}
		}

		if ( auto depthStencil = static_cast<D3D11DepthStencilState*>( initializer.m_depthStencilState ) )
		{
			m_depthStencilState = depthStencil->Resource();
			if ( m_depthStencilState )
			{
				m_depthStencilState->AddRef();
			}
		}

		if ( auto vertexLayout = static_cast<D3D11VertexLayout*>( initializer.m_vertexLayout ) )
		{
			m_inputLayout = vertexLayout->Resource();
			if ( m_inputLayout )
			{
				m_inputLayout->AddRef();
			}
		}
	}

	void D3D11GraphicsPipelineState::FreeResource()
	{
		if ( m_vertexShader )
		{
			m_vertexShader->Release();
			m_vertexShader = nullptr;
		}

		if ( m_geometryShader )
		{
			m_geometryShader->Release();
			m_geometryShader = nullptr;
		}

		if ( m_pixelShader )
		{
			m_pixelShader->Release();
			m_pixelShader = nullptr;
		}

		if ( m_blendState )
		{
			m_blendState->Release();
			m_blendState = nullptr;
		}

		if ( m_rasterizerState )
		{
			m_rasterizerState->Release();
			m_rasterizerState = nullptr;
		}

		if ( m_depthStencilState )
		{
			m_depthStencilState->Release();
			m_depthStencilState = nullptr;
		}

		if ( m_inputLayout )
		{
			m_inputLayout->Release();
			m_inputLayout = nullptr;
		}
	}

	ID3D11ComputeShader* D3D11ComputePipelineState::ComputeShader()
	{
		return m_computeShader;
	}

	const ID3D11ComputeShader* D3D11ComputePipelineState::ComputeShader() const
	{
		return m_computeShader;
	}

	D3D11ComputePipelineState::D3D11ComputePipelineState( const ComputePipelineStateInitializer& initializer )
	{
		if ( auto cs = static_cast<D3D11ComputeShader*>( initializer.m_computeShader ) )
		{
			m_computeShader = cs->Resource();
			if ( m_computeShader )
			{
				m_computeShader->AddRef();
			}
		}
	}

	void D3D11ComputePipelineState::FreeResource()
	{
		if ( m_computeShader )
		{
			m_computeShader->Release();
			m_computeShader = nullptr;
		}
	}
}
