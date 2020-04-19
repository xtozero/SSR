#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;

ID3D11Device& D3D11Device( );
ID3D11DeviceContext& D3D11Context( );
void* GetD3D11GraphicsApi( );