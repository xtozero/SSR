#include "D3D12CommandList.h"

#include "D3D12Api.h"
#include "D3D12FlagConvertor.h"
#include "D3D12ResourceUploader.h"
#include "D3D12ResourceViews.h"

#include "PipelineState.h"

using ::Microsoft::WRL::ComPtr;

namespace agl
{
	void D3D12CommnadListResourcePool::Prepare()
	{
		D3D12CommandListResource* iter = m_runningList;
		while ( iter != nullptr )
		{
			D3D12CommandListResource* next = iter->m_next;

			if ( iter->m_fence->GetCompletedValue() > 0 )
			{
				SLinkedList::Remove( m_runningList, iter );
				SLinkedList::AddToHead( m_freeList, iter );
			}

			iter = next;
		}
	}

	D3D12CommandListResource& D3D12CommnadListResourcePool::ObtainCommandList()
	{
		D3D12CommandListResource* ret = nullptr;
		if ( m_freeList == nullptr )
		{
			ret = m_allocator.Allocate();
			std::construct_at( ret );

			[[maybe_unused]] HRESULT hr = D3D12Device().CreateCommandAllocator( m_type, IID_PPV_ARGS( &ret->m_commandAllocator ) );
			assert( SUCCEEDED( hr ) );

			ComPtr<ID3D12GraphicsCommandList> commandList;

			hr = D3D12Device().CreateCommandList( 0, m_type, ret->m_commandAllocator.Get(), nullptr, IID_PPV_ARGS( &commandList ) );
			assert( SUCCEEDED( hr ) );

			commandList.As( &ret->m_commandList );

			hr = D3D12Device().CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &ret->m_fence ) );
			assert( SUCCEEDED( hr ) );
		}
		else
		{
			ret = m_freeList;
			SLinkedList::Remove( m_freeList, ret );

			[[maybe_unused]] HRESULT hr = ret->m_commandAllocator->Reset();
			assert( SUCCEEDED( hr ) );

			hr = ret->m_commandList->Reset( ret->m_commandAllocator.Get(), nullptr );
			assert( SUCCEEDED( hr ) );

			hr = ret->m_fence->Signal( 0 );
			assert( SUCCEEDED( hr ) );
		}

		SLinkedList::AddToHead( m_runningList, ret );
		return *ret;
	}

	D3D12CommnadListResourcePool::D3D12CommnadListResourcePool( D3D12_COMMAND_LIST_TYPE type )
		: m_type( type )
	{
	}

	D3D12CommnadListResourcePool::~D3D12CommnadListResourcePool()
	{
		D3D12CommandListResource* iter = m_freeList;
		while ( iter != nullptr )
		{
			D3D12CommandListResource* next = iter->m_next;
			std::destroy_at( iter );
			iter = next;
		}

		iter = m_runningList;
		while ( iter != nullptr )
		{
			D3D12CommandListResource* next = iter->m_next;
			std::destroy_at( iter );
			iter = next;
		}
	}

	void D3D12CommandListImpl::Initialize()
	{
		m_globalConstantBuffers.Initialize();
		m_cmdListResource = D3D12CmdPool( D3D12_COMMAND_LIST_TYPE_DIRECT ).ObtainCommandList();
	}

	void D3D12CommandListImpl::Prepare()
	{
		if ( m_cmdListResource.m_fence->GetCompletedValue() > 0 )
		{
			m_cmdListResource = D3D12CmdPool( D3D12_COMMAND_LIST_TYPE_DIRECT ).ObtainCommandList();
			m_stateCache.Prepare();
		}

		m_globalConstantBuffers.Prepare();
		m_globalDescriptorHeap.Prepare();
	}

	void D3D12CommandListImpl::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		m_stateCache.BindVertexBuffer( CommandList(), vertexBuffers, startSlot, numBuffers, pOffsets);
	}

	void D3D12CommandListImpl::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_stateCache.BindIndexBuffer( CommandList(), indexBuffer, indexOffset );
	}

	void D3D12CommandListImpl::BindPipelineState( GraphicsPipelineState* pipelineState )
	{
		m_stateCache.BindPipelineState( CommandList(), pipelineState );
	}

	void D3D12CommandListImpl::BindPipelineState( ComputePipelineState* pipelineState )
	{
		m_stateCache.BindPipelineState( CommandList(), pipelineState );
	}

	void D3D12CommandListImpl::BindShaderResources( ShaderBindings& shaderBindings )
	{
		m_globalConstantBuffers.AddGlobalConstantBuffers( shaderBindings );
		m_stateCache.BindShaderResources( CommandList(), m_globalDescriptorHeap, shaderBindings );
	}

	void D3D12CommandListImpl::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		m_globalConstantBuffers.SetShaderValue( parameter, value );
	}

	void D3D12CommandListImpl::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
		CommandList().DrawInstanced( vertexCount, numInstance, baseVertexLocation, 0 );
		m_globalConstantBuffers.Reset( false );
	}

	void D3D12CommandListImpl::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
		CommandList().DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation, 0 );
		m_globalConstantBuffers.Reset( false );
	}

	void D3D12CommandListImpl::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_globalConstantBuffers.CommitShaderValue( true );
		CommandList().Dispatch( x, y, z );
		m_globalConstantBuffers.Reset( true );
	}

	void D3D12CommandListImpl::SetViewports( uint32 count, const CubeArea<float>* area )
	{
		m_stateCache.SetViewports( CommandList(), count, area );
	}

	void D3D12CommandListImpl::SetScissorRects( uint32 count, const RectangleArea<int32>* area )
	{
		m_stateCache.SetScissorRects( CommandList(), count, area );
	}

	void D3D12CommandListImpl::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		m_stateCache.BindRenderTargets( CommandList(), pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D12CommandListImpl::ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] )
	{
		if ( renderTarget == nullptr )
		{
			return;
		}

		auto d3d12RTV = static_cast<D3D12RenderTargetView*>( renderTarget );
		D3D12_CPU_DESCRIPTOR_HANDLE handle = d3d12RTV->GetCpuHandle().At();
		CommandList().ClearRenderTargetView( handle, clearColor, 0, nullptr );
	}

	void D3D12CommandListImpl::ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
	{
		if ( depthStencil == nullptr )
		{
			return;
		}

		auto d3d12DSV = static_cast<D3D12DepthStencilView*>( depthStencil );
		D3D12_CPU_DESCRIPTOR_HANDLE handle = d3d12DSV->GetCpuHandle().At();
		CommandList().ClearDepthStencilView( handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depthColor, stencilColor, 0, nullptr );
	}

	void D3D12CommandListImpl::CopyResource( Texture* dest, Texture* src, bool bDirect )
	{
		auto d3d12Dest = static_cast<D3D12Texture*>( dest );
		auto d3d12Src = static_cast<D3D12Texture*>( src );

		if ( ( d3d12Dest == nullptr ) || ( d3d12Src == nullptr ) )
		{
			return;
		}

		if ( bDirect )
		{
			CommandList().CopyResource( static_cast<ID3D12Resource*>( d3d12Dest->Resource() ),	static_cast<ID3D12Resource*>( d3d12Src->Resource() ) );
		}
		else
		{
			D3D12Uploader().Copy( *d3d12Dest, *d3d12Src );
		}
	}

	void D3D12CommandListImpl::CopyResource( Buffer* dest, Buffer* src, uint32 numByte, bool bDirect )
	{
		auto d3d12Dest = static_cast<D3D12Buffer*>( dest );
		auto d3d12Src = static_cast<D3D12Buffer*>( src );

		if ( ( d3d12Dest == nullptr ) || ( d3d12Src == nullptr ) )
		{
			return;
		}

		if ( bDirect )
		{
			if ( numByte == 0 )
			{
				CommandList().CopyResource( d3d12Dest->Resource(), d3d12Src->Resource() );
			}
			else
			{
				CommandList().CopyBufferRegion( d3d12Dest->Resource(), 0, d3d12Src->Resource(), 0, numByte );
			}
		}
		else
		{
			D3D12Uploader().Copy( *d3d12Dest, *d3d12Src, numByte );
		}
	}

	void D3D12CommandListImpl::UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea, uint32 subresource )
	{
		auto d3d12Texture = static_cast<D3D12Texture*>( dest );
		if ( d3d12Texture == nullptr )
		{
			return;
		}

		D3D12Uploader().Upload( *d3d12Texture, src, srcRowSize, destArea, subresource );
	}

	void D3D12CommandListImpl::UpdateSubresource( agl::Buffer* dest, const void* src, uint32 destOffset, uint32 numByte )
	{
		auto d3d12Buffer = static_cast<D3D12Buffer*>( dest );
		if ( d3d12Buffer == nullptr )
		{
			return;
		}

		D3D12Uploader().Upload( *d3d12Buffer, src, destOffset, numByte );
	}

	void D3D12CommandListImpl::Transition( uint32 numTransitions, const ResourceTransition* transitions )
	{
		std::vector<D3D12_RESOURCE_BARRIER, InlineAllocator<D3D12_RESOURCE_BARRIER, 1>> d3d12Barriers;
		d3d12Barriers.reserve( numTransitions );
		for ( uint32 i = 0; i < numTransitions; ++i )
		{
			ResourceState beforeState = transitions[i].m_pTransitionable.GetState();
			ResourceState afterState = transitions[i].m_state;

			if ( beforeState != afterState )
			{
				d3d12Barriers.emplace_back( ConvertToResourceBarrier( transitions[i] ) );
				transitions[i].m_pTransitionable.SetState( transitions[i].m_state );
			}
		}

		if ( d3d12Barriers.empty() )
		{
			return;
		}

		CommandList().ResourceBarrier( static_cast<uint32>( d3d12Barriers.size() ), d3d12Barriers.data() );
	}

	void D3D12CommandListImpl::Close()
	{
		CommandList().Close();
	}

	void D3D12CommandListImpl::OnCommited()
	{
		D3D12DirectCommandQueue().Signal( m_cmdListResource.m_fence.Get(), 1 );

		m_cmdListResource = D3D12CmdPool( D3D12_COMMAND_LIST_TYPE_DIRECT ).ObtainCommandList();
		m_stateCache.Prepare();
	}

	ID3D12CommandList* D3D12CommandListImpl::Resource() const
	{
		return m_cmdListResource.m_commandList.Get();
	}

	ID3D12GraphicsCommandList6& D3D12CommandListImpl::CommandList()
	{
		return *m_cmdListResource.m_commandList.Get();
	}

	void D3D12CommandList::Prepare()
	{
		m_imple.Prepare();

		for ( auto commandList : m_parallelCommandLists )
		{
			auto d3d12CommandList = static_cast<D3D12ParallelCommandList*>( commandList );
			d3d12CommandList->Prepare();
		}

		m_numUsedParallelCommandList = 0;
	}

	void D3D12CommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		m_imple.BindVertexBuffer( vertexBuffers, startSlot, numBuffers, pOffsets );
	}

	void D3D12CommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_imple.BindIndexBuffer( indexBuffer, indexOffset );
	}

	void D3D12CommandList::BindPipelineState( GraphicsPipelineState* pipelineState )
	{
		m_imple.BindPipelineState( pipelineState );
	}

	void D3D12CommandList::BindPipelineState( ComputePipelineState* pipelineState )
	{
		m_imple.BindPipelineState( pipelineState );
	}

	void D3D12CommandList::BindShaderResources( ShaderBindings& shaderBindings )
	{
		m_imple.BindShaderResources( shaderBindings );
	}

	void D3D12CommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		m_imple.SetShaderValue( parameter, value );
	}

	void D3D12CommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_imple.DrawInstanced( vertexCount, numInstance, baseVertexLocation );
	}

	void D3D12CommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_imple.DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation );
	}

	void D3D12CommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_imple.Dispatch( x, y, z );
	}

	void D3D12CommandList::SetViewports( uint32 count, const CubeArea<float>* area )
	{
		m_imple.SetViewports( count, area );
	}

	void D3D12CommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* area )
	{
		m_imple.SetScissorRects( count, area );
	}

	void D3D12CommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		m_imple.BindRenderTargets( pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D12CommandList::ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] )
	{
		m_imple.ClearRenderTarget( renderTarget, clearColor );
	}

	void D3D12CommandList::ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
	{
		m_imple.ClearDepthStencil( depthStencil, depthColor, stencilColor );
	}

	void D3D12CommandList::CopyResource( Texture* dest, Texture* src, bool bDirect )
	{
		m_imple.CopyResource( dest, src, bDirect );
	}

	void D3D12CommandList::CopyResource( Buffer* dest, Buffer* src, uint32 numByte, bool bDirect )
	{
		m_imple.CopyResource( dest, src, numByte, bDirect );
	}

	void D3D12CommandList::UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea, uint32 subresource )
	{
		m_imple.UpdateSubresource( dest, src, srcRowSize, destArea, subresource );
	}

	void D3D12CommandList::UpdateSubresource( agl::Buffer* dest, const void* src, uint32 destOffset, uint32 numByte )
	{
		m_imple.UpdateSubresource( dest, src, destOffset, numByte );
	}

	void D3D12CommandList::Transition( uint32 numTransitions, const ResourceTransition* transitions )
	{
		m_imple.Transition( numTransitions, transitions );
	}

	IParallelCommandList& D3D12CommandList::GetParallelCommandList()
	{
		if ( m_numUsedParallelCommandList >= m_parallelCommandLists.size() )
		{
			auto newCommandList = new D3D12ParallelCommandList();
			newCommandList->Initialize();
			newCommandList->Prepare();

			m_parallelCommandLists.push_back( newCommandList );
		}

		return *m_parallelCommandLists[m_numUsedParallelCommandList++];
	}

	void D3D12CommandList::WaitUntilFlush()
	{
	}

	void D3D12CommandList::Commit()
	{
		m_imple.Close();

		std::vector<ID3D12CommandList*, InlineAllocator<ID3D12CommandList*, 1>> commandLists;
		commandLists.push_back( m_imple.Resource() );

		for ( size_t i = 0; i < m_parallelCommandLists.size(); ++i )
		{
			auto d3d12CommandList = static_cast<D3D12ParallelCommandList*>( m_parallelCommandLists[i] );
			d3d12CommandList->Close();

			commandLists.push_back( d3d12CommandList->Resource() );
		}

		auto numCommandList = static_cast<uint32>( commandLists.size() );
		D3D12DirectCommandQueue().ExecuteCommandLists( numCommandList, commandLists.data() );

		m_imple.OnCommited();
		for ( size_t i = 0; i < m_parallelCommandLists.size(); ++i )
		{
			auto d3d12CommandList = static_cast<D3D12ParallelCommandList*>( m_parallelCommandLists[i] );
			d3d12CommandList->OnCommitted();
		}
	}

	void D3D12CommandList::Initialize()
	{
		m_imple.Initialize();
	}

	void D3D12CommandList::OnCommitted()
	{
		m_imple.OnCommited();
	}

	D3D12CommandList::D3D12CommandList( D3D12CommandList&& other ) noexcept
	{
		*this = std::move( other );
	}

	D3D12CommandList& D3D12CommandList::operator=( D3D12CommandList&& other ) noexcept
	{
		if ( this != &other )
		{
			m_imple = std::move( other.m_imple );
			m_numUsedParallelCommandList = other.m_numUsedParallelCommandList;
			m_parallelCommandLists = std::move( m_parallelCommandLists );
		}

		return *this;
	}

	D3D12CommandList::~D3D12CommandList()
	{
		for ( IParallelCommandList* commandList : m_parallelCommandLists )
		{
			delete commandList;
		}
		m_parallelCommandLists.clear();
	}

	void D3D12ParallelCommandList::Prepare()
	{
		m_imple.Prepare();
	}

	void D3D12ParallelCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		m_imple.BindVertexBuffer( vertexBuffers, startSlot, numBuffers, pOffsets );
	}

	void D3D12ParallelCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_imple.BindIndexBuffer( indexBuffer, indexOffset );
	}

	void D3D12ParallelCommandList::BindPipelineState( GraphicsPipelineState* pipelineState )
	{
		m_imple.BindPipelineState( pipelineState );
	}

	void D3D12ParallelCommandList::BindPipelineState( ComputePipelineState* pipelineState )
	{
		m_imple.BindPipelineState( pipelineState );
	}

	void D3D12ParallelCommandList::BindShaderResources( ShaderBindings& shaderBindings )
	{
		m_imple.BindShaderResources( shaderBindings );
	}

	void D3D12ParallelCommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		m_imple.SetShaderValue( parameter, value );
	}

	void D3D12ParallelCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_imple.DrawInstanced( vertexCount, numInstance, baseVertexLocation );
	}

	void D3D12ParallelCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_imple.DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation );
	}

	void D3D12ParallelCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_imple.Dispatch( x, y, z );
	}

	void D3D12ParallelCommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
	{
		m_imple.SetViewports( count, areas );
	}

	void D3D12ParallelCommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
	{
		m_imple.SetScissorRects( count, areas );
	}

	void D3D12ParallelCommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		m_imple.BindRenderTargets( pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D12ParallelCommandList::ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] )
	{
		m_imple.ClearRenderTarget( renderTarget, clearColor );
	}

	void D3D12ParallelCommandList::ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
	{
		m_imple.ClearDepthStencil( depthStencil, depthColor, stencilColor );
	}

	void D3D12ParallelCommandList::CopyResource( Texture* dest, Texture* src, bool bDirect )
	{
		m_imple.CopyResource( dest, src, bDirect );
	}

	void D3D12ParallelCommandList::CopyResource( Buffer* dest, Buffer* src, uint32 numByte, bool bDirect )
	{
		m_imple.CopyResource( dest, src, numByte, bDirect );
	}

	void D3D12ParallelCommandList::UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea, uint32 subresource )
	{
		m_imple.UpdateSubresource( dest, src, srcRowSize, destArea, subresource );
	}

	void D3D12ParallelCommandList::UpdateSubresource( agl::Buffer* dest, const void* src, uint32 destOffset, uint32 numByte )
	{
		m_imple.UpdateSubresource( dest, src, destOffset, numByte );
	}

	void D3D12ParallelCommandList::Transition( uint32 numTransitions, const ResourceTransition* transitions )
	{
		m_imple.Transition( numTransitions, transitions );
	}

	void D3D12ParallelCommandList::Close()
	{
		m_imple.Close();
	}

	void D3D12ParallelCommandList::Initialize()
	{
		m_imple.Initialize();
	}

	void D3D12ParallelCommandList::OnCommitted()
	{
		m_imple.OnCommited();
	}

	ID3D12CommandList* D3D12ParallelCommandList::Resource() const
	{
		return m_imple.Resource();
	}
}
