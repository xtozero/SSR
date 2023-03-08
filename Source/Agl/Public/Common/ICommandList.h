#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace agl
{
	class Buffer;
	class ComputePipelineState;
	class ComputeShader;
	class DepthStencilView;
	class GeometryShader;
	class GraphicsPipelineState;
	class PixelShader;
	class RenderTargetView;
	class SamplerState;
	class ShaderBindings;
	class ShaderParameter;
	class ShaderResourceView;
	class Texture;
	class UnorderedAccessView;
	class VertexShader;

	class ICommandListBase
	{
	public:
		virtual void Prepare() = 0;

		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets ) = 0;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) = 0;

		virtual void BindPipelineState( GraphicsPipelineState* pipelineState ) = 0;
		virtual void BindPipelineState( ComputePipelineState* pipelineState ) = 0;
		virtual void BindShaderResources( ShaderBindings& shaderBindings ) = 0;
		virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) = 0;

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

		virtual void Transition( uint32 numTransitions, const ResourceTransition* transitions ) = 0;

		virtual ~ICommandListBase() = default;
	};

	class ICommandList : public ICommandListBase
	{
	public:
		virtual void WaitUntilFlush() = 0;

		virtual void Commit() = 0;
	};

	class IParallelCommandList : public ICommandListBase
	{
	};
}
