#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGIFactory1;

ID3D11Device& D3D11Device( );
ID3D11DeviceContext& D3D11Context( );
IDXGIFactory1& D3D11Factory( );
void* GetD3D11GraphicsApi( );