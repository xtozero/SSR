#pragma once

#include "common.h"
#include "CommonMeshDefine.h"
#include "Math/CXMFloat.h"
#include "Util.h"

#include <memory>

class BaseMesh
{
public:
	~BaseMesh( );

protected:
	MeshData* m_pMeshData = nullptr;
};
