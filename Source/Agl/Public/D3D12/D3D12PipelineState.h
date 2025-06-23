#pragma once

#include "D3D12BlendState.h"
#include "D3D12DepthStencilState.h"
#include "D3D12RasterizerState.h"
#include "D3D12RootSignature.h"
#include "D3D12Shaders.h"
#include "D3D12VertexLayout.h"
#include "GraphicsApiResource.h"
#include "PipelineState.h"

#include <d3d12.h>

namespace agl
{
	struct GraphicsPipelineStateDesc
	{
		RefHandle<D3D12RootSignature> m_rootSignature;
		RefHandle<D3D12VertexShader> m_vertexShader;
		// Reserved
		// RefHandle<D3D12VertexShader> m_hullShader;
		// RefHandle<D3D12VertexShader> m_domainShader;
		RefHandle<D3D12GeometryShader> m_geometryShader;
		RefHandle<D3D12PixelShader> m_pixelShader;
		RefHandle<D3D12MeshShader> m_meshShader;
		RefHandle<D3D12AmplificationShader> m_amplificationShader;
		RefHandle<D3D12BlendState> m_blendState;
		RefHandle<D3D12RasterizerState> m_rasterizerState;
		RefHandle<D3D12DepthStencilState> m_depthStencilState;
		RefHandle<D3D12VertexLayout> m_vertexLayout;
		ResourcePrimitive m_primitiveType;
	};

	class D3D12GraphicsPipelineState final : public GraphicsPipelineState
	{
	public:
		const GraphicsPipelineStateDesc& GetDesc() const;
		D3D12RootSignature* GetRootSignature() const;
		D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;

		explicit D3D12GraphicsPipelineState( const GraphicsPipelineStateInitializer& initializer );
		D3D12GraphicsPipelineState( const D3D12GraphicsPipelineState& ) = delete;
		D3D12GraphicsPipelineState( D3D12GraphicsPipelineState&& ) = delete;
		D3D12GraphicsPipelineState& operator=( const D3D12GraphicsPipelineState& ) = delete;
		D3D12GraphicsPipelineState& operator=( D3D12GraphicsPipelineState&& ) = delete;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		GraphicsPipelineStateDesc m_desc = {};
	};

	class D3D12ComputePipelineState final : public ComputePipelineState
	{
	public:
		D3D12RootSignature* GetRootSignature() const;
		D3D12ComputeShader* GetComputeShader() const;

		D3D12ComputePipelineState( const ComputePipelineStateInitializer& initializer );
		D3D12ComputePipelineState( const D3D12ComputePipelineState& ) = delete;
		D3D12ComputePipelineState( D3D12ComputePipelineState&& ) = delete;
		D3D12ComputePipelineState& operator=( const D3D12ComputePipelineState& ) = delete;
		D3D12ComputePipelineState& operator=( D3D12ComputePipelineState&& ) = delete;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		RefHandle<D3D12RootSignature> m_rootSignature;
		RefHandle<D3D12ComputeShader> m_computeShader;
	};
}
