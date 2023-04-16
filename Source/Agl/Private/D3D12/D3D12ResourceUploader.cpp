#include "D3D12ResourceUploader.h"

#include "D3D12Api.h"

#include <cassert>
#include <tchar.h>

namespace agl
{
	void D3D12UploadContext::Prepare()
	{
		D3D12Device().CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS( m_uploadCommandAllocator.GetAddressOf() ) );

		D3D12Device().CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_COPY, m_uploadCommandAllocator.Get(), nullptr, IID_PPV_ARGS( m_uploadCommandList.GetAddressOf() ) );
	}

	void D3D12UploadContext::RecordUploadCommand( D3D12Buffer& dest, const void* data, size_t size )
	{
		assert( m_destResource.Get() == nullptr );

		m_destResource = &dest;

		D3D12HeapProperties heapProperties = {
			.m_alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.m_heapType = D3D12_HEAP_TYPE_UPLOAD,
			.m_heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS
		};

		D3D12_RESOURCE_DESC desc = {
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Width = dest.Size(),
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

		std::memcpy( mappedData, data, size );

		resource->Unmap( 0, nullptr );

		m_uploadCommandList->CopyBufferRegion( dest.Resource(), 0, resource, 0, size);
		m_uploadCommandList->Close();
	}

	D3D12UploadContext::D3D12UploadContext( D3D12ResourceUploader& uploader ) noexcept
		: m_uploader( &uploader )
		, m_fenceValue( uploader.FenceValue() )
	{
	}

	D3D12UploadContext::~D3D12UploadContext()
	{
		m_srcResourceInfo.Release();
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

		hr = device.CreateFence( m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( m_fence.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		++m_fenceValue;

		m_fenceEvent = CreateEvent( nullptr, false, false, _T( "Uploader Fence Event" ) );
		if ( m_fenceEvent == nullptr )
		{
			return false;
		}

		return true;
	}

	void D3D12ResourceUploader::Prepare()
	{
		if ( m_pendingList != nullptr )
		{
			uint64 fenceValue = m_fenceValue;
			++m_fenceValue;

			[[maybe_unused]] HRESULT hr = m_uploadQueue->Signal( m_fence.Get(), fenceValue );
			assert( SUCCEEDED( hr ) );
		
			D3D12DirectCommandQueue().Wait( m_fence.Get(), fenceValue );

			uint64 completedValue = m_fence->GetCompletedValue();
			D3D12UploadContext* pContext = m_pendingList;

			while ( pContext != nullptr )
			{
				if ( pContext->m_fenceValue >= completedValue )
				{
					D3D12UploadContext* deleteTarget = pContext;
					pContext = pContext->m_next;

					SLinkedList::Remove( m_pendingList, deleteTarget );
					
					std::destroy_at( deleteTarget );
					m_contextPool.Deallocate( deleteTarget );
				}
				else
				{
					pContext = pContext->m_next;
				}
			}
		}
	}

	void D3D12ResourceUploader::Upload( D3D12Buffer& dest, const void* data, size_t size )
	{
		D3D12UploadContext& context = *m_contextPool.Allocate();
		std::construct_at( &context, *this );

		SLinkedList::AddToHead( m_pendingList, &context );
		context.Prepare();
		context.RecordUploadCommand( dest, data, size);

		ID3D12CommandList* commandList[] = { context.m_uploadCommandList.Get() };
		m_uploadQueue->ExecuteCommandLists( 1, commandList );
	}

	void D3D12ResourceUploader::WaitUntilCopyCompleted()
	{
		uint64 fenceValue = m_fenceValue;
		++m_fenceValue;

		[[maybe_unused]] HRESULT hr = m_uploadQueue->Signal( m_fence.Get(), fenceValue );
		assert( SUCCEEDED( hr ) );

		if ( m_fence->GetCompletedValue() < fenceValue )
		{
			hr = m_fence->SetEventOnCompletion( fenceValue, m_fenceEvent );
			assert( SUCCEEDED( hr ) );
			WaitForSingleObject( m_fenceEvent, INFINITE );
		}

		while ( m_pendingList != nullptr )
		{
			D3D12UploadContext& context = *m_pendingList;
			SLinkedList::Remove( m_pendingList, m_pendingList );
			std::destroy_at( &context );

			m_contextPool.Deallocate( &context );
		}
	}

	uint64 D3D12ResourceUploader::FenceValue() const
	{
		return m_fenceValue;
	}

	D3D12ResourceUploader::~D3D12ResourceUploader()
	{
		CloseHandle( m_fenceEvent );
	}
}
