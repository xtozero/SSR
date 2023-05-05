#include "D3D12ResourceUploader.h"

#include "D3D12Api.h"

#include <cassert>
#include <tchar.h>

namespace agl
{
	void D3D12UploadContext::Prepare()
	{
		m_cmdListResource = D3D12CmdPool( D3D12_COMMAND_LIST_TYPE_COPY ).ObtainCommandList();
	}

	void D3D12UploadContext::RecordUploadCommand( D3D12Buffer& dest, const void* data, uint32 destOffset, uint32 numByte )
	{
		assert( m_destResource.Get() == nullptr );

		m_destResource = &dest;

		if ( numByte == 0 )
		{
			numByte = dest.Size();
		}

		D3D12HeapProperties heapProperties = {
			.m_alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.m_heapType = D3D12_HEAP_TYPE_UPLOAD,
			.m_heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS
		};

		D3D12_RESOURCE_DESC desc = {
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Width = numByte,
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags = D3D12_RESOURCE_FLAG_NONE
		};

		D3D12ResourceAllocator& allocator = D3D12Allocator();
		m_srcResourceInfo = allocator.AllocateResource( heapProperties, desc, D3D12_RESOURCE_STATE_GENERIC_READ );

		ID3D12Resource* resource = m_srcResourceInfo.GetResource();
		void* mappedData = nullptr;
		[[maybe_unused]] HRESULT hr = resource->Map( 0, nullptr, &mappedData );
		assert( SUCCEEDED( hr ) );

		std::memcpy( mappedData, data, numByte );

		resource->Unmap( 0, nullptr );

		CommandList()->CopyBufferRegion( dest.Resource(), destOffset, resource, 0, numByte );

		CommandList()->Close();
	}

	void D3D12UploadContext::RecordUploadCommand( D3D12Texture& dest, const void* data, uint32 srcRowSize, const CubeArea<uint32>* pDestArea, uint32 subresource )
	{
		assert( m_destResource.Get() == nullptr );

		m_destResource = &dest;

		const TextureTrait& destTrait = dest.GetTrait();
		bool bIsTexture3D = HasAnyFlags( destTrait.m_miscFlag, ResourceMisc::Texture3D );

		CubeArea<uint32> destArea;
		if ( pDestArea == nullptr )
		{
			destArea = {
				.m_left = 0,
				.m_top = 0,
				.m_right = destTrait.m_width,
				.m_bottom = destTrait.m_height,
				.m_back = bIsTexture3D ? destTrait.m_depth : 1,
				.m_front = 0
			};
		}
		else
		{
			destArea = *pDestArea;
		}

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout = {};
		uint32 numRows = 0;
		uint64 rowSize = 0;
		uint64 totalSize = 0;

		D3D12_RESOURCE_DESC intermediateDesc = dest.GetDesc();
		intermediateDesc.Width = destArea.m_right - destArea.m_left;
		intermediateDesc.Height = destArea.m_bottom - destArea.m_top;
		intermediateDesc.DepthOrArraySize = static_cast<uint16>( destArea.m_back - destArea.m_front );

		D3D12Device().GetCopyableFootprints( &dest.GetDesc(), subresource, 1, 0, &layout, &numRows, &rowSize, &totalSize);

		D3D12HeapProperties heapProperties = {
			.m_alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.m_heapType = D3D12_HEAP_TYPE_UPLOAD,
			.m_heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS
		};

		D3D12_RESOURCE_DESC desc = {
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Width = totalSize,
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags = D3D12_RESOURCE_FLAG_NONE
		};

		D3D12ResourceAllocator& allocator = D3D12Allocator();
		m_srcResourceInfo = allocator.AllocateResource( heapProperties, desc, D3D12_RESOURCE_STATE_GENERIC_READ );

		ID3D12Resource* resource = m_srcResourceInfo.GetResource();
		uint8* mappedData = nullptr;
		[[maybe_unused]] HRESULT hr = resource->Map( 0, nullptr, reinterpret_cast<void**>( &mappedData ) );
		assert( SUCCEEDED( hr ) );

		auto srcData = static_cast<const uint8*>( data );

		for ( uint32 z = 0, zEnd = destArea.m_back - destArea.m_front; z < zEnd; ++z )
		{
			auto row = mappedData;
			for ( uint32 y = 0, yEnd = numRows; y < yEnd; ++y )
			{
				std::memcpy( row, srcData, srcRowSize );
				srcData += srcRowSize;
				row += rowSize;
			}
			mappedData += ( numRows * rowSize );
		}

		resource->Unmap( 0, nullptr );

		D3D12_TEXTURE_COPY_LOCATION destLocation = {
			.pResource = static_cast<ID3D12Resource*>( dest.Resource() ),
			.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			.SubresourceIndex = subresource
		};

		D3D12_TEXTURE_COPY_LOCATION srcLocation = {
			.pResource = resource,
			.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
			.PlacedFootprint = layout
		};

		CommandList()->CopyTextureRegion(
			&destLocation,
			destArea.m_left, 
			destArea.m_top,
			destArea.m_front,
			&srcLocation,
			nullptr
		);

		CommandList()->Close();
	}

