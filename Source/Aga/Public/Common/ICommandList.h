#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace aga
{
	class Buffer;
	class PipelineState;
	class ShaderBindings;
	class Texture;

	class ICommandList
	{
	public:
		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets ) = 0;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) = 0;
		
		virtual void BindPipelineState( PipelineState* pipelineState ) = 0;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) = 0;
		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) = 0;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) = 0;

		virtual void SetViewports( uint32 count, const CubeArea<float>* areas ) = 0;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) = 0;

		virtual void BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil ) = 0;

		virtual ~ICommandList( ) = default;
	};

	class IDeferredCommandList : public ICommandList
	{
	public:
		virtual void Finish( ) = 0;
	};

	class IImmediateCommandList : public ICommandList
	{
	public:
		virtual void Execute( IDeferredCommandList& commandList ) = 0;
	};
}
