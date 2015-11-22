#pragma once

#include "BaseMesh.h"
#include "common.h"
#include "IMesh.h"
#include <map>
#include <memory>
#include <string>

class CMeshLoader
{
public:
	bool LoadMeshFromFile( const TCHAR* pfileName );
	std::shared_ptr<IMesh> GetMesh( const TCHAR* pfileName );

	CMeshLoader( );
	virtual ~CMeshLoader( );

private:
	std::map<String, std::shared_ptr<IMesh>> m_meshList;
};

