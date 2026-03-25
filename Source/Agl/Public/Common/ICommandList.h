#pragma once

#include "AreaTypes.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace DirectX
{
	class ScratchImage;
}

namespace agl
{
	class Buffer;
	class ComputePipelineState;
	class ComputeShader;
	class DepthStencilView;
	class GeometryShader;
	class GraphicsPipelineState;
	class PixelShader;
	class RaytracingPipelineState;
	class RenderTargetView;
	class SamplerState;
	class ShaderBindings;
	class ShaderParameter;
	class ShaderResourceView;
	class Texture;
	class UnorderedAccessView;
	class VertexShader;

	enum class IndirectCommandType : uint8
	{
		Draw,
		DrawIndexed,
		Dispatch,
		DispatchMesh,
	};

	class ICommandListBase
	{
	public:
		virtual void Prepare() = 0;

		virtual void AddTransition( const ResourceTransition& transition ) = 0;
		virtual void AddUavBarrier( const UavBarrier& uavBarrier ) = 0;

		virtual void BeginQuery( void* rawQuery ) = 0;
		virtual void EndQuery( void* rawQuery ) = 0;

		virtual void BeginEvent( const char* eventName ) = 0;
		virtual void EndEvent() = 0;

		virtual void Commit() = 0;

		virtual ~ICommandListBase() = default;
	};

	class ICopyCommandList : public ICommandListBase
	{
	public:
		virtual void CopyResource( Texture* dest, Texture* src, bool bAsync = true ) = 0;
		virtual void CopyResource( Buffer* dest, Buffer* src, bool bAsync = true, uint32 numByte = 0 ) = 0;

		virtual void UpdateSubresource( Texture* dest, const void* src, uint32 srcRowSize, bool bAsync = true, const CubeArea<uint32>* destArea = nullptr, uint32 subresource = 0 ) = 0;
		virtual void UpdateSubresource( Buffer* dest, const void* src, bool bAsync = true, uint32 destOffset = 0, uint32 numByte = 0 ) = 0;
	};

	class IComputeCommandList : public ICopyCommandList
	{
	public:
		virtual void BindPipelineState( const ComputePipelineState* pipelineState ) = 0;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) = 0;
		virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) = 0;

		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) = 0;
		virtual void DispatchRays( const RaytracingPipelineState* pipelineState, const ShaderBindings& shaderBindings, uint32 width, uint32 height, uint32 depth ) = 0;

		virtual void ExecuteIndirect( IndirectCommandType type, Buffer* argument, uint64 argumentOffset = 0 ) = 0;
	};

	class ICommandList : public IComputeCommandList
	{
	public:
		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) = 0;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) = 0;
		virtual void DispatchMesh( uint32 x, uint32 y, uint32 z = 1 ) = 0;

		virtual void SetViewports( uint32 count, const CubeArea<float>* areas ) = 0;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) = 0;

		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets ) = 0;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) = 0;
		virtual void BindPipelineState( const GraphicsPipelineState* pipelineState ) = 0;
		virtual void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil ) = 0;

		virtual void ClearRenderTarget( RenderTargetView* renderTarget ) = 0;
		virtual void ClearDepthStencil( DepthStencilView* depthStencil ) = 0;

		virtual bool CaptureTexture( Texture* texture, DirectX::ScratchImage& outResult ) = 0;
	};
}
