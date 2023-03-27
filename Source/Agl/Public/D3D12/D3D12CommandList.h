#pragma once

#include "D3D12GlobalDescriptorHeap.h"
#include "D3D12PipelineCache.h"
#include "GlobalConstantBuffers.h"
#include "ICommandList.h"
#include "Memory/InlineMemoryAllocator.h"

#include <wrl/client.h>
#include <d3d12.h>

namespace agl
{
	class D3D12CommandListImpl
	{
	public:
		void Initialize();

		void Prepare();

		void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets );
		void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset );
		void BindPipelineState( GraphicsPipelineState* pipelineState );
		void BindPipelineState( ComputePipelineState* pipelineState );
		void BindShaderResources( ShaderBindings& shaderBindings );
		void SetShaderValue( const ShaderParameter& parameter, const void* value );
		void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation );
		void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation );
		void Dispatch( uint32 x, uint32 y, uint32 z = 1 );

		void SetViewports( uint32 count, const CubeArea<float>* area );
		void SetScissorRects( uint32 count, const RectangleArea<int32>* area );

		void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil );

		void ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] );
		void ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor );

		void CopyResource( Texture* dest, Texture* src );
		void CopyResource( Buffer* dest, Buffer* src );

		void Transition( uint32 numTransitions, const ResourceTransition* transitions );

		void Close();

		ID3D12CommandList* Resource() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_commandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

		GlobalAsyncConstantBuffers m_globalConstantBuffers;
		D3D12GlobalDescriptorHeap m_globalDescriptorHeap;

		D3D12PipelineCache m_stateCache;
	};

	class D3D12CommandList : public ICommandList
	{
	public:
		virtual void Prepare() override;

		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( GraphicsPipelineState* pipelineState ) override;
		virtual void BindPipelineState( ComputePipelineState* pipelineState ) override;
		virtual void BindShaderResources( ShaderBindings& shaderBindings ) override;
		virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) override;
		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) override;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* area ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* area ) override;

		virtual void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil ) override;

		virtual void ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] ) override;
		virtual void ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor ) override;

		virtual void CopyResource( Texture* dest, Texture* src ) override;
		virtual void CopyResource( Buffer* dest, Buffer* src ) override;

		virtual void Transition( uint32 numTransitions, const ResourceTransition* transitions ) override;

		virtual void WaitUntilFlush() override;

		virtual void Commit() override;

		void Initialize();

		IParallelCommandList& GetParallelCommandList();

		D3D12CommandList() = default;
		D3D12CommandList( const D3D12CommandList& ) = delete;
		D3D12CommandList& operator=( const D3D12CommandList& ) = delete;
		D3D12CommandList( D3D12CommandList&& other ) noexcept;
		D3D12CommandList& operator=( D3D12CommandList&& other ) noexcept;
		virtual ~D3D12CommandList();

	private:
		D3D12CommandListImpl m_imple;

		uint32 m_numUsedParallelCommandList = 0;
		std::vector<IParallelCommandList*, InlineAllocator<IParallelCommandList*, 1>> m_parallelCommandLists;
	};

	class D3D12ParallelCommandList : public IParallelCommandList
	{
	public:
		virtual void Prepare() override;

		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( GraphicsPipelineState* pipelineState ) override;
		virtual void BindPipelineState( ComputePipelineState* pipelineState ) override;
		virtual void BindShaderResources( ShaderBindings& shaderBindings ) override;
		virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) override;
		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) override;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* areas ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) override;

		virtual void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil ) override;

		virtual void ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] ) override;
		virtual void ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor ) override;

		virtual void CopyResource( Texture* dest, Texture* src ) override;
		virtual void CopyResource( Buffer* dest, Buffer* src ) override;

		virtual void Transition( uint32 numTransitions, const ResourceTransition* transitions ) override;

		void Close();

		void Initialize();

		ID3D12CommandList* Resource() const;

	private:
		friend D3D12CommandList;
		D3D12CommandListImpl m_imple;
	};
}
