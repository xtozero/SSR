#pragma once

#include "ICommandList.h"

namespace agl
{
	class D3D12CommandList : public IImmediateCommandList
	{
	public:
		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( PipelineState* pipelineState ) override;
		virtual void BindShader( VertexShader* vs ) override;
		virtual void BindShader( GeometryShader* gs ) override;
		virtual void BindShader( PixelShader* ps ) override;
		virtual void BindShader( ComputeShader* cs ) override;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) override;
		virtual void BindConstantBuffer( ShaderType shader, uint32 slot, Buffer* buffer ) override;
		virtual void BindSRV( ShaderType shader, uint32 slot, ShaderResourceView* srv ) override;
		virtual void BindUAV( ShaderType shader, uint32 slot, UnorderedAccessView* uav ) override;
		virtual void BindSampler( ShaderType shader, uint32 slot, SamplerState* sampler ) override;
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
		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( PipelineState* pipelineState ) override;
		virtual void BindShader( VertexShader* vs ) override;
		virtual void BindShader( GeometryShader* gs ) override;
		virtual void BindShader( PixelShader* ps ) override;
		virtual void BindShader( ComputeShader* cs ) override;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) override;
		virtual void BindConstantBuffer( ShaderType shader, uint32 slot, Buffer* buffer ) override;
		virtual void BindSRV( ShaderType shader, uint32 slot, ShaderResourceView* srv ) override;
		virtual void BindUAV( ShaderType shader, uint32 slot, UnorderedAccessView* uav ) override;
		virtual void BindSampler( ShaderType shader, uint32 slot, SamplerState* sampler ) override;
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
		virtual ~D3D12DeferredCommandList();

	private:
	};
}
