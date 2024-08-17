#pragma once

#include "D3D12BarrierBatcher.h"
#include "D3D12GlobalDescriptorHeap.h"
#include "D3D12PipelineCache.h"
#include "DirectXTex.h"
#include "GlobalConstantBuffers.h"
#include "ICommandList.h"
#include "Memory/FixedBlockMemoryPool.h"
#include "Memory/InlineMemoryAllocator.h"

#include <d3d12.h>
#include <wrl/client.h>

namespace agl
{
	struct D3D12CommandListResource final
	{
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_commandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		D3D12CommandListResource* m_next = nullptr;
	};

	class D3D12CommnadListResourcePool final
	{
	public:
		void Prepare();
		D3D12CommandListResource& ObtainCommandList();

		D3D12CommnadListResourcePool( D3D12_COMMAND_LIST_TYPE type );
		~D3D12CommnadListResourcePool();

	private:
		D3D12_COMMAND_LIST_TYPE m_type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		FixedBlockMemoryPool<D3D12CommandListResource> m_allocator;
		D3D12CommandListResource* m_freeList = nullptr;
		D3D12CommandListResource* m_runningList = nullptr;
	};

	class D3D12CommandListImpl final
	{
	public:
		void Initialize();

		void Prepare();

		void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets );
		void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset );
		void BindPipelineState( GraphicsPipelineState* pipelineState );
		void BindPipelineState( ComputePipelineState* pipelineState );
		void BindShaderResources( ShaderBindings& shaderBindings );
		void SetShaderValue( const ShaderParameter& parameter, const void* value );
		void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation );
		void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation );
		void Dispatch( uint32 x, uint32 y, uint32 z = 1 );

		void SetViewports( uint32 count, const CubeArea<float>* area );
		void SetScissorRects( uint32 count, const RectangleArea<int32>* area );

		void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil );

		void ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] );
		void ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor );

		void CopyResource( Texture* dest, Texture* src, bool bDirect );
		void CopyResource( Buffer* dest, Buffer* src, uint32 numByte, bool bDirect );

		void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea, uint32 subresource );
		void UpdateSubresource( agl::Buffer* dest, const void* src, uint32 destOffset, uint32 numByte );

		void AddTransition( const ResourceTransition& transition );
		void AddUavBarrier( const UavBarrier& uavBarrier );

		void ResourceBarrier( uint32 numBarriers, D3D12_RESOURCE_BARRIER* barriers );

		bool CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult );

		void BeginQuery( void* rawQuery );
		void EndQuery( void* rawQuery );
		void ResolveQueryData( void* queryHeap, D3D12_QUERY_TYPE type, uint32 offset, uint32 numQueries );

		void Signal( ID3D12Fence* fence, uint64 fenceValue );

		void Close();

		void OnCommited();

		ID3D12CommandList* Resource() const;

	private:
		ID3D12GraphicsCommandList6& CommandList();

		D3D12CommandListResource m_cmdListResource;

		GlobalAsyncConstantBuffers m_globalConstantBuffers;
		D3D12GlobalDescriptorHeap m_globalDescriptorHeap;

		D3D12PipelineCache m_stateCache;

		D3D12BarrierBatcher m_barrierBatcher;
		std::vector<std::pair<ID3D12Fence*, int64>> m_fenceBatch;
	};

	class ID3D12CommandListEX : public ICommandList
	{
	public:
		virtual void ResolveQueryData( void* queryHeap, D3D12_QUERY_TYPE type, uint32 offset, uint32 numQueries ) = 0;

		virtual void Signal( ID3D12Fence* fence, uint64 fenceValue ) = 0;
	};

	class D3D12CommandList final : public ID3D12CommandListEX
	{
	public:
		virtual void Prepare() override;

		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( GraphicsPipelineState* pipelineState ) override;
		virtual void BindPipelineState( ComputePipelineState* pipelineState ) override;
		virtual void BindShaderResources( ShaderBindings& shaderBindings ) override;
		virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) override;
		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) override;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* area ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* area ) override;

		virtual void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil ) override;

		virtual void ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] ) override;
		virtual void ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor ) override;

		virtual void CopyResource( Texture* dest, Texture* src, bool bDirect = false ) override;
		virtual void CopyResource( Buffer* dest, Buffer* src, uint32 numByte, bool bDirect = false ) override;

		virtual void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea = nullptr, uint32 subresource = 0 ) override;
		virtual void UpdateSubresource( agl::Buffer* dest, const void* src, uint32 destOffset = 0, uint32 numByte = 0 ) override;

		virtual void AddTransition( const ResourceTransition& transition ) override;
		virtual void AddUavBarrier( const UavBarrier& uavBarrier ) override;

		virtual bool CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult ) override;

		virtual void BeginQuery( void* rawQuery ) override;
		virtual void EndQuery( void* rawQuery ) override;

		virtual void WaitUntilFlush() override;

		virtual void Commit() override;

		virtual void ResolveQueryData( void* queryHeap, D3D12_QUERY_TYPE type, uint32 offset, uint32 numQueries ) override;

		virtual void Signal( ID3D12Fence* fence, uint64 fenceValue ) override;

		void Initialize();

		void OnCommitted();

		ID3D12CommandListEX& GetParallelCommandList();

		D3D12CommandList() = default;
		D3D12CommandList( const D3D12CommandList& ) = delete;
		D3D12CommandList& operator=( const D3D12CommandList& ) = delete;
		D3D12CommandList( D3D12CommandList&& other ) noexcept;
		D3D12CommandList& operator=( D3D12CommandList&& other ) noexcept;
		virtual ~D3D12CommandList();

	private:
		D3D12CommandListImpl m_imple;

		uint32 m_numUsedParallelCommandList = 0;
		std::vector<ID3D12CommandListEX*, InlineAllocator<ID3D12CommandListEX*, 1>> m_parallelCommandLists;
	};

	class D3D12ParallelCommandList final : public ID3D12CommandListEX
	{
	public:
		virtual void Prepare() override;

		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( GraphicsPipelineState* pipelineState ) override;
		virtual void BindPipelineState( ComputePipelineState* pipelineState ) override;
		virtual void BindShaderResources( ShaderBindings& shaderBindings ) override;
		virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) override;
		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) override;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* areas ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) override;

		virtual void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil ) override;

		virtual void ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] ) override;
		virtual void ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor ) override;

		virtual void CopyResource( Texture* dest, Texture* src, bool bDirect = false ) override;
		virtual void CopyResource( Buffer* dest, Buffer* src, uint32 numByte, bool bDirect = false ) override;

		virtual void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea = nullptr, uint32 subresource = 0 ) override;
		virtual void UpdateSubresource( agl::Buffer* dest, const void* src, uint32 destOffset = 0, uint32 numByte = 0 ) override;

		virtual void AddTransition( const ResourceTransition& transition ) override;
		virtual void AddUavBarrier( const UavBarrier& uavBarrier ) override;

		virtual bool CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult ) override;

		virtual void BeginQuery( void* rawQuery ) override;
		virtual void EndQuery( void* rawQuery ) override;

		virtual void ResolveQueryData( void* queryHeap, D3D12_QUERY_TYPE type, uint32 offset, uint32 numQueries ) override;

		virtual void Signal( ID3D12Fence* fence, uint64 fenceValue ) override;

		virtual void WaitUntilFlush() override {}

		virtual void Commit() override {}

		void Close();

		void Initialize();

		void OnCommitted();

		ID3D12CommandList* Resource() const;

	private:
		friend D3D12CommandList;
		D3D12CommandListImpl m_imple;
	};
}
