#pragma once

#include "GuideTypes.h"

struct ID3D12CommandQueue;
struct ID3D12Device;
struct IDXGIFactory7;

namespace agl
{
	class IAgl;

	ID3D12CommandQueue& D3D12DirectCommandQueue();
	ID3D12Device& D3D12Device();
	IDXGIFactory7& D3D12Factory();

	Owner<IAgl*> CreateD3D12GraphicsApi();
}