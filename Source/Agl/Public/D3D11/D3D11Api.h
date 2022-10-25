#pragma once

#include "GuideTypes.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGIFactory1;

namespace agl
{
	class IAgl;

	ID3D11Device& D3D11Device();
	ID3D11DeviceContext& D3D11Context();
	IDXGIFactory1& D3D11Factory();

	Owner<IAgl*> CreateD3D11GraphicsApi();
}