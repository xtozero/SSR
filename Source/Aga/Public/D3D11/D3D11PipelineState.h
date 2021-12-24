#pragma once

#include "PipelineState.h"

#include <d3d11.h>

namespace aga
{
	class D3D11PipelineState : public PipelineState
	{
	public:
		ID3D11VertexShader* VertexShader( );
		const ID3D11VertexShader* VertexShader( ) const;

		ID3D11GeometryShader* GeometryShader( );
		const ID3D11GeometryShader* GeometryShader( ) const;

		ID3D11PixelShader* PixelShader( );
		const ID3D11PixelShader* PixelShader( ) const;
		
		ID3D11BlendState* BlendState( );
		const ID3D11BlendState* BlendState( ) const;

		ID3D11RasterizerState* RasterizerState( );
		const ID3D11RasterizerState* RasterizerState( ) const;

		ID3D11DepthStencilState* DepthStencilState( );
		const ID3D11DepthStencilState* DepthStencilState( ) const;

		ID3D11InputLayout* InputLayout( );
		const ID3D11InputLayout* InputLayout( ) const;

		D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology( ) const;

		uint32 SampleMask( ) const;

		explicit D3D11PipelineState( const PipelineStateInitializer& initializer );
		D3D11PipelineState( const D3D11PipelineState& ) = delete;
		D3D11PipelineState( D3D11PipelineState&& ) = delete;
		D3D11PipelineState& operator=( const D3D11PipelineState& ) = delete;
		D3D11PipelineState& operator=( D3D11PipelineState&& ) = delete;

	private:
		virtual void InitResource( ) override { }
		virtual void FreeResource( ) override;

		ID3D11VertexShader* m_vertexShader = nullptr;
		ID3D11GeometryShader* m_geometryShader = nullptr;
		ID3D11PixelShader* m_pixelShader = nullptr;
		ID3D11BlendState* m_blendState = nullptr;
		ID3D11RasterizerState* m_rasterizerState = nullptr;
		ID3D11DepthStencilState* m_depthStencilState = nullptr;
		ID3D11InputLayout* m_inputLayout = nullptr;
		D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		uint32 m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
	};
}