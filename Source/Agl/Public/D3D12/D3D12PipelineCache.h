#pragma once

#include "AreaTypes.h"
#include "D3D12ResourceAllocator.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "StackMemoryAllocator.h"
#include "TransientAllocator.h"

#include <array>
#include <d3d12.h>
#include <set>

namespace agl
{
	class Buffer;
	class ComputePipelineState;
	class D3D12GlobalDescriptorHeap;
	class DepthStencilView;
	class GlobalConstantBuffers;
	class GraphicsPipelineState;
	class RenderTargetView;
	class ShaderBindings;
	class Texture;

	class D3D12PipelineCache final
	{
	public:
		void Prepare();
		void ReleaseRenderResources();

		void BindVertexBuffer( ID3D12GraphicsCommandList6& commandList, Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets );
		void BindIndexBuffer( ID3D12GraphicsCommandList6& commandList, Buffer* indexBuffer, uint32 indexOffset );

		void BindPipelineState( ID3D12GraphicsCommandList6& commandList, GraphicsPipelineState* pipelineState );
		void BindPipelineState( ID3D12GraphicsCommandList6& commandList, ComputePipelineState* pipelineState );

		void BindBindlessResources( ID3D12GraphicsCommandList6& commandList, GlobalConstantBuffers& globalConstantBuffers, ShaderBindings& shaderBindings );
		void BindShaderResources( ID3D12GraphicsCommandList6& commandList, D3D12GlobalDescriptorHeap& descriptorHeap, GlobalConstantBuffers& globalConstantBuffers, ShaderBindings& shaderBindings );

		void SetViewports( ID3D12GraphicsCommandList6& commandList, uint32 count, const CubeArea<float>* area );
		void SetScissorRects( ID3D12GraphicsCommandList6& commandList, uint32 count, const RectangleArea<int32>* area );
		void BindRenderTargets( ID3D12GraphicsCommandList6& commandList, RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil );

		D3D12PipelineCache();
		~D3D12PipelineCache() = default;
		D3D12PipelineCache( const D3D12PipelineCache& ) = delete;
		D3D12PipelineCache& operator=( const D3D12PipelineCache& ) = delete;
		D3D12PipelineCache( D3D12PipelineCache&& ) = default;
		D3D12PipelineCache& operator=( D3D12PipelineCache&& ) = default;

	private:
		void RegisterRenderResource( GraphicsApiResource* resource );
		void RegisterRenderResource( IUnknown* resource );

		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferViews[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
		uint32 m_numVertexBufferViews = 0;

		D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};

		ID3D12PipelineState* m_pipelineState = nullptr;

		D3D12_VIEWPORT m_viewports[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
		uint32 m_numViewports = 0;

		D3D12_RECT m_siccorRects[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
		uint32 m_numSiccorRects = 0;

		RenderTargetView* m_rtvs[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
		DepthStencilView* m_dsv = nullptr;

		TypedStackAllocator<const ID3D12Resource*> m_allocatedIdentifierAllocator;
		std::set<const ID3D12Resource*, std::less<const ID3D12Resource*>, TypedStackAllocator<const ID3D12Resource*>> m_allocatedIdentifiers;

		TypedStackAllocator<AllocatedResourceInfo> m_allocatedInfoAllocator;
		std::vector<AllocatedResourceInfo, TypedStackAllocator<AllocatedResourceInfo>> m_allocatedInfos;

		std::vector<Microsoft::WRL::ComPtr<IUnknown>, TypedStackAllocator<Microsoft::WRL::ComPtr<IUnknown>>> m_residentResource;
	};
}
