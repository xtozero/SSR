#pragma once

#include "BaseMesh.h"
#include "common.h"
#include "IMesh.h"
#include <map>
#include <memory>
#include <string>

class IMeshLoader;

class CMeshLoader
{
public:
	bool Initialize( );
	bool LoadMeshFromFile( const TCHAR* pfileName );
	std::shared_ptr<IMesh> GetMesh( const TCHAR* pfileName );
	void RegisterMesh( const String& pMeshName, const std::shared_ptr<IMesh>& pMesh );

	CMeshLoader( );
	virtual ~CMeshLoader( );

private:
	std::map<String, std::shared_ptr<IMesh>> m_meshList;
	std::map<String, std::unique_ptr<IMeshLoader>> m_meshLoaders;
};

