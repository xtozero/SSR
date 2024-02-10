#pragma once

#include "GuideTypes.h"

#include <d3d12.h>

struct ID3D12CommandQueue;
struct ID3D12Device;
struct IDXGIFactory7;

namespace agl
{
	class D3D12BindlessManager;
	class D3D12CommnadListResourcePool;
	class D3D12QueryAllocator;
	class D3D12ResourceAllocator;
	class D3D12ResourceUploader;
	class IAgl;

	ID3D12CommandQueue& D3D12DirectCommandQueue();
	ID3D12Device& D3D12Device();
	IDXGIFactory7& D3D12Factory();

	D3D12ResourceAllocator& D3D12Allocator();
	D3D12QueryAllocator& D3D12AllocatorForQuery();
	D3D12CommnadListResourcePool& D3D12CmdPool( D3D12_COMMAND_LIST_TYPE type );
	D3D12ResourceUploader& D3D12Uploader();
	D3D12BindlessManager& D3D12BindlessMgr();

	Owner<IAgl*> CreateD3D12GraphicsApi();

	uint32 GetFrameIndex();
}