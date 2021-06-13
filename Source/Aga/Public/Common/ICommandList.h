#pragma once

namespace aga
{
	class Buffer;
	class PipelineState;
	class ShaderBindings;

	class ICommandList
	{
	public:
		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pOffsets ) = 0;
		virtual void BindIndexBuffer( Buffer* indexBuffer, UINT indexOffset ) = 0;
		
		virtual void BindPipelineState( PipelineState* pipelineState ) = 0;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) = 0;
		virtual void Draw( UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation ) = 0;
	};

	class IImmediateCommandList : public ICommandList
	{

	};
}
