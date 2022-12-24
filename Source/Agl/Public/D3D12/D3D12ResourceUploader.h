#pragma once

#include "D3D12Buffer.h"
#include "FixedBlockMemoryPool.h"
#include "RefHandle.h"

#include <d3d12.h>
#include <wrl/client.h>

namespace agl
{
	class D3D12ResourceUploader;

	class D3D12UploadContext
	{
	public:
		void Prepare();
		void RecordUploadCommand( D3D12Buffer& dest, const void* data, size_t size );

		explicit D3D12UploadContext( D3D12ResourceUploader& uploader ) noexcept
			: m_uploader( &uploader ) {}
		~D3D12UploadContext();

		D3D12UploadContext* m_next = nullptr;

	private:
		friend D3D12ResourceUploader;

		D3D12ResourceUploader* m_uploader;

		AllocatedResourceInfo m_srcResourceInfo;
		RefHandle<GraphicsApiResource> m_destResource;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_uploadCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_uploadCommandAllocator;
	};

	class D3D12ResourceUploader
	{
	public:
		bool Initialize();
		void Upload( D3D12Buffer& dest, const void* data, size_t size );
		void WaitUntilCopyCompleted();

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_uploadQueue;
		
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		uint64 m_fenceValue = 0;
		HANDLE m_fenceEvent = nullptr;

		FixedBlockMemoryPool<D3D12UploadContext> m_contextPool;

		D3D12UploadContext* m_pendingList = nullptr;
	};
}
