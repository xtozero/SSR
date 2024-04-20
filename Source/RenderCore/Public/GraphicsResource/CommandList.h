#pragma once

#include "ICommandList.h"

#include <memory>

namespace agl
{
	class Query;
}

namespace rendercore
{
	class CommandList final
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

		void SetViewports( uint32 count, const CubeArea<float>* areas );
		void SetScissorRects( uint32 count, const RectangleArea<int32>* areas );

		void BindRenderTargets( agl::RenderTargetView** pRenderTargets, uint32 renderTargetCount, agl::DepthStencilView* depthStencil );

		void ClearRenderTarget( agl::RenderTargetView* renderTarget, const float( &clearColor )[4] );
		void ClearDepthStencil( agl::DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor );

		void CopyResource( agl::Texture* dest, agl::Texture* src, bool bDirect = false );
		void CopyResource( agl::Buffer* dest, agl::Buffer* src, uint32 numByte = 0, bool bDirect = false );

		void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea = nullptr, uint32 subresource = 0 );
		void UpdateSubresource( agl::Buffer* dest, const void* src, uint32 destOffset = 0, uint32 numByte = 0 );

		void AddTransition( const agl::ResourceTransition& transition );
		void AddUavBarrier( const agl::UavBarrier& uavBarrier );

		RENDERCORE_DLL bool CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult );

		void BeginQuery( agl::Query* rawQuery );
		void EndQuery( agl::Query* rawQuery );

		void WaitUntilFlush();

		void Commit();

		explicit CommandList( agl::ICommandList& imple )
			: m_imple( imple ) {}

	private:
		agl::ICommandList& m_imple;
	};

	RENDERCORE_DLL CommandList GetCommandList();
}
