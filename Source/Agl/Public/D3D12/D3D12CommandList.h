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
		D3D12CommandListResource& GetCommandList();

		D3D12CommnadListResourcePool( D3D12_COMMAND_LIST_TYPE type );
		~D3D12CommnadListResourcePool();

	private:
		D3D12_COMMAND_LIST_TYPE m_type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		FixedBlockMemoryPool<D3D12CommandListResource> m_allocator;
		D3D12CommandListResource* m_freeList = nullptr;
		D3D12CommandListResource* m_runningList = nullptr;
	};

	class D3D12BaseCommandListImpl
	{
	public:
		void Initialize();
		void Prepare();

		bool HasCommands() const;

		ID3D12CommandQueue& GetCommandQueue();

		void BeginQuery( void* rawQuery );
		void EndQuery( void* rawQuery );
		void ResolveQueryData( void* queryHeap, D3D12_QUERY_TYPE type, uint32 offset, uint32 numQueries );

		void BeginEvent( const char* eventName );
		void EndEvnet();

		ID3D12CommandList* Resource() const;

	protected:
		D3D12BaseCommandListImpl( D3D12_COMMAND_LIST_TYPE type ) : m_type( type ) {}

		ID3D12GraphicsCommandList6& CommandList();
		void InitializeCommandList();

		void OnCommandRecorded();

		D3D12CommandListResource m_cmdListResource;

		uint32 m_numCommands = 0;

	private:
		D3D12_COMMAND_LIST_TYPE m_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	};

	class D3D12CopyCommandListImpl : public D3D12BaseCommandListImpl
	{
	public:
		void CopyResource( Texture* dest, Texture* src, bool bAsync );
		void CopyResource( Buffer* dest, Buffer* src, bool bAsync, uint32 numByte );

		void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, bool bAsync, const CubeArea<uint32>* destArea, uint32 subresource );
		void UpdateSubresource( agl::Buffer* dest, const void* src, bool bAsync, uint32 destOffset, uint32 numByte );

		void AddTransition( const ResourceTransition& transition );
		void AddUavBarrier( const UavBarrier& uavBarrier );

		void ResourceBarrier( uint32 numBarriers, D3D12_RESOURCE_BARRIER* barriers );

		void Signal( ID3D12Fence* fence, uint64 fenceValue );

		void Close();

		void OnCommited();

		D3D12CopyCommandListImpl() : D3D12BaseCommandListImpl( D3D12_COMMAND_LIST_TYPE_COPY ) {}

	protected:
		D3D12CopyCommandListImpl( D3D12_COMMAND_LIST_TYPE type ) : D3D12BaseCommandListImpl( type ) {}

		D3D12BarrierBatcher m_barrierBatcher;
		std::vector<std::pair<ID3D12Fence*, int64>> m_fenceBatch;
	};

	class D3D12ComputeCommandListImpl : public D3D12CopyCommandListImpl
	{
	public:
		void Initialize();

		void Prepare();

		void BindComputePipelineState( ComputePipelineState* pipelineState );
		void BindShaderResources( ShaderBindings& shaderBindings );
		void SetShaderValue( const ShaderParameter& parameter, const void* value );

		void Dispatch( uint32 x, uint32 y, uint32 z = 1 );

		void OnCommited();

		D3D12ComputeCommandListImpl() : D3D12CopyCommandListImpl( D3D12_COMMAND_LIST_TYPE_COMPUTE ) {}

	protected:
		D3D12ComputeCommandListImpl( D3D12_COMMAND_LIST_TYPE type ) : D3D12CopyCommandListImpl( type ) {}

		D3D12PipelineCache m_stateCache;

		GlobalAsyncConstantBuffers m_globalConstantBuffers;
		D3D12GlobalDescriptorHeap m_globalDescriptorHeap;
	};

	class D3D12CommandListImpl : public D3D12ComputeCommandListImpl
	{
	public:
		void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets );
		void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset );
		void BindGraphicsPipelineState( GraphicsPipelineState* pipelineState );
		void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil );

		void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation );
		void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation );
		void DispatchMesh( uint32 x, uint32 y, uint32 z = 1 );

		void SetViewports( uint32 count, const CubeArea<float>* area );
		void SetScissorRects( uint32 count, const RectangleArea<int32>* area );

		void ClearRenderTarget( RenderTargetView* renderTarget );
		void ClearDepthStencil( DepthStencilView* depthStencil );

		bool CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult );

		D3D12CommandListImpl() : D3D12ComputeCommandListImpl( D3D12_COMMAND_LIST_TYPE_DIRECT ) {}

	protected:
		D3D12CommandListImpl( D3D12_COMMAND_LIST_TYPE type ) : D3D12ComputeCommandListImpl( type ) {}
	};

	class D3D12ComputeCommandList final : public IComputeCommandList
	{
	public:
		virtual void Prepare() override;

		virtual void AddTransition( const ResourceTransition& transition ) override;
		virtual void AddUavBarrier( const UavBarrier& uavBarrier ) override;

		virtual void BeginQuery( void* rawQuery ) override;
		virtual void EndQuery( void* rawQuery ) override;

		virtual void BeginEvent( const char* eventName ) override;
		virtual void EndEvent() override;

		virtual void Commit() override;

		virtual void CopyResource( Texture* dest, Texture* src, bool bAsync ) override;
		virtual void CopyResource( Buffer* dest, Buffer* src, bool bAsync, uint32 numByte ) override;

		virtual void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, bool bAsync,
			const CubeArea<uint32>* destArea, uint32 subresource ) override;
		virtual void UpdateSubresource( agl::Buffer* dest, const void* src, bool bAsync, uint32 destOffset,
			uint32 numByte ) override;

		virtual void BindPipelineState( ComputePipelineState* pipelineState ) override;
		virtual void BindShaderResources( ShaderBindings& shaderBindings ) override;
		virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) override;

		virtual void Dispatch( uint32 x, uint32 y, uint32 z ) override;

		void Initialize();

		void OnCommited();

		D3D12ComputeCommandList() = default;
		D3D12ComputeCommandList( const D3D12ComputeCommandList& ) = delete;
		D3D12ComputeCommandList& operator=( const D3D12ComputeCommandList& ) = delete;
		D3D12ComputeCommandList( D3D12ComputeCommandList&& other ) noexcept;
		D3D12ComputeCommandList& operator=( D3D12ComputeCommandList&& other ) noexcept;

	private:
		D3D12ComputeCommandListImpl m_impl;
		// If this variable is absent, an MSB6006 error occurs when declaring InlineAllocator.
		// The exact cause is still unknown.
		bool m_avoidMSB6006 = false;
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

		virtual void AddTransition( const ResourceTransition& transition ) override;
		virtual void AddUavBarrier( const UavBarrier& uavBarrier ) override;

		virtual void BeginQuery( void* rawQuery ) override;
		virtual void EndQuery( void* rawQuery ) override;

		virtual void BeginEvent( const char* eventName ) override;
		virtual void EndEvent() override;

		virtual void Commit() override;

		virtual void CopyResource( Texture* dest, Texture* src, bool bAsync = true ) override;
		virtual void CopyResource( Buffer* dest, Buffer* src, bool bAsync = true, uint32 numByte = 0 ) override;

		virtual void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, bool bAsync = true, const CubeArea<uint32>* destArea = nullptr, uint32 subresource = 0 ) override;
		virtual void UpdateSubresource( agl::Buffer* dest, const void* src, bool bAsync = true, uint32 destOffset = 0, uint32 numByte = 0 ) override;

		virtual void BindPipelineState( ComputePipelineState* pipelineState ) override;
		virtual void BindShaderResources( ShaderBindings& shaderBindings ) override;
		virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) override;

		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) override;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void DispatchMesh( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* area ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* area ) override;

		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( GraphicsPipelineState* pipelineState ) override;
		virtual void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil ) override;

		virtual void ClearRenderTarget( RenderTargetView* renderTarget ) override;
		virtual void ClearDepthStencil( DepthStencilView* depthStencil ) override;

		virtual bool CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult ) override;

		virtual void ResolveQueryData( void* queryHeap, D3D12_QUERY_TYPE type, uint32 offset, uint32 numQueries ) override;

		virtual void Signal( ID3D12Fence* fence, uint64 fenceValue ) override;

		void Initialize();

		void OnCommited();

		ID3D12CommandListEX& GetParallelCommandList();

		D3D12CommandList() = default;
		D3D12CommandList( const D3D12CommandList& ) = delete;
		D3D12CommandList& operator=( const D3D12CommandList& ) = delete;
		D3D12CommandList( D3D12CommandList&& other ) noexcept;
		D3D12CommandList& operator=( D3D12CommandList&& other ) noexcept;
		virtual ~D3D12CommandList() override;

	private:
		D3D12CommandListImpl m_impl;

		uint32 m_numUsedParallelCommandList = 0;
		std::vector<ID3D12CommandListEX*, InlineAllocator<ID3D12CommandListEX*, 1>> m_parallelCommandLists;
	};

	class D3D12ParallelCommandList final : public ID3D12CommandListEX
	{
	public:
		virtual void Prepare() override;

		virtual void AddTransition( const ResourceTransition& transition ) override;
		virtual void AddUavBarrier( const UavBarrier& uavBarrier ) override;

		virtual void BeginQuery( void* rawQuery ) override;
		virtual void EndQuery( void* rawQuery ) override;

		virtual void BeginEvent( const char* eventName ) override;
		virtual void EndEvent() override;

		virtual void Commit() override {}

		virtual void CopyResource( Texture* dest, Texture* src, bool bAsync = true ) override;
		virtual void CopyResource( Buffer* dest, Buffer* src, bool bAsync = true, uint32 numByte = 0 ) override;

		virtual void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, bool bAsync = true, const CubeArea<uint32>* destArea = nullptr, uint32 subresource = 0 ) override;
		virtual void UpdateSubresource( agl::Buffer* dest, const void* src, bool bAsync = true, uint32 destOffset = 0, uint32 numByte = 0 ) override;

		virtual void BindPipelineState( ComputePipelineState* pipelineState ) override;
		virtual void BindShaderResources( ShaderBindings& shaderBindings ) override;
		virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) override;

		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) override;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void DispatchMesh( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* areas ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) override;

		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( GraphicsPipelineState* pipelineState ) override;
		virtual void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil ) override;

		virtual void ClearRenderTarget( RenderTargetView* renderTarget ) override;
		virtual void ClearDepthStencil( DepthStencilView* depthStencil ) override;

		virtual bool CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult ) override;

		virtual void ResolveQueryData( void* queryHeap, D3D12_QUERY_TYPE type, uint32 offset, uint32 numQueries ) override;

		virtual void Signal( ID3D12Fence* fence, uint64 fenceValue ) override;

		void Close();

		void Initialize();

		void OnCommited();

		ID3D12CommandList* Resource() const;

	private:
		bool HasCommands() const;

		friend D3D12CommandList;
		D3D12CommandListImpl m_impl;
	};
}
