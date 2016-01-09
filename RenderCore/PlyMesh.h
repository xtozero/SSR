#pragma once

#include "BaseMesh.h"

class CPlyMesh : public BaseMesh
{
public:
	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) override;

	CPlyMesh( );
	virtual ~CPlyMesh( );
};

