#pragma once

#include "BaseMesh.h"

class ISurface;

class CPlyMesh : public BaseMesh
{
public:
	virtual void Draw( IRenderer& renderer ) override;

	void SetSurface( ISurface* pSurface ) noexcept { m_pSurface = pSurface; }

	CPlyMesh( );
private:

	ISurface* m_pSurface;
};