	bool D3D12UploadContext::IsFinished() const
	{
		return Fence()->GetCompletedValue() > 0;
	}

	D3D12UploadContext::~D3D12UploadContext()
	{
		m_srcResourceInfo.Release();
	}

	ID3D12GraphicsCommandList6* D3D12UploadContext::CommandList() const
	{
		return m_cmdListResource.m_commandList.Get();
	}

	ID3D12Fence* D3D12UploadContext::Fence() const
	{
		return m_cmdListResource.m_fence.Get();
	}

	void D3D12CopyContext::Prepare()
	{
		m_cmdListResource = D3D12CmdPool( D3D12_COMMAND_LIST_TYPE_COPY ).ObtainCommandList();
	}

	void D3D12CopyContext::RecordCopyCommand( D3D12Buffer& dest, D3D12Buffer& src, uint32 numByte )
	{
		assert( dest.Resource() != nullptr );
		assert( src.Resource() != nullptr );

		m_destResource = &dest;
		m_srcResource = &src;

		if ( ( numByte == 0 )
			|| ( dest.Size() == src.Size() ) )
		{
			CommandList()->CopyResource( dest.Resource(), src.Resource() );
		}
		else
		{
			CommandList()->CopyBufferRegion( dest.Resource(), 0, src.Resource(), 0, numByte );
		}

		CommandList()->Close();
	}

	void D3D12CopyContext::RecordCopyCommand( D3D12Texture& dest, D3D12Texture& src )
	{
		assert( dest.Resource() != nullptr );
		assert( src.Resource() != nullptr );

		m_destResource = &dest;
		m_srcResource = &src;

		CommandList()->CopyResource( static_cast<ID3D12Resource*>( dest.Resource() ), static_cast<ID3D12Resource*>( src.Resource() ) );

		CommandList()->Close();
	}

	bool D3D12CopyContext::IsFinished() const
	{
		return Fence()->GetCompletedValue() > 0;
	}

	ID3D12GraphicsCommandList6* D3D12CopyContext::CommandList() const
	{
		return m_cmdListResource.m_commandList.Get();
	}

	ID3D12Fence* D3D12CopyContext::Fence() const
	{
		return m_cmdListResource.m_fence.Get();
	}

