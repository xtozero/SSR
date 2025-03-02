#pragma once

#include "ICommandList.h"

#include <memory>

namespace agl
{
	class Query;
}

namespace rendercore
{
	class ComputeCommandList
	{
	public:
		void BindPipelineState( agl::ComputePipelineState* pipelineState );
		void BindShaderResources( agl::ShaderBindings& shaderBindings );
		void SetShaderValue( const agl::ShaderParameter& parameter, const void* value );

		void Dispatch( uint32 x, uint32 y, uint32 z = 1 );

		void AddTransition( const agl::ResourceTransition& transition );
		void AddUavBarrier( const agl::UavBarrier& uavBarrier );

		void BeginQuery( agl::Query* rawQuery );
		void EndQuery( agl::Query* rawQuery );

		void BeginEvent( const char* eventName );
		void EndEvent();

		void Commit();

		explicit ComputeCommandList( agl::ICommandList& imple )
			: m_imple( imple ) {}

	protected:
		agl::ICommandList& m_imple;
	};

	class ResourceCommandList : public ComputeCommandList
	{
	public:
		void CopyResource( agl::Texture* dest, agl::Texture* src, bool bAsync = true );
		void CopyResource( agl::Buffer* dest, agl::Buffer* src, bool bAsync = true, uint32 numByte = 0 );

		void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, bool bAsync = true, const CubeArea<uint32>* destArea = nullptr, uint32 subresource = 0 );
		void UpdateSubresource( agl::Buffer* dest, const void* src, bool bAsync = true, uint32 destOffset = 0, uint32 numByte = 0 );

		RENDERCORE_DLL bool CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult );

		void ClearRenderTarget( agl::RenderTargetView* renderTarget );
		void ClearDepthStencil( agl::DepthStencilView* depthStencil );

		explicit ResourceCommandList( agl::ICommandList& imple )
			: ComputeCommandList( imple ) {}
	};

	class CommandList : public ResourceCommandList
	{
	public:
		using ComputeCommandList::BindPipelineState;

		void BindVertexBuffer( agl::Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets );
		void BindIndexBuffer( agl::Buffer* indexBuffer, uint32 indexOffset );

		void BindPipelineState( agl::GraphicsPipelineState* pipelineState );
		void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation );
		void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation );
		void DispatchMesh( uint32 x, uint32 y, uint32 z = 1 );

		void SetViewports( uint32 count, const CubeArea<float>* areas );
		void SetScissorRects( uint32 count, const RectangleArea<int32>* areas );

		void BindRenderTargets( agl::RenderTargetView** pRenderTargets, uint32 renderTargetCount, agl::DepthStencilView* depthStencil );

		explicit CommandList( agl::ICommandList& imple )
			: ResourceCommandList( imple ) {}
	};

	RENDERCORE_DLL CommandList GetCommandList();
}
