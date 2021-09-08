#pragma once

#include "ICommandList.h"

namespace aga
{
	class D3D11ImmediateCommandList : public IImmediateCommandList
	{
	public:
		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, UINT indexOffset ) override;
		virtual void BindPipelineState( PipelineState* pipelineState ) override;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) override;
		virtual void Draw( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void DrawInstancing( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
	};
}