	bool D3D12ResourceUploader::Initialize()
	{
		ID3D12Device& device = D3D12Device();

		D3D12_COMMAND_QUEUE_DESC desc{
			.Type = D3D12_COMMAND_LIST_TYPE_COPY,
			.Priority = 0,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};

		HRESULT hr = device.CreateCommandQueue( &desc, IID_PPV_ARGS( m_uploadQueue.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = device.CreateCommandQueue( &desc, IID_PPV_ARGS( m_copyQueue.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = device.CreateFence( m_uploadFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( m_uploadFence.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = device.CreateFence( m_copyFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( m_copyFence.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		++m_uploadFenceValue;

		m_uploadFenceEvent = CreateEvent( nullptr, false, false, _T( "Upload Fence Event" ) );
		if ( m_uploadFenceEvent == nullptr )
		{
			return false;
		}

		++m_copyFenceValue;

		m_copyFenceEvent = CreateEvent( nullptr, false, false, _T( "Copy Fence Event" ) );
		if ( m_copyFenceEvent == nullptr )
		{
			return false;
		}

		return true;
	}

	void D3D12ResourceUploader::Prepare()
	{
		D3D12UploadContext* pUploadContext = m_pendingListForUpload;
		while ( pUploadContext != nullptr )
		{
			if ( pUploadContext->IsFinished() )
			{
				D3D12UploadContext* deleteTarget = pUploadContext;
				pUploadContext = pUploadContext->m_next;

				SLinkedList::Remove( m_pendingListForUpload, deleteTarget );

				std::destroy_at( deleteTarget );
				m_uploadContextPool.Deallocate( deleteTarget );
			}
			else
			{
				pUploadContext = pUploadContext->m_next;
			}
		}

		if ( m_pendingListForUpload != nullptr )
		{
			uint64 fenceValue = m_uploadFenceValue;
			++m_uploadFenceValue;

			[[maybe_unused]] HRESULT hr = m_uploadQueue->Signal( m_uploadFence.Get(), fenceValue );
			assert( SUCCEEDED( hr ) );
		
			D3D12DirectCommandQueue().Wait( m_uploadFence.Get(), fenceValue );
		}

		D3D12CopyContext* pCopyContext = m_pendingListForCopy;
		while ( pCopyContext != nullptr )
		{
			if ( pCopyContext->IsFinished() )
			{
				D3D12CopyContext* deleteTarget = pCopyContext;
				pCopyContext = pCopyContext->m_next;

				SLinkedList::Remove( m_pendingListForCopy, deleteTarget );

				std::destroy_at( deleteTarget );
				m_copyContextPool.Deallocate( deleteTarget );
			}
			else
			{
				pCopyContext = pCopyContext->m_next;
			}
		}

		if ( m_pendingListForCopy != nullptr )
		{
			uint64 fenceValue = m_copyFenceValue;
			++m_copyFenceValue;

			[[maybe_unused]] HRESULT hr = m_uploadQueue->Signal( m_copyFence.Get(), fenceValue );
			assert( SUCCEEDED( hr ) );

			D3D12DirectCommandQueue().Wait( m_copyFence.Get(), fenceValue );
		}
	}

	void D3D12ResourceUploader::Upload( D3D12Buffer& dest, const void* data, uint32 destOffset, uint32 numByte )
	{
		D3D12UploadContext& context = *m_uploadContextPool.Allocate();
		std::construct_at( &context );

		SLinkedList::AddToHead( m_pendingListForUpload, &context );
		context.Prepare();
		context.RecordUploadCommand( dest, data, destOffset, numByte );

		ID3D12CommandList* commandList[] = { context.CommandList() };
		m_uploadQueue->ExecuteCommandLists( 1, commandList );
		m_uploadQueue->Signal( context.Fence(), 1 );
	}

	void D3D12ResourceUploader::Upload( D3D12Texture& dest, const void* data, uint32 srcRowSize, const CubeArea<uint32>* pDestArea, uint32 subresource )
	{
		D3D12UploadContext& context = *m_uploadContextPool.Allocate();
		std::construct_at( &context );

		SLinkedList::AddToHead( m_pendingListForUpload, &context );
		context.Prepare();
		context.RecordUploadCommand( dest, data, srcRowSize, pDestArea, subresource );

		ID3D12CommandList* commandList[] = { context.CommandList() };
		m_uploadQueue->ExecuteCommandLists( 1, commandList );
		m_uploadQueue->Signal( context.Fence(), 1 );
	}

	void D3D12ResourceUploader::Copy( D3D12Buffer& dest, D3D12Buffer& src, uint32 numByte )
	{
		D3D12CopyContext& context = *m_copyContextPool.Allocate();
		std::construct_at( &context );

		SLinkedList::AddToHead( m_pendingListForCopy, &context );
		context.Prepare();
		context.RecordCopyCommand( dest, src, numByte );

		ID3D12CommandList* commandList[] = { context.CommandList() };
		m_copyQueue->ExecuteCommandLists( 1, commandList );
		m_copyQueue->Signal( context.Fence(), 1 );

		if ( HasAnyFlags( dest.GetTrait().m_access, ResourceAccessFlag::CpuRead ) )
		{
			WaitUntilCopyCompleted();
		}
	}

	void D3D12ResourceUploader::Copy( D3D12Texture& dest, D3D12Texture& src )
	{
		D3D12CopyContext& context = *m_copyContextPool.Allocate();
		std::construct_at( &context );

		SLinkedList::AddToHead( m_pendingListForCopy, &context );
		context.Prepare();
		context.RecordCopyCommand( dest, src );

		ID3D12CommandList* commandList[] = { context.CommandList() };
		m_copyQueue->ExecuteCommandLists( 1, commandList );
		m_copyQueue->Signal( context.Fence(), 1);

		if ( HasAnyFlags( dest.GetTrait().m_access, ResourceAccessFlag::CpuRead ) )
		{
			WaitUntilCopyCompleted();
		}
	}

	void D3D12ResourceUploader::WaitUntilUploadCompleted()
	{
		uint64 fenceValue = m_uploadFenceValue;
		++m_uploadFenceValue;

		[[maybe_unused]] HRESULT hr = m_uploadQueue->Signal( m_uploadFence.Get(), fenceValue );
		assert( SUCCEEDED( hr ) );

		if ( m_uploadFence->GetCompletedValue() < fenceValue )
		{
			hr = m_uploadFence->SetEventOnCompletion( fenceValue, m_uploadFenceEvent );
			assert( SUCCEEDED( hr ) );
			WaitForSingleObject( m_uploadFenceEvent, INFINITE );
		}

		while ( m_pendingListForUpload != nullptr )
		{
			D3D12UploadContext& context = *m_pendingListForUpload;
			SLinkedList::Remove( m_pendingListForUpload, m_pendingListForUpload );
			std::destroy_at( &context );

			m_uploadContextPool.Deallocate( &context );
		}
	}

	void D3D12ResourceUploader::WaitUntilCopyCompleted()
	{
		uint64 fenceValue = m_copyFenceValue;
		++m_copyFenceValue;

		[[maybe_unused]] HRESULT hr = m_copyQueue->Signal( m_copyFence.Get(), fenceValue );
		assert( SUCCEEDED( hr ) );

		if ( m_copyFence->GetCompletedValue() < fenceValue )
		{
			hr = m_copyFence->SetEventOnCompletion( fenceValue, m_copyFenceEvent );
			assert( SUCCEEDED( hr ) );
			WaitForSingleObject( m_copyFenceEvent, INFINITE );
		}

		while ( m_pendingListForCopy != nullptr )
		{
			D3D12CopyContext& context = *m_pendingListForCopy;
			SLinkedList::Remove( m_pendingListForCopy, m_pendingListForCopy );
			std::destroy_at( &context );

			m_copyContextPool.Deallocate( &context );
		}
	}

	D3D12ResourceUploader::~D3D12ResourceUploader()
	{
		CloseHandle( m_uploadFenceEvent );
		CloseHandle( m_copyFenceEvent );
	}
}
