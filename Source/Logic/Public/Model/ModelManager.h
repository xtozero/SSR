#pragma once

#include "BaseMesh.h"
#include "common.h"
#include "IMesh.h"
#include "ModelBuilder.h"
#include "ObjMeshLoader.h"
#include "PlyMeshLoader.h"
#include "Scene/INotifyGraphicsDevice.h"
#include "Util.h"

#include <map>
#include <memory>
#include <string>

class IModelLoader;
class IRenderer;

class CModelManager : IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	IMesh* LoadMeshFromFile( IRenderer& renderer, const char* pfileName );
	IMesh* FindModel( const std::string& modelName );
	void RegisterMesh( const std::string& modelName, const std::unique_ptr<IMesh> pMesh );

	CModelBuilder& GetModelBuilder( ) { return m_meshBuilder; }

private:
	std::map<std::string, std::unique_ptr<IMesh>> m_meshList;

	CPlyMeshLoader m_plyLoader;
	CObjMeshLoader m_objLoader;

	std::map<std::string, IModelLoader&> m_meshLoaders = {
		{ std::string( "ply" ), m_plyLoader },
		{ std::string( "obj" ), m_objLoader }
	};

	CModelBuilder m_meshBuilder = CModelBuilder( *this );

	SurfaceMap m_surfaces;
};

