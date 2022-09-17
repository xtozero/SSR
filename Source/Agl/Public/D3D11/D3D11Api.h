#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGIFactory1;

namespace agl
{
	ID3D11Device& D3D11Device( );
	ID3D11DeviceContext& D3D11Context( );
	IDXGIFactory1& D3D11Factory( );

	void CreateD3D11GraphicsApi( );
	void DestoryD3D11GraphicsApi( );
	void* GetD3D11GraphicsLibrary( );
}