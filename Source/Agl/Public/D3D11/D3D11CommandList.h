#pragma once

#include "CommandLists.h"
#include "GlobalConstantBuffers.h"
#include "D3D11StateCache.h"

struct ID3D11DeviceContext;
struct ID3D11CommandList;

namespace agl
{
	class D3D11ImmediateCommandList : public IImmediateCommandList
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

		D3D11ImmediateCommandList();

	private:
		D3D11PipelineCache m_stateCache;
		GlobalSyncConstantBuffers m_globalConstantBuffers;
	};

	class D3D11DeferredCommandList : public IDeferredCommandList
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

		void RequestExecute();

		D3D11DeferredCommandList();
		virtual ~D3D11DeferredCommandList() override;

	private:
		ID3D11DeviceContext* m_pContext = nullptr;
		ID3D11CommandList* m_pCommandList = nullptr;
		D3D11PipelineCache m_stateCache;
		GlobalAsyncConstantBuffers m_globalConstantBuffers;
	};

	class D3D11GraphicsCommandLists : public GraphicsCommandListsBase
	{
	public:
		virtual Owner<IGraphicsCommandList*> CreateCommandList() override;

		virtual void Prepare() override;
		virtual void Commit() override;
	};
}
