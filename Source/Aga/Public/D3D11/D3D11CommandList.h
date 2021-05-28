#pragma once

#include "ICommandList.h"

namespace aga
{
	class D3D11ImmediateCommandList : public IImmediateCommandList
	{
	public:
		virtual void BindVertexBuffer( aga::Buffer* const* vertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pOffsets ) override;
		virtual void BindIndexBuffer( aga::Buffer* indexBuffer, UINT indexOffset ) override;
		virtual void BindPipelineState( aga::PipelineState* pipelineState ) override;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) override;
		virtual void Draw( UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation ) override;
	};
}
