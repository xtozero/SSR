#pragma once

#include "BaseMesh.h"

class ISurface;

class CPlyMesh : public BaseMesh
{
public:
	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) override;

	void SetSurface( ISurface* pSurface ) noexcept { m_pSurface = pSurface; }

	CPlyMesh( );
private:

	ISurface* m_pSurface;
};

