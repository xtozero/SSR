#pragma once

#include "D3D11BlendState.h"
#include "D3D11DepthStencilState.h"
#include "D3D11RasterizerState.h"
#include "D3D11Shaders.h"
#include "D3D11VetexLayout.h"
#include "PipelineState.h"

#include <d3d11.h>

namespace agl
{
	class D3D11GraphicsPipelineState final : public GraphicsPipelineState
	{
	public:
		ID3D11VertexShader* VertexShader() const;

		ID3D11GeometryShader* GeometryShader() const;

		ID3D11PixelShader* PixelShader() const;

		ID3D11BlendState* BlendState() const;

		ID3D11RasterizerState* RasterizerState() const;

		ID3D11DepthStencilState* DepthStencilState() const;

		ID3D11InputLayout* InputLayout() const;

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;

		uint32 SampleMask() const;

		explicit D3D11GraphicsPipelineState( const GraphicsPipelineStateDesc& desc );
		D3D11GraphicsPipelineState( const D3D11GraphicsPipelineState& ) = delete;
		D3D11GraphicsPipelineState( D3D11GraphicsPipelineState&& ) = delete;
		D3D11GraphicsPipelineState& operator=( const D3D11GraphicsPipelineState& ) = delete;
		D3D11GraphicsPipelineState& operator=( D3D11GraphicsPipelineState&& ) = delete;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override;

		RefHandle<D3D11VertexShader> m_vertexShader;
		RefHandle<D3D11GeometryShader> m_geometryShader;
		RefHandle<D3D11PixelShader> m_pixelShader;
		RefHandle<D3D11BlendState> m_blendState;
		RefHandle<D3D11RasterizerState> m_rasterizerState;
		RefHandle<D3D11DepthStencilState> m_depthStencilState;
		RefHandle<D3D11VertexLayout> m_inputLayout;
		D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		uint32 m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
	};

	class D3D11ComputePipelineState final : public ComputePipelineState
	{
	public:
		ID3D11ComputeShader* ComputeShader() const;

		explicit D3D11ComputePipelineState( const ComputePipelineStateDesc& desc );
		D3D11ComputePipelineState( const D3D11ComputePipelineState& ) = delete;
		D3D11ComputePipelineState( D3D11ComputePipelineState&& ) = delete;
		D3D11ComputePipelineState& operator=( const D3D11ComputePipelineState& ) = delete;
		D3D11ComputePipelineState& operator=( D3D11ComputePipelineState&& ) = delete;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override;

		RefHandle<D3D11ComputeShader> m_computeShader;
	};
}