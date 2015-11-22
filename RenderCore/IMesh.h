#pragma once

#include "common.h"
#include <d3d11.h>

class RENDERCORE_DLL IMesh
{
public:
	virtual void SetModelData( void* pOrignal, UINT vertexCount ) = 0;
	virtual void SetIndexData( void* pOrignal, UINT indexCount ) = 0;
	virtual bool Load( ) = 0;

	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) = 0;

public:
	virtual ~IMesh( ) {};
};

