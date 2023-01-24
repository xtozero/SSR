#pragma once

#include "ICommandList.h"

#include <memory>

namespace rendercore
{
	class ImmediateCommandList
	{
	public:
		void BindVertexBuffer( agl::Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets );
		void BindIndexBuffer( agl::Buffer* indexBuffer, uint32 indexOffset );

		void BindPipelineState( agl::GraphicsPipelineState* pipelineState );
		void BindPipelineState( agl::ComputePipelineState* pipelineState );
		void BindShaderResources( agl::ShaderBindings& shaderBindings );
		void SetShaderValue( const agl::ShaderParameter& parameter, const void* value );
		void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation );
		void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation );
		void Dispatch( uint32 x, uint32 y, uint32 z = 1 );

		void SetViewports( uint32 count, const agl::CubeArea<float>* areas );
		void SetScissorRects( uint32 count, const agl::RectangleArea<int32>* areas );

		void BindRenderTargets( agl::RenderTargetView** pRenderTargets, uint32 renderTargetCount, agl::DepthStencilView* depthStencil );

		void ClearRenderTarget( agl::RenderTargetView* renderTarget, const float( &clearColor )[4] );
		void ClearDepthStencil( agl::DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor );

		void CopyResource( agl::Texture* dest, agl::Texture* src );
		void CopyResource( agl::Buffer* dest, agl::Buffer* src );

		void WaitUntilFlush();

		void Execute( agl::IDeferredCommandList& commandList );

		explicit ImmediateCommandList( agl::IImmediateCommandList& imple )
			: m_imple( imple ) {}

	private:
		agl::IImmediateCommandList& m_imple;
	};

	ImmediateCommandList GetImmediateCommandList();
}
