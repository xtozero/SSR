#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace aga
{
	class Buffer;
	class ComputeShader;
	class DepthStencilView;
	class GeometryShader;
	class PipelineState;
	class PixelShader;
	class RenderTargetView;
	class SamplerState;
	class ShaderBindings;
	class ShaderResourceView;
	class Texture;
	class UnorderedAccessView;
	class VertexShader;

	class ICommandList
	{
	public:
		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets ) = 0;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) = 0;

		virtual void BindPipelineState( PipelineState* pipelineState ) = 0;
		virtual void BindShader( VertexShader* vs ) = 0;
		virtual void BindShader( GeometryShader* gs ) = 0;
		virtual void BindShader( PixelShader* ps ) = 0;
		virtual void BindShader( ComputeShader* cs ) = 0;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) = 0;
		virtual void BindConstantBuffer( SHADER_TYPE shader, uint32 slot, Buffer* buffer ) = 0;
		virtual void BindSRV( SHADER_TYPE shader, uint32 slot, ShaderResourceView* srv ) = 0;
		virtual void BindUAV( SHADER_TYPE shader, uint32 slot, UnorderedAccessView* uav ) = 0;
		virtual void BindSampler( SHADER_TYPE shader, uint32 slot, SamplerState* sampler ) = 0;
		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) = 0;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) = 0;
		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) = 0;

		virtual void SetViewports( uint32 count, const CubeArea<float>* areas ) = 0;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) = 0;

		virtual void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil ) = 0;

		virtual void ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] ) = 0;
		virtual void ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor ) = 0;

		virtual void CopyResource( Texture* dest, Texture* src ) = 0;
		virtual void CopyResource( Buffer* dest, Buffer* src ) = 0;

		virtual ~ICommandList() = default;
	};

	class IDeferredCommandList : public ICommandList
	{
	public:
		virtual void Finish() = 0;
	};

	class IImmediateCommandList : public ICommandList
	{
	public:
		virtual void WaitUntilFlush() = 0;

		virtual void Execute( IDeferredCommandList& commandList ) = 0;
	};
}
