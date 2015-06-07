#pragma once

#include "BaseMesh.h"
#include "common.h"

class RENDERCORE_DLL TriangleMesh : public BaseMesh
{
public:
	virtual bool Load( );

	TriangleMesh( );
};

