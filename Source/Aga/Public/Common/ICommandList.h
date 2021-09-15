#pragma once

#include "SizedTypes.h"

namespace aga
{
	class Buffer;
	class PipelineState;
	class ShaderBindings;

	class ICommandList
	{
	public:
		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets ) = 0;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) = 0;
		
		virtual void BindPipelineState( PipelineState* pipelineState ) = 0;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) = 0;
		virtual void Draw( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation ) = 0;
		virtual void DrawInstancing( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) = 0;
	};

	class IImmediateCommandList : public ICommandList
	{

	};
}
