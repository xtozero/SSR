#include "stdafx.h"
#include "D3D11PipelineState.h"

#include "Core/InterfaceFactories.h"
#include "D3D11BlendState.h"
#include "D3D11DepthStencilState.h"
#include "D3D11FlagConvertor.h"
#include "D3D11RasterizerState.h"
#include "D3D11Shaders.h"
#include "D3D11VetexLayout.h"
#include "IAga.h"

namespace aga
{
	ID3D11VertexShader* D3D11PipelineState::VertexShader( )
	{
		return m_vertexShader;
	}

	const ID3D11VertexShader* D3D11PipelineState::VertexShader( ) const
	{
		return m_vertexShader;
	}

	ID3D11PixelShader* D3D11PipelineState::PixelShader( )
	{
		return m_pixelShader;
	}

	const ID3D11PixelShader* D3D11PipelineState::PixelShader( ) const
	{
		return m_pixelShader;
	}

	ID3D11BlendState* D3D11PipelineState::BlendState( )
	{
		return m_blendState;
	}

	const ID3D11BlendState* D3D11PipelineState::BlendState( ) const
	{
		return m_blendState;
	}

	ID3D11RasterizerState* D3D11PipelineState::RasterizerState( )
	{
		return m_rasterizerState;
	}

	const ID3D11RasterizerState* D3D11PipelineState::RasterizerState( ) const
	{
		return m_rasterizerState;
	}

	ID3D11DepthStencilState* D3D11PipelineState::DepthStencilState( )
	{
		return m_depthStencilState;
	}

	const ID3D11DepthStencilState* D3D11PipelineState::DepthStencilState( ) const
	{
		return m_depthStencilState;
	}

	ID3D11InputLayout* D3D11PipelineState::InputLayout( )
	{
		return m_inputLayout;
	}

	const ID3D11InputLayout* D3D11PipelineState::InputLayout( ) const
	{
		return m_inputLayout;
	}

	D3D11_PRIMITIVE_TOPOLOGY D3D11PipelineState::PrimitiveTopology( ) const
	{
		return m_primitiveTopology;
	}

	UINT D3D11PipelineState::SampleMask( ) const
	{
		return m_sampleMask;
	}

	D3D11PipelineState::D3D11PipelineState( const PipelineStateInitializer& initializer ) : m_primitiveTopology( ConvertPrimToD3D11Prim( initializer.m_primitiveType ) )
	{
		if ( auto vs = static_cast<D3D11VertexShader*>( initializer.m_vertexShader ) )
		{
			m_vertexShader = vs->Resource( );
			if ( m_vertexShader )
			{
				m_vertexShader->AddRef( );
			}
		}

		if ( auto ps = static_cast<D3D11PixelShader*>( initializer.m_piexlShader ) )
		{
			m_pixelShader = ps->Resource( );
			if ( m_pixelShader )
			{
				m_pixelShader->AddRef( );
			}
		}

		if ( auto blend = static_cast<D3D11BlendState*>( initializer.m_blendState ) )
		{
			m_blendState = blend->Resource( );
			if ( m_blendState )
			{
				m_blendState->AddRef( );
			}
		}

		if ( auto rasterizer = static_cast<D3D11RasterizerState*>( initializer.m_rasterizerState ) )
		{
			m_rasterizerState = rasterizer->Resource( );
			if ( m_rasterizerState )
			{
				m_rasterizerState->AddRef( );
			}
		}

		if ( auto depthStencil = static_cast<D3D11DepthStencilState*>( initializer.m_depthStencilState ) )
		{
			m_depthStencilState = depthStencil->Resource( );
			if ( m_depthStencilState )
			{
				m_depthStencilState->AddRef( );
			}
		}

		if ( auto vertexLayout = static_cast<D3D11VertexLayout*>( initializer.m_vertexLayout ) )
		{
			m_inputLayout = vertexLayout->Resource( );
			if ( m_inputLayout )
			{
				m_inputLayout->AddRef( );
			}
		}
	}

	void D3D11PipelineState::FreeResource( )
	{
		if ( m_vertexShader )
		{
			m_vertexShader->Release( );
			m_vertexShader = nullptr;
		}

		if ( m_pixelShader )
		{
			m_pixelShader->Release( );
			m_pixelShader = nullptr;
		}
		
		if ( m_blendState )
		{
			m_blendState->Release( );
			m_blendState = nullptr;
		}

		if ( m_rasterizerState )
		{
			m_rasterizerState->Release( );
			m_rasterizerState = nullptr;
		}
		
		if ( m_depthStencilState )
		{
			m_depthStencilState->Release( );
			m_depthStencilState = nullptr;
		}

		if ( m_inputLayout )
		{
			m_inputLayout->Release( );
			m_inputLayout = nullptr;
		}
	}
}
