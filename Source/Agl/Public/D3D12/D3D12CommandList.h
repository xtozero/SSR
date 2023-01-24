#pragma once

#include "CommandLists.h"
#include "GlobalConstantBuffers.h"

#include <wrl/client.h>
#include <d3d12.h>

namespace agl
{
	class D3D12CommandList : public IImmediateCommandList
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

		virtual void WaitUntilFlush() override;

		virtual void Execute( IDeferredCommandList& commandList ) override;

	private:
	};

	class D3D12DeferredCommandList : public IDeferredCommandList
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

		virtual void Finish() override;

		D3D12DeferredCommandList();
		virtual ~D3D12DeferredCommandList() override;

	private:
	};

	class D3D12GraphicsCommandList : public IGraphicsCommandList
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

		void Initialize();
		void Reset();
		void Close();

		ID3D12CommandList* Resource();

	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_commandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		
		GlobalAsyncConstantBuffers m_globalConstantBuffers;
	};

	class D3D12GraphicsCommandLists : public GraphicsCommandListsBase
	{
	public:
		virtual Owner<IGraphicsCommandList*> CreateCommandList() override;

		virtual void Prepare() override;
		virtual void Commit() override;
	};
}
