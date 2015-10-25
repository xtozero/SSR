#pragma once

#include "BaseMesh.h"

class CPlyMesh : public BaseMesh
{
public:
	virtual bool Load( );

	virtual void Draw( ID3D11DeviceContext* pDeviceContext );

	CPlyMesh( );
	virtual ~CPlyMesh( );

private:

};

