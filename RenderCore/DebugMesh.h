#pragma once

#include "BaseMesh.h"
#include "common.h"

class TriangleMesh : public BaseMesh
{
public:
	virtual bool Load( );

	virtual void Draw( ID3D11DeviceContext* pDeviceContext );

	TriangleMesh( );
};

