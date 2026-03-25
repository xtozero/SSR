#include "D3D12CommandList.h"

#include "D3D12Api.h"
#include "D3D12FlagConvertor.h"
#include "D3D12Query.h"
#include "D3D12ResourceUploader.h"
#include "D3D12ResourceViews.h"

#include "PipelineState.h"

#include "ShaderBindings.h"

#include "WinPixEventRuntime/pix3.h"

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

	D3D12CommandListResource& D3D12CommnadListResourcePool::GetCommandList()
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

	void D3D12BaseCommandListImpl::Initialize()
	{
		InitializeCommandList();
	}

	void D3D12BaseCommandListImpl::Prepare()
	{
		assert( m_cmdListResource.m_fence != nullptr );
		if ( m_cmdListResource.m_fence->GetCompletedValue() > 0 )
		{
			InitializeCommandList();
		}
	}

	bool D3D12BaseCommandListImpl::HasCommands() const
	{
		return m_numCommands > 0;
	}

	ID3D12CommandQueue& D3D12BaseCommandListImpl::GetCommandQueue() const
	{
		return m_type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? D3D12ComputeCommandQueue() : D3D12DirectCommandQueue();
	}

	void D3D12BaseCommandListImpl::BeginQuery( void* rawQuery )
	{
		auto d3dQuery = static_cast<D3D12Query*>( rawQuery );

		CommandList().BeginQuery( d3dQuery->m_heap->GetHeap(), d3dQuery->m_type, d3dQuery->m_offset);

		OnCommandRecorded();
	}

	void D3D12BaseCommandListImpl::EndQuery( void* rawQuery )
	{
		auto d3dQuery = static_cast<D3D12Query*>( rawQuery );

		CommandList().EndQuery( d3dQuery->m_heap->GetHeap(), d3dQuery->m_type, d3dQuery->m_offset);

		OnCommandRecorded();
	}

	void D3D12BaseCommandListImpl::ResolveQueryData( void* queryHeap, D3D12_QUERY_TYPE type, uint32 offset, uint32 numQueries )
	{
		auto d3dQueryHeap = static_cast<D3D12QueryHeapBlock*>( queryHeap );
		ID3D12QueryHeap* heap = d3dQueryHeap->GetHeap();
		ID3D12Resource* readBackBuffer = d3dQueryHeap->GetReadBackBuffer();

		CommandList().ResolveQueryData( heap, type, offset, numQueries, readBackBuffer, GetQueryDataSize( type ) * offset );

		OnCommandRecorded();
	}

	void D3D12BaseCommandListImpl::BeginEvent( const char* eventName )
	{
		static uint64 black = PIX_COLOR( 0, 0, 0 );
		PIXBeginEvent( &CommandList(), black, eventName );

		OnCommandRecorded();
	}

	void D3D12BaseCommandListImpl::EndEvnet()
	{
		PIXEndEvent( &CommandList() );

		OnCommandRecorded();
	}

	ID3D12CommandList* D3D12BaseCommandListImpl::Resource() const
	{
		return m_cmdListResource.m_commandList.Get();
	}

	ID3D12GraphicsCommandList6& D3D12BaseCommandListImpl::CommandList() const
	{
		return *m_cmdListResource.m_commandList.Get();
	}

	void D3D12BaseCommandListImpl::InitializeCommandList()
	{
		m_cmdListResource = D3D12CmdPool( m_type ).GetCommandList();
		m_numCommands = 0;
	}

	void D3D12BaseCommandListImpl::OnCommandRecorded()
	{
	    ++m_numCommands;
	}

	void D3D12CopyCommandListImpl::CopyResource( Texture* dest, Texture* src, bool bAsync )
	{
		auto d3d12Dest = static_cast<D3D12Texture*>( dest );
		auto d3d12Src = static_cast<D3D12Texture*>( src );

		if ( ( d3d12Dest == nullptr ) || ( d3d12Src == nullptr ) )
		{
			return;
		}

		m_barrierBatcher.Commit( *this );

		if ( bAsync )
		{
			D3D12Uploader().Copy( *d3d12Dest, *d3d12Src );
		}
		else
		{
			if ( HasAnyFlags( d3d12Dest->GetTrait().m_access, ResourceAccess::CpuRead ) )
			{
				D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout = {};
				uint32 numRows = 0;
				uint64 rowSize = 0;
				uint64 totalSize = 0;

				D3D12Device().GetCopyableFootprints( &d3d12Src->GetDesc(), 0, 1, 0, &layout, &numRows, &rowSize, &totalSize );

				D3D12_TEXTURE_COPY_LOCATION destLocation = {
					.pResource = static_cast<ID3D12Resource*>( d3d12Dest->Resource() ),
					.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
					.PlacedFootprint = layout
				};

				D3D12_TEXTURE_COPY_LOCATION srcLocation = {
					.pResource = static_cast<ID3D12Resource*>( d3d12Src->Resource() ),
					.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
					.SubresourceIndex = 0
				};

				CommandList().CopyTextureRegion( &destLocation, 0, 0, 0, &srcLocation, nullptr );
			}
			else
			{
				CommandList().CopyResource( static_cast<ID3D12Resource*>( d3d12Dest->Resource() ), static_cast<ID3D12Resource*>( d3d12Src->Resource() ) );
			}

			D3D12FrameResources().RegisterResource( dest );
			D3D12FrameResources().RegisterResource( src );

			OnCommandRecorded();
		}
	}

	void D3D12CopyCommandListImpl::CopyResource( Buffer* dest, Buffer* src, bool bAsync, uint32 numByte )
	{
		auto d3d12Dest = static_cast<D3D12Buffer*>( dest );
		auto d3d12Src = static_cast<D3D12Buffer*>( src );

		if ( ( d3d12Dest == nullptr ) || ( d3d12Src == nullptr ) )
		{
			return;
		}

		m_barrierBatcher.Commit( *this );

		if ( bAsync )
		{
			D3D12Uploader().Copy( *d3d12Dest, *d3d12Src, numByte );
		}
		else
		{
			if ( numByte == 0 )
			{
				CommandList().CopyResource( d3d12Dest->Resource(), d3d12Src->Resource() );
			}
			else
			{
				CommandList().CopyBufferRegion( d3d12Dest->Resource(), 0, d3d12Src->Resource(), 0, numByte );
			}

			D3D12FrameResources().RegisterResource( dest );
            D3D12FrameResources().RegisterResource( src );

			OnCommandRecorded();
		}
	}

	void D3D12CopyCommandListImpl::UpdateSubresource( Texture* dest, const void* src, uint32 srcRowSize, bool bAsync, const CubeArea<uint32>* destArea, uint32 subresource )
	{
		auto d3d12Texture = static_cast<D3D12Texture*>( dest );
		if ( d3d12Texture == nullptr )
		{
			return;
		}

		m_barrierBatcher.Commit( *this );

		if ( bAsync )
		{
			D3D12Uploader().Upload( *d3d12Texture, src, srcRowSize, destArea, subresource );
		}
		else
		{
			auto intermediate = CreateIntermediateInfo( *d3d12Texture, src, srcRowSize, destArea, subresource );

			D3D12_TEXTURE_COPY_LOCATION destLocation = {
				.pResource = static_cast<ID3D12Resource*>( d3d12Texture->Resource() ),
				.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
				.SubresourceIndex = subresource
			};

			D3D12_TEXTURE_COPY_LOCATION srcLocation = {
				.pResource = static_cast<ID3D12Resource*>( intermediate.m_buffer->Resource() ),
				.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
				.PlacedFootprint = intermediate.m_layout
			};

			CommandList().CopyTextureRegion(
				&destLocation,
				intermediate.m_destArea.m_left,
				intermediate.m_destArea.m_top,
				intermediate.m_destArea.m_front,
				&srcLocation,
				nullptr
			);

			D3D12FrameResources().RegisterResource( dest );
			D3D12FrameResources().RegisterResource( intermediate.m_buffer.Get() );

			OnCommandRecorded();
		}
	}

	void D3D12CopyCommandListImpl::UpdateSubresource( Buffer* dest, const void* src, bool bAsync, uint32 destOffset, uint32 numByte )
	{
		auto d3d12Buffer = static_cast<D3D12Buffer*>( dest );
		if ( d3d12Buffer == nullptr )
		{
			return;
		}

		m_barrierBatcher.Commit( *this );

		if ( bAsync )
		{
			D3D12Uploader().Upload( *d3d12Buffer, src, destOffset, numByte );
		}
		else
		{
			if ( numByte == 0 )
			{
				numByte = dest->Size();
			}

			BufferTrait trait = {
				.m_stride = static_cast<uint32>( numByte ),
				.m_count = 1,
				.m_access = ResourceAccess::Upload,
				.m_bindType = ResourceBindType::None,
				.m_miscFlag = ResourceMisc::Intermediate,
				.m_format = ResourceFormat::Unknown
			};

			auto intermediate = Buffer::Create( trait, "Uploader.Buffer.Intermediate" );
			intermediate->Init();

			auto resource = static_cast<ID3D12Resource*>( intermediate->Resource() );
			void* mappedData = nullptr;
			[[maybe_unused]] HRESULT hr = resource->Map( 0, nullptr, &mappedData );
			assert( SUCCEEDED( hr ) );

			std::memcpy( mappedData, src, numByte );

			resource->Unmap( 0, nullptr );

			CommandList().CopyBufferRegion( d3d12Buffer->Resource(), destOffset, resource, 0, numByte );

			D3D12FrameResources().RegisterResource( dest );
			D3D12FrameResources().RegisterResource( intermediate.Get() );

			OnCommandRecorded();
		}
	}

	void D3D12CopyCommandListImpl::AddTransition( const ResourceTransition& transition )
	{
		m_barrierBatcher.AddTransition( transition );
	}

	void D3D12CopyCommandListImpl::AddUavBarrier( const UavBarrier& uavBarrier )
	{
		m_barrierBatcher.AddUavBarrier( uavBarrier );
	}

	void D3D12CopyCommandListImpl::ResourceBarrier( uint32 numBarriers, const D3D12_RESOURCE_BARRIER* barriers )
	{
		CommandList().ResourceBarrier( numBarriers, barriers );

		OnCommandRecorded();
	}

	void D3D12CopyCommandListImpl::Signal( ID3D12Fence* fence, uint64 fenceValue )
	{
		if ( fence == nullptr )
		{
			return;
		}

		m_fenceBatch.emplace_back( fence, fenceValue );
	}

	void D3D12CopyCommandListImpl::Close()
	{
		m_barrierBatcher.Commit( *this );

		if ( HasCommands() == false )
		{
			return;
		}

		CommandList().Close();
	}

	void D3D12CopyCommandListImpl::OnCommited()
	{
		GetCommandQueue().Signal( m_cmdListResource.m_fence.Get(), 1 );
		for ( const auto& pair : m_fenceBatch )
		{
			GetCommandQueue().Signal( pair.first, pair.second );
		}

		InitializeCommandList();
		m_fenceBatch.clear();
	}

	void D3D12ComputeCommandListImpl::Initialize()
	{
		D3D12CopyCommandListImpl::Initialize();
		m_globalConstantBuffers.Initialize();
	}

	void D3D12ComputeCommandListImpl::Prepare()
	{
		assert( m_cmdListResource.m_fence != nullptr );
		if ( m_cmdListResource.m_fence->GetCompletedValue() > 0 )
		{
			m_stateCache.Prepare();
		}

		D3D12CopyCommandListImpl::Prepare();

		m_globalConstantBuffers.Prepare();
		m_globalDescriptorHeap.Prepare();
	}

	void D3D12ComputeCommandListImpl::BindComputePipelineState( const ComputePipelineState* pipelineState )
	{
		m_stateCache.BindPipelineState( CommandList(), pipelineState );
	}

	void D3D12ComputeCommandListImpl::BindShaderResources( const ShaderBindings& shaderBindings )
	{
		if ( shaderBindings.HasBindless() )
		{
			m_stateCache.BindBindlessResources( CommandList(), m_globalDescriptorHeap, m_globalConstantBuffers, shaderBindings );
		}
		else
		{
			m_stateCache.BindShaderResources( CommandList(), m_globalDescriptorHeap, m_globalConstantBuffers, shaderBindings );
		}
	}

	void D3D12ComputeCommandListImpl::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		m_globalConstantBuffers.SetShaderValue( parameter, value );
	}

	void D3D12ComputeCommandListImpl::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_barrierBatcher.Commit( *this );
		m_globalConstantBuffers.CommitShaderValue( true );
		CommandList().Dispatch( x, y, z );
		m_globalConstantBuffers.Reset( true );

		OnCommandRecorded();
	}

	void D3D12ComputeCommandListImpl::DispatchRays( const RaytracingPipelineState* pipelineState, const ShaderBindings& shaderBindings, uint32 width, uint32 height, uint32 depth )
	{
		m_barrierBatcher.Commit( *this );
		m_globalConstantBuffers.CommitShaderValue( true );

		m_stateCache.BindPipelineState( CommandList(), pipelineState );
		BindShaderResources( shaderBindings );

		auto d3d12RaytracingPipelineState = static_cast<const D3D12RaytracingPipelineState*>( pipelineState );
		assert( d3d12RaytracingPipelineState != nullptr );

		D3D12_DISPATCH_RAYS_DESC desc = d3d12RaytracingPipelineState->GetDispatchRaysDesc( width, height, depth );
		CommandList().DispatchRays( &desc );

		m_globalConstantBuffers.Reset( true );

		OnCommandRecorded();
	}

	void D3D12ComputeCommandListImpl::ExecuteIndirect( IndirectCommandType type, Buffer* argument, uint64 argumentOffset )
	{
		auto d3d12Argument = static_cast<D3D12Buffer*>( argument );
		if ( d3d12Argument == nullptr )
		{
			return;
		}

		auto& d3d12ResourceManager = *static_cast<D3D12ResourceManager*>( GetInterface<IResourceManager>() );
		ID3D12CommandSignature* commandSignature = d3d12ResourceManager.FindOrCreate( type );
		if ( commandSignature == nullptr )
		{
			return;
		}

		m_barrierBatcher.Commit( *this );
		bool bCompute = ( type == IndirectCommandType::Dispatch );
		m_globalConstantBuffers.CommitShaderValue( bCompute );
		uint64 argumentBufferOffset = GetIndirectArgumentStride( type ) * argumentOffset;
		CommandList().ExecuteIndirect( commandSignature, 1, d3d12Argument->Resource(), argumentBufferOffset, nullptr, 0 );
		m_globalConstantBuffers.Reset( bCompute );

		D3D12FrameResources().RegisterResource( argument );

		OnCommandRecorded();
	}

	void D3D12ComputeCommandListImpl::BuildRaytracingAccelerationStructure( const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& desc )
	{
		CommandList().BuildRaytracingAccelerationStructure( &desc, 0, nullptr );

		OnCommandRecorded();
	}

	void D3D12ComputeCommandListImpl::OnCommited()
	{
		D3D12CopyCommandListImpl::OnCommited();
		m_stateCache.Prepare();
	}

	void D3D12CommandListImpl::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets )
	{
		m_stateCache.BindVertexBuffer( CommandList(), vertexBuffers, startSlot, numBuffers, strides, pOffsets );
	}

	void D3D12CommandListImpl::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_stateCache.BindIndexBuffer( CommandList(), indexBuffer, indexOffset );
	}

	void D3D12CommandListImpl::BindGraphicsPipelineState( const GraphicsPipelineState* pipelineState )
	{
		m_stateCache.BindPipelineState( CommandList(), pipelineState );
	}

	void D3D12CommandListImpl::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		m_stateCache.BindRenderTargets( CommandList(), pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D12CommandListImpl::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_barrierBatcher.Commit( *this );
		m_globalConstantBuffers.CommitShaderValue( false );
		CommandList().DrawInstanced( vertexCount, numInstance, baseVertexLocation, 0 );
		m_globalConstantBuffers.Reset( false );

		OnCommandRecorded();
	}

	void D3D12CommandListImpl::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_barrierBatcher.Commit( *this );
		m_globalConstantBuffers.CommitShaderValue( false );
		CommandList().DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation, 0 );
		m_globalConstantBuffers.Reset( false );

		OnCommandRecorded();
	}

	void D3D12CommandListImpl::DispatchMesh( uint32 x, uint32 y, uint32 z )
	{
		m_barrierBatcher.Commit( *this );
		m_globalConstantBuffers.CommitShaderValue( false );
		CommandList().DispatchMesh( x, y, z );
		m_globalConstantBuffers.Reset( false );

		OnCommandRecorded();
	}

	void D3D12CommandListImpl::SetViewports( uint32 count, const CubeArea<float>* area )
	{
		m_stateCache.SetViewports( CommandList(), count, area );
	}

	void D3D12CommandListImpl::SetScissorRects( uint32 count, const RectangleArea<int32>* area )
	{
		m_stateCache.SetScissorRects( CommandList(), count, area );
	}

	void D3D12CommandListImpl::ClearRenderTarget( RenderTargetView* renderTarget )
	{
		if ( renderTarget == nullptr )
		{
			return;
		}

		m_barrierBatcher.Commit( *this );

		auto d3d12RTV = static_cast<D3D12RenderTargetView*>( renderTarget );
		D3D12_CPU_DESCRIPTOR_HANDLE handle = d3d12RTV->GetCpuHandle().At();
		ColorF clearValue = d3d12RTV->GetClearColor();
		CommandList().ClearRenderTargetView( handle, clearValue.RGBA(), 0, nullptr);

		OnCommandRecorded();
	}

	void D3D12CommandListImpl::ClearDepthStencil( DepthStencilView* depthStencil )
	{
		if ( depthStencil == nullptr )
		{
			return;
		}

		m_barrierBatcher.Commit( *this );

		auto d3d12DSV = static_cast<D3D12DepthStencilView*>( depthStencil );
		D3D12_CPU_DESCRIPTOR_HANDLE handle = d3d12DSV->GetCpuHandle().At();
		float depthClearValue = d3d12DSV->GetDepthClearValue();
		uint8 stencilClearValue = d3d12DSV->GetStencilClearValue();

		CommandList().ClearDepthStencilView( handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depthClearValue, stencilClearValue, 0, nullptr );

		OnCommandRecorded();
	}

	bool D3D12CommandListImpl::CaptureTexture( const Texture* texture, DirectX::ScratchImage& outResult ) const
	{
		if ( texture == nullptr )
		{
			return false;
		}

		const TextureTrait& trait = texture->GetTrait();

		bool isCubeMap = HasAnyFlags( trait.m_miscFlag, ResourceMisc::TextureCube );
		auto resource = static_cast<ID3D12Resource *>( texture->Resource() );
		if ( resource == nullptr )
		{
			return false;
		}

		D3D12_RESOURCE_STATES resourceState = ConvertToResourceStates( texture->GetResourceState() );

		HRESULT hr = DirectX::CaptureTexture( &GetCommandQueue(), resource, isCubeMap, outResult, resourceState, resourceState );
		return SUCCEEDED( hr );
	}

	void D3D12ComputeCommandList::Prepare()
	{
		m_impl.Prepare();
	}

	void D3D12ComputeCommandList::AddTransition( const ResourceTransition& transition )
	{
		m_impl.AddTransition( transition );
	}

	void D3D12ComputeCommandList::AddUavBarrier( const UavBarrier& uavBarrier )
	{
		m_impl.AddUavBarrier( uavBarrier );
	}

	void D3D12ComputeCommandList::BeginQuery( void* rawQuery )
	{
		m_impl.BeginQuery( rawQuery );
	}

	void D3D12ComputeCommandList::EndQuery( void* rawQuery )
	{
		m_impl.EndQuery( rawQuery );
	}

	void D3D12ComputeCommandList::BeginEvent( const char* eventName )
	{
		m_impl.BeginEvent( eventName );
	}

	void D3D12ComputeCommandList::EndEvent()
	{
		m_impl.EndEvnet();
	}

	void D3D12ComputeCommandList::Commit()
	{
		m_impl.Close();

		if ( m_impl.HasCommands() == false )
		{
			return;
		}

		ID3D12CommandList* commandLists[] = { m_impl.Resource() };

		m_impl.GetCommandQueue().ExecuteCommandLists( 1, commandLists );

		OnCommited();
	}

	void D3D12ComputeCommandList::CopyResource( Texture* dest, Texture* src, bool bAsync )
	{
		m_impl.CopyResource( dest, src, bAsync );
	}

	void D3D12ComputeCommandList::CopyResource( Buffer* dest, Buffer* src, bool bAsync, uint32 numByte )
	{
		m_impl.CopyResource( dest, src, bAsync, numByte );
	}

	void D3D12ComputeCommandList::UpdateSubresource( Texture* dest, const void* src, uint32 srcRowSize, bool bAsync, const CubeArea<uint32>* destArea, uint32 subresource )
	{
		m_impl.UpdateSubresource( dest, src, srcRowSize, bAsync, destArea, subresource );
	}

	void D3D12ComputeCommandList::UpdateSubresource( Buffer* dest, const void* src, bool bAsync, uint32 destOffset, uint32 numByte )
	{
		m_impl.UpdateSubresource( dest, src, bAsync, destOffset, numByte );
	}

	void D3D12ComputeCommandList::BindPipelineState( const ComputePipelineState* pipelineState )
	{
		m_impl.BindComputePipelineState( pipelineState );
	}

	void D3D12ComputeCommandList::BindShaderResources( const ShaderBindings& shaderBindings )
	{
		m_impl.BindShaderResources( shaderBindings );
	}

	void D3D12ComputeCommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		m_impl.SetShaderValue( parameter, value );
	}

	void D3D12ComputeCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_impl.Dispatch( x, y, z );
	}

	void D3D12ComputeCommandList::DispatchRays( const RaytracingPipelineState* pipelineState, const ShaderBindings& shaderBindings, uint32 width, uint32 height, uint32 depth )
	{
		m_impl.DispatchRays( pipelineState, shaderBindings, width, height, depth );
	}

	void D3D12ComputeCommandList::ExecuteIndirect( IndirectCommandType type, Buffer* argument, uint64 argumentOffset )
	{
		m_impl.ExecuteIndirect( type, argument, argumentOffset );
	}

	void D3D12ComputeCommandList::BuildRaytracingAccelerationStructure( const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& desc )
	{
		m_impl.BuildRaytracingAccelerationStructure( desc );
	}

	void D3D12ComputeCommandList::Initialize()
	{
		m_impl.Initialize();
	}

	void D3D12ComputeCommandList::OnCommited()
	{
		m_impl.OnCommited();
	}

	D3D12ComputeCommandList::D3D12ComputeCommandList( D3D12ComputeCommandList&& other ) noexcept
	{
		*this = std::move( other );
	}

	D3D12ComputeCommandList& D3D12ComputeCommandList::operator=( D3D12ComputeCommandList&& other ) noexcept
	{
		if ( this != &other )
		{
			m_impl = std::move( other.m_impl );
		}

		return *this;
	}

	void D3D12CommandList::Prepare()
	{
		m_impl.Prepare();

		for ( auto commandList : m_parallelCommandLists )
		{
			auto d3d12CommandList = static_cast<D3D12ParallelCommandList*>( commandList );
			d3d12CommandList->Prepare();
		}

		m_numUsedParallelCommandList = 0;
	}

	void D3D12CommandList::AddTransition( const ResourceTransition& transition )
	{
		m_impl.AddTransition( transition );
	}

	void D3D12CommandList::AddUavBarrier( const UavBarrier& uavBarrier )
	{
		m_impl.AddUavBarrier( uavBarrier );
	}

	void D3D12CommandList::BeginQuery( void* rawQuery )
	{
		m_impl.BeginQuery( rawQuery );
	}

	void D3D12CommandList::EndQuery( void* rawQuery )
	{
		m_impl.EndQuery( rawQuery );
	}

	void D3D12CommandList::BeginEvent( const char* eventName )
	{
		m_impl.BeginEvent( eventName );
	}

	void D3D12CommandList::EndEvent()
	{
		m_impl.EndEvnet();
	}

	void D3D12CommandList::Commit()
	{
		m_impl.Close();

		std::vector<ID3D12CommandList*, InlineAllocator<ID3D12CommandList*, 1>> commandLists;

		if ( m_impl.HasCommands() )
		{
			commandLists.push_back( m_impl.Resource() );
		}

		for ( size_t i = 0; i < m_parallelCommandLists.size(); ++i )
		{
			auto d3d12CommandList = static_cast<D3D12ParallelCommandList*>( m_parallelCommandLists[i] );
			d3d12CommandList->Close();

			if ( d3d12CommandList->HasCommands() )
			{
				commandLists.push_back( d3d12CommandList->Resource() );
			}
		}

		if ( commandLists.empty() )
		{
			return;
		}

		auto numCommandList = static_cast<uint32>( commandLists.size() );
		m_impl.GetCommandQueue().ExecuteCommandLists( numCommandList, commandLists.data() );

		OnCommited();
		for ( size_t i = 0; i < m_parallelCommandLists.size(); ++i )
		{
			auto d3d12CommandList = static_cast<D3D12ParallelCommandList*>( m_parallelCommandLists[i] );
			d3d12CommandList->OnCommited();
		}
	}

	void D3D12CommandList::CopyResource( Texture* dest, Texture* src, bool bAsync )
	{
		m_impl.CopyResource( dest, src, bAsync );
	}

	void D3D12CommandList::CopyResource( Buffer* dest, Buffer* src, bool bAsync, uint32 numByte )
	{
		m_impl.CopyResource( dest, src, bAsync, numByte );
	}

	void D3D12CommandList::UpdateSubresource( Texture* dest, const void* src, uint32 srcRowSize, bool bAsync, const CubeArea<uint32>* destArea, uint32 subresource )
	{
		m_impl.UpdateSubresource( dest, src, srcRowSize, bAsync, destArea, subresource );
	}

	void D3D12CommandList::UpdateSubresource( Buffer* dest, const void* src, bool bAsync, uint32 destOffset, uint32 numByte )
	{
		m_impl.UpdateSubresource( dest, src, bAsync, destOffset, numByte );
	}

	void D3D12CommandList::BindPipelineState( const ComputePipelineState* pipelineState )
    {
    	m_impl.BindComputePipelineState( pipelineState );
    }

    void D3D12CommandList::BindShaderResources( const ShaderBindings& shaderBindings )
    {
    	m_impl.BindShaderResources( shaderBindings );
    }

    void D3D12CommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
    {
    	m_impl.SetShaderValue( parameter, value );
    }

	void D3D12CommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_impl.Dispatch( x, y, z );
	}

	void D3D12CommandList::DispatchRays( const RaytracingPipelineState* pipelineState, const ShaderBindings& shaderBindings, uint32 width, uint32 height, uint32 depth )
	{
		m_impl.DispatchRays( pipelineState, shaderBindings, width, height, depth );
	}

	void D3D12CommandList::ExecuteIndirect( IndirectCommandType type, Buffer* argument, uint64 argumentOffset )
	{
		m_impl.ExecuteIndirect( type, argument, argumentOffset );
	}

	void D3D12CommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_impl.DrawInstanced( vertexCount, numInstance, baseVertexLocation );
	}

	void D3D12CommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_impl.DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation );
	}

	void D3D12CommandList::DispatchMesh( uint32 x, uint32 y, uint32 z )
	{
		m_impl.DispatchMesh( x, y, z );
	}

	void D3D12CommandList::SetViewports( uint32 count, const CubeArea<float>* area )
	{
		m_impl.SetViewports( count, area );
	}

	void D3D12CommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* area )
	{
		m_impl.SetScissorRects( count, area );
	}

	void D3D12CommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets )
	{
		m_impl.BindVertexBuffer( vertexBuffers, startSlot, numBuffers, strides, pOffsets );
	}

	void D3D12CommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_impl.BindIndexBuffer( indexBuffer, indexOffset );
	}

	void D3D12CommandList::BindPipelineState( const GraphicsPipelineState* pipelineState )
	{
		m_impl.BindGraphicsPipelineState( pipelineState );
	}

	void D3D12CommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		m_impl.BindRenderTargets( pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D12CommandList::ClearRenderTarget( RenderTargetView* renderTarget )
	{
		m_impl.ClearRenderTarget( renderTarget );
	}

	void D3D12CommandList::ClearDepthStencil( DepthStencilView* depthStencil )
	{
		m_impl.ClearDepthStencil( depthStencil );
	}

	bool D3D12CommandList::CaptureTexture( Texture* texture, DirectX::ScratchImage& outResult )
	{
		return m_impl.CaptureTexture( texture, outResult );
	}

	void D3D12CommandList::ResolveQueryData( void* queryHeap, D3D12_QUERY_TYPE type, uint32 offset, uint32 numQueries )
	{
		m_impl.ResolveQueryData( queryHeap, type, offset, numQueries );
	}

	void D3D12CommandList::Signal( ID3D12Fence* fence, uint64 fenceValue )
	{
		m_impl.Signal( fence, fenceValue );
	}

	void D3D12CommandList::Initialize()
	{
		m_impl.Initialize();
	}

	void D3D12CommandList::OnCommited()
	{
		m_impl.OnCommited();
	}

	ID3D12CommandListEX& D3D12CommandList::GetParallelCommandList()
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

	D3D12CommandList::D3D12CommandList( D3D12CommandList&& other ) noexcept
	{
		*this = std::move( other );
	}

	D3D12CommandList& D3D12CommandList::operator=( D3D12CommandList&& other ) noexcept
	{
		if ( this != &other )
		{
			m_impl = std::move( other.m_impl );
			m_numUsedParallelCommandList = other.m_numUsedParallelCommandList;
			m_parallelCommandLists = std::move( m_parallelCommandLists );
		}

		return *this;
	}

	D3D12CommandList::~D3D12CommandList()
	{
		for ( ID3D12CommandListEX* commandList : m_parallelCommandLists )
		{
			delete commandList;
		}
		m_parallelCommandLists.clear();
	}

	void D3D12ParallelCommandList::Prepare()
	{
		m_impl.Prepare();
	}

	void D3D12ParallelCommandList::AddTransition( const ResourceTransition& transition )
	{
		m_impl.AddTransition( transition );
	}

	void D3D12ParallelCommandList::AddUavBarrier( const UavBarrier& uavBarrier )
	{
		m_impl.AddUavBarrier( uavBarrier );
	}

	void D3D12ParallelCommandList::BeginQuery( void* rawQuery )
	{
		m_impl.BeginQuery( rawQuery );
	}

	void D3D12ParallelCommandList::EndQuery( void* rawQuery )
	{
		m_impl.EndQuery( rawQuery );
	}

	void D3D12ParallelCommandList::BeginEvent( const char* eventName )
	{
		m_impl.BeginEvent( eventName );
	}

	void D3D12ParallelCommandList::EndEvent()
	{
		m_impl.EndEvnet();
	}

	void D3D12ParallelCommandList::CopyResource( Texture* dest, Texture* src, bool bAsync )
	{
		m_impl.CopyResource( dest, src, bAsync );
	}

	void D3D12ParallelCommandList::CopyResource( Buffer* dest, Buffer* src, bool bAsync, uint32 numByte )
	{
		m_impl.CopyResource( dest, src, bAsync, numByte );
	}

	void D3D12ParallelCommandList::UpdateSubresource( Texture* dest, const void* src, uint32 srcRowSize, bool bAsync, const CubeArea<uint32>* destArea, uint32 subresource )
	{
		m_impl.UpdateSubresource( dest, src, srcRowSize, bAsync, destArea, subresource );
	}

	void D3D12ParallelCommandList::UpdateSubresource( Buffer* dest, const void* src, bool bAsync, uint32 destOffset, uint32 numByte )
	{
		m_impl.UpdateSubresource( dest, src, bAsync, destOffset, numByte );
	}

	void D3D12ParallelCommandList::BindPipelineState( const ComputePipelineState* pipelineState )
	{
		m_impl.BindComputePipelineState( pipelineState );
	}

	void D3D12ParallelCommandList::BindShaderResources( const ShaderBindings& shaderBindings )
	{
		m_impl.BindShaderResources( shaderBindings );
	}

	void D3D12ParallelCommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		m_impl.SetShaderValue( parameter, value );
	}

	void D3D12ParallelCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_impl.Dispatch( x, y, z );
	}

	void D3D12ParallelCommandList::DispatchRays( const RaytracingPipelineState* pipelineState, const ShaderBindings& shaderBindings, uint32 width, uint32 height, uint32 depth )
	{
		m_impl.DispatchRays( pipelineState, shaderBindings, width, height, depth );
	}

	void D3D12ParallelCommandList::ExecuteIndirect( IndirectCommandType type, Buffer* argument, uint64 argumentOffset )
	{
		m_impl.ExecuteIndirect( type, argument, argumentOffset );
	}

	void D3D12ParallelCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_impl.DrawInstanced( vertexCount, numInstance, baseVertexLocation );
	}

	void D3D12ParallelCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_impl.DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation );
	}

	void D3D12ParallelCommandList::DispatchMesh( uint32 x, uint32 y, uint32 z )
	{
		m_impl.DispatchMesh( x, y, z );
	}

	void D3D12ParallelCommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
	{
		m_impl.SetViewports( count, areas );
	}

	void D3D12ParallelCommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
	{
		m_impl.SetScissorRects( count, areas );
	}

	void D3D12ParallelCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets )
	{
		m_impl.BindVertexBuffer( vertexBuffers, startSlot, numBuffers, strides, pOffsets );
	}

	void D3D12ParallelCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_impl.BindIndexBuffer( indexBuffer, indexOffset );
	}

	void D3D12ParallelCommandList::BindPipelineState( const GraphicsPipelineState* pipelineState )
	{
		m_impl.BindGraphicsPipelineState( pipelineState );
	}

	void D3D12ParallelCommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		m_impl.BindRenderTargets( pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D12ParallelCommandList::ClearRenderTarget( RenderTargetView* renderTarget )
	{
		m_impl.ClearRenderTarget( renderTarget );
	}

	void D3D12ParallelCommandList::ClearDepthStencil( DepthStencilView* depthStencil )
	{
		m_impl.ClearDepthStencil( depthStencil );
	}

	bool D3D12ParallelCommandList::CaptureTexture( Texture* texture, DirectX::ScratchImage& outResult )
	{
		return m_impl.CaptureTexture( texture, outResult );
	}

	void D3D12ParallelCommandList::ResolveQueryData( void* queryHeap, D3D12_QUERY_TYPE type, uint32 offset, uint32 numQueries )
	{
		m_impl.ResolveQueryData( queryHeap, type, offset, numQueries );
	}

	void D3D12ParallelCommandList::Signal( ID3D12Fence* fence, uint64 fenceValue )
	{
		m_impl.Signal( fence, fenceValue );
	}

	void D3D12ParallelCommandList::Close()
	{
		m_impl.Close();
	}

	void D3D12ParallelCommandList::Initialize()
	{
		m_impl.Initialize();
	}

	void D3D12ParallelCommandList::OnCommited()
	{
		m_impl.OnCommited();
	}

	ID3D12CommandList* D3D12ParallelCommandList::Resource() const
	{
		return m_impl.Resource();
	}

	bool D3D12ParallelCommandList::HasCommands() const
	{
		return m_impl.HasCommands();
	}
}
