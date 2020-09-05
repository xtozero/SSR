#pragma once

#include "CommonMeshDefine.h"
#include "IModelLoader.h"
#include "Material/Material.h"
#include "Model/MeshDescription.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class CObjMeshLoader : public IModelLoader
{
public:
	virtual ModelLoaderSharedHandle RequestAsyncLoad( const char* pFilePath, LoadCompletionCallback completionCallback ) override;

private:
	struct ObjMeshLoadContext
	{
		std::size_t m_contextID = 0;

		std::string m_meshDirectory;

		MeshDescription m_meshDescription;

		std::vector<Material> m_meshMaterials;
		std::vector<std::string> m_mtlFileNames;

		std::size_t m_loadingMaterialIndex = 0;

		std::shared_ptr<ModelLoaderHandle> m_handle;
	};

	void LoadMaterialFromFile( ObjMeshLoadContext& context );
	void ParseMtl( ObjMeshLoadContext& context, char* buffer, unsigned long bufferSize );
	void ParseObjMesh( ObjMeshLoadContext& context, char* buffer, unsigned long bufferSize );

	void OnLoadSuccessed( ObjMeshLoadContext& context );
	void OnLoadFailed( ObjMeshLoadContext& context );

	std::shared_ptr<ModelLoaderHandle> CreateHandle( );

	// std::vector<MeshVertex> BuildVertices( );
	// void CalcObjNormal( );

	enum OBJ_FACE_ELEMENT
	{
		VERTEX = 0,
		TEXTURE,
		NORMAL,
	};

	std::map<std::size_t, ObjMeshLoadContext> m_contexts;
	std::size_t m_lastContextID = 0;
};
