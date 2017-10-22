#pragma once

#include "BaseMesh.h"
#include "IMesh.h"
#include "ModelBuilder.h"
#include "ObjMeshLoader.h"
#include "PlyMeshLoader.h"

#include "../common.h"
#include "../../shared/Util.h"

#include <map>
#include <memory>
#include <string>

class IModelLoader;
class IRenderer;
class CSurfaceManager;

class CModelManager
{
public:
	IMesh* LoadMeshFromFile( IRenderer& renderer, const TCHAR* pfileName );
	IMesh* FindModel( const String& modelName );
	void RegisterMesh( const String& modelName, const std::unique_ptr<IMesh> pMesh );

	CModelBuilder& GetModelBuilder( ) { return m_meshBuilder; }

private:
	std::map<String, std::unique_ptr<IMesh>> m_meshList;

	CPlyMeshLoader m_plyLoader;
	CObjMeshLoader m_objLoader;

	std::map<String, IModelLoader&> m_meshLoaders = {
		{ String( _T( "ply" ) ), m_plyLoader },
		{ String( _T( "obj" ) ), m_objLoader }
	};

	CModelBuilder m_meshBuilder = CModelBuilder( *this );

	SurfaceMap m_surfaces;
};

