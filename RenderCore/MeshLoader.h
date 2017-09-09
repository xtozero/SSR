#pragma once

#include "BaseMesh.h"
#include "common.h"
#include "IMesh.h"

#include "../shared/Util.h"

#include <map>
#include <memory>
#include <string>

class IMeshLoader;
class IRenderer;
class CSurfaceManager;

class CMeshLoader
{
public:
	bool Initialize( );
	bool LoadMeshFromFile( IRenderer& renderer, const TCHAR* pfileName, CSurfaceManager* pSurfaceManager );
	IMesh* GetMesh( const TCHAR* pfileName );
	void RegisterMesh( const String& pMeshName, const Owner<IMesh*> pMesh );

	CMeshLoader( );
	virtual ~CMeshLoader( );

private:
	std::map<String, std::unique_ptr<IMesh>> m_meshList;
	std::map<String, std::unique_ptr<IMeshLoader>> m_meshLoaders;
};

