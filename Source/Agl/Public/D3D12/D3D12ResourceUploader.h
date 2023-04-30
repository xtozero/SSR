#pragma once

#include "D3D12BaseTexture.h"
#include "D3D12Buffer.h"
#include "D3D12CommandList.h"

#include "FixedBlockMemoryPool.h"

#include "RefHandle.h"

#include <d3d12.h>
#include <wrl/client.h>

namespace agl
{
	class D3D12ResourceUploader;

	class D3D12UploadContext final
	{
	public:
		void Prepare();

		void RecordUploadCommand( D3D12Buffer& dest, const void* data, uint32 destOffset, uint32 numByte );
		void RecordUploadCommand( D3D12Texture& dest, const void* data, uint32 srcRowSize, const CubeArea<uint32>* pDestArea, uint32 subresource );

		bool IsFinished() const;

		~D3D12UploadContext();

		D3D12UploadContext* m_next = nullptr;

	private:
		ID3D12GraphicsCommandList6* CommandList() const;
		ID3D12Fence* Fence() const;

		friend D3D12ResourceUploader;

		AllocatedResourceInfo m_srcResourceInfo;
		RefHandle<GraphicsApiResource> m_destResource;

		D3D12CommandListResource m_cmdListResource;
	};

	class D3D12CopyContext final
	{
	public:
		void Prepare();

		void RecordCopyCommand( D3D12Buffer& dest, D3D12Buffer& src, uint32 numByte );
		void RecordCopyCommand( D3D12Texture& dest, D3D12Texture& src );

		bool IsFinished() const;

		~D3D12CopyContext() = default;

		D3D12CopyContext* m_next = nullptr;

	private:
		ID3D12GraphicsCommandList6* CommandList() const;
		ID3D12Fence* Fence() const;

		friend D3D12ResourceUploader;

		RefHandle<GraphicsApiResource> m_destResource;
		RefHandle<GraphicsApiResource> m_srcResource;

		D3D12CommandListResource m_cmdListResource;
	};

	class D3D12ResourceUploader final
	{
	public:
		bool Initialize();
		void Prepare();

		void Upload( D3D12Buffer& dest, const void* data, uint32 destOffset = 0, uint32 numByte = 0 );
		void Upload( D3D12Texture& dest, const void* data, uint32 srcRowSize, const CubeArea<uint32>* pDestArea = nullptr, uint32 subresource = 0 );
		void Copy( D3D12Buffer& dest, D3D12Buffer& src, uint32 numByte = 0 );
		void Copy( D3D12Texture& dest, D3D12Texture& src );

		void WaitUntilUploadCompleted();
		void WaitUntilCopyCompleted();

		~D3D12ResourceUploader();

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_uploadQueue;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_copyQueue;

		Microsoft::WRL::ComPtr<ID3D12Fence> m_uploadFence;
		uint64 m_uploadFenceValue = 0;
		HANDLE m_uploadFenceEvent = nullptr;
		
		Microsoft::WRL::ComPtr<ID3D12Fence> m_copyFence;
		uint64 m_copyFenceValue = 0;
		HANDLE m_copyFenceEvent = nullptr;

		FixedBlockMemoryPool<D3D12UploadContext> m_uploadContextPool;
		FixedBlockMemoryPool<D3D12CopyContext> m_copyContextPool;

		D3D12UploadContext* m_pendingListForUpload = nullptr;
		D3D12CopyContext* m_pendingListForCopy = nullptr;
	};
}
