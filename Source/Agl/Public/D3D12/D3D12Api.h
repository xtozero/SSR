#pragma once

#include "GuideTypes.h"

struct ID3D12Device;
struct IDXGIFactory1;

namespace agl
{
	class IAgl;

	ID3D12Device& D3D12Device();
	IDXGIFactory1& D3D11Factory();

	Owner<IAgl*> CreateD3D12GraphicsApi();
}