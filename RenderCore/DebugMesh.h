#pragma once

#include "BaseMesh.h"
#include "common.h"

class TriangleMesh : public BaseMesh
{
public:
	virtual bool Load( D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) override;

	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) override;

	TriangleMesh( );
};

