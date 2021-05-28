#pragma once

class ShaderBindings;

namespace aga
{
	class Buffer;
	class PipelineState;

	class ICommandList
	{
	public:
		virtual void BindVertexBuffer( aga::Buffer* const* vertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pOffsets ) = 0;
		virtual void BindIndexBuffer( aga::Buffer* indexBuffer, UINT indexOffset ) = 0;
		
		virtual void BindPipelineState( aga::PipelineState* pipelineState ) = 0;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) = 0;
		virtual void Draw( UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation ) = 0;
	};

	class IImmediateCommandList : public ICommandList
	{

	};
}
