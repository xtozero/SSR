#pragma once

class IMaterial
{
public:
	virtual void Init( ) = 0;
	virtual void SetShader( ID3D11DeviceContext* pDeviceContext ) = 0;
	virtual void Draw( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT vertexOffset = 0 ) = 0;

	virtual void SetPrimitiveTopology( ID3D11DeviceContext* pDeviceContext, D3D11_PRIMITIVE_TOPOLOGY primtopology ) = 0;
};