#pragma once

#include "common.h"
#include "Material/Material.h"
#include "MeshDescription.h"
#include "Model/BaseMesh.h"
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

	using LoadCompletionCallback = Delegate<void, void*>;

	ModelLoaderSharedHandle RequestAsyncLoad( const char* pFilePath, LoadCompletionCallback completionCallback );

	CModelBuilder& GetModelBuilder( ) { return m_meshBuilder; }

private:
	void PostMeshLoading( const std::string& path, void* asset );
	std::map<std::string, std::unique_ptr<BaseMesh>> m_modelList;

	CPlyMeshLoader m_plyLoader;
	CObjMeshLoader m_objLoader;

	std::map<std::string, IModelLoader&> m_meshLoaders = {
		{ std::string( "ply" ), m_plyLoader },
		{ std::string( "obj" ), m_objLoader }
	};

	CModelBuilder m_meshBuilder = CModelBuilder( *this );
};

