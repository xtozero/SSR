#pragma once

#include <d3d11.h>

class IShader;

namespace
{
	enum type
	{
		VS = 0,
		HS,
		DS,
		GS,
		PS,
		CS,
		MAX_SHADER
	};
}

class Material
{
private:
	IShader* m_pShaders[MAX_SHADER];

public:
	virtual void Init( );
	void SetShader( ID3D11DeviceContext* pDeviceContext );

	void Draw( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT vertexOffset = 0 );
	void DrawIndexed( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT indexOffset = 0, const UINT vertexOffset = 0 );
	void DrawInstanced( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT instanceCount, const UINT vertexOffset = 0, const UINT instanceOffset = 0 );
	void DrawInstancedInstanced( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT instanceCount, const UINT indexOffset = 0, const UINT vertexOffset = 0, const UINT instanceOffset = 0 );
	void DrawAuto( ID3D11DeviceContext* pDeviceContext );

	Material( );
	virtual ~Material( );
};

