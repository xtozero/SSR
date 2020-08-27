#include "stdafx.h"
#include "Model/BaseMesh.h"

#include "common.h"
#include "Model/CommonMeshDefine.h"
//#include "Render/IRenderer.h"

BaseMesh::~BaseMesh( )
{
	delete[] m_pMeshData;
}
