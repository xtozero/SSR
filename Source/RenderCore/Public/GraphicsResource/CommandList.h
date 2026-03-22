#pragma once

#include "ICommandList.h"

#include <memory>

namespace agl
{
	class Query;
}

namespace rendercore
{
	class CopyCommandList
	{
	public:
		void CopyResource( agl::Texture* dest, agl::Texture* src, bool bAsync = true ) const;
		void CopyResource( agl::Buffer* dest, agl::Buffer* src, bool bAsync = true, uint32 numByte = 0 ) const;

		void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, bool bAsync = true, const CubeArea<uint32>* destArea = nullptr, uint32 subresource = 0 ) const;
		void UpdateSubresource( agl::Buffer* dest, const void* src, bool bAsync = true, uint32 destOffset = 0, uint32 numByte = 0 ) const;

		void Commit() const;

		explicit CopyCommandList( agl::ICopyCommandList& impl )
			: m_impl( impl ) {}

	protected:
		agl::ICommandListBase& m_impl;

	private:
		agl::ICopyCommandList& GetImpl() const;
	};

	class ComputeCommandList : public CopyCommandList
	{
	public:
		void BindPipelineState( agl::ComputePipelineState* pipelineState ) const;
		void BindShaderResources( agl::ShaderBindings& shaderBindings ) const;
		void SetShaderValue( const agl::ShaderParameter& parameter, const void* value ) const;

		void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) const;
		void DispatchRays( agl::RaytracingPipelineState* pipelineState, agl::ShaderBindings& shaderBindings, uint32 width, uint32 height, uint32 depth = 1 ) const;
		void ExecuteIndirect( agl::IndirectCommandType type, agl::Buffer* argument, uint64 argumentOffset = 0 );

		void AddTransition( const agl::ResourceTransition& transition ) const;
		void AddUavBarrier( const agl::UavBarrier& uavBarrier ) const;

		void BeginQuery( agl::Query* rawQuery ) const;
		void EndQuery( agl::Query* rawQuery ) const;

		void BeginEvent( const char* eventName ) const;
		void EndEvent() const;

		explicit ComputeCommandList( agl::IComputeCommandList& impl )
			: CopyCommandList( impl ) {}

	private:
		agl::IComputeCommandList& GetImpl() const;
	};

	class CommandList : public ComputeCommandList
	{
	public:
		using ComputeCommandList::BindPipelineState;

		void BindVertexBuffer( agl::Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets ) const;
		void BindIndexBuffer( agl::Buffer* indexBuffer, uint32 indexOffset ) const;
		void BindPipelineState( agl::GraphicsPipelineState* pipelineState ) const;
		void BindRenderTargets( agl::RenderTargetView** pRenderTargets, uint32 renderTargetCount, agl::DepthStencilView* depthStencil ) const;

		void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) const;
		void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) const;
		void DispatchMesh( uint32 x, uint32 y, uint32 z = 1 ) const;

		void SetViewports( uint32 count, const CubeArea<float>* areas ) const;
		void SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) const;

		void ClearRenderTarget( agl::RenderTargetView* renderTarget ) const;
		void ClearDepthStencil( agl::DepthStencilView* depthStencil ) const;

		RENDERCORE_DLL bool CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult ) const;

		explicit CommandList( agl::ICommandList& impl )
			: ComputeCommandList( impl ) {}

	private:
		agl::ICommandList& GetImpl() const;
	};

	RENDERCORE_DLL CommandList GetCommandList();
	RENDERCORE_DLL ComputeCommandList GetComputeCommandList();
}
