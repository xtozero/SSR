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
	class D3D12GraphicsPipelineState final : public GraphicsPipelineState
	{
	public:
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetDesc() const;
		D3D12RootSignature* GetRootSignature() const;
		D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;

		void SetRTVFormat( const DXGI_FORMAT* formats, uint32 numFormat );
		void SetDSVFormat( DXGI_FORMAT format );

		explicit D3D12GraphicsPipelineState( const GraphicsPipelineStateInitializer& initializer );
		D3D12GraphicsPipelineState( const D3D12GraphicsPipelineState& ) = delete;
		D3D12GraphicsPipelineState( D3D12GraphicsPipelineState&& ) = delete;
		D3D12GraphicsPipelineState& operator=( const D3D12GraphicsPipelineState& ) = delete;
		D3D12GraphicsPipelineState& operator=( D3D12GraphicsPipelineState&& ) = delete;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc = {};

		RefHandle<D3D12RootSignature> m_rootSignature;
		RefHandle<D3D12VertexShader> m_vertexShader;
		RefHandle<D3D12PixelShader> m_pixelShader;
		// Reserved
		// RefHandle<D3D12VertexShader> m_domainShader;
		// RefHandle<D3D12VertexShader> m_hullShader;
		RefHandle<D3D12GeometryShader> m_geometryShader;
		RefHandle<D3D12BlendState> m_blendState;
		RefHandle<D3D12RasterizerState> m_rasterizerState;
		RefHandle<D3D12DepthStencilState> m_depthStencilState;
		RefHandle<D3D12VertexLayout> m_vertexLayout;
		D3D12_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	};

	class D3D12ComputePipelineState final : public ComputePipelineState
	{
	public:
		ID3D12PipelineState* Resource() const;
		D3D12RootSignature* GetRootSignature() const;

		D3D12ComputePipelineState( const ComputePipelineStateInitializer& initializer, const BinaryChunk* cachedPSO );
		D3D12ComputePipelineState( const D3D12ComputePipelineState& ) = delete;
		D3D12ComputePipelineState( D3D12ComputePipelineState&& ) = delete;
		D3D12ComputePipelineState& operator=( const D3D12ComputePipelineState& ) = delete;
		D3D12ComputePipelineState& operator=( D3D12ComputePipelineState&& ) = delete;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		RefHandle<D3D12RootSignature> m_rootSignature;
		RefHandle<D3D12ComputeShader> m_computeShader;

		ID3D12PipelineState* m_pipelineState = nullptr;
		D3D12_COMPUTE_PIPELINE_STATE_DESC m_desc = {};
	};
}
