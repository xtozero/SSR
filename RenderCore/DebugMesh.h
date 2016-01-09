#pragma once

#include "BaseMesh.h"
#include "common.h"

class TriangleMesh : public BaseMesh
{
public:
	virtual bool Load( ) override;

	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) override;

	TriangleMesh( );
};

