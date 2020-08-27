#pragma once

#include "CommonMeshDefine.h"
#include "IModelLoader.h"
#include "Material/Material.h"
#include "Model/MeshDescription.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

struct ObjMeshLoadContext
{
	std::size_t m_contextID = 0;

	std::string m_meshDirectory;

	MeshDescription m_meshDescription;

	std::vector<Material> m_meshMaterials;
	std::vector<std::string> m_mtlFileNames;

	std::size_t m_loadingMaterialIndex = 0;

	IModelLoader::LoadCompletionCallback m_completionCallback;
};

class CObjMeshLoader : public IModelLoader
{
public:
	virtual Owner<MeshDescription*> RequestAsyncLoad( const char* pFilePath, LoadCompletionCallback completionCallback ) override;

private:
	void LoadMaterialFromFile( ObjMeshLoadContext* pContext );
	void ParseMtl( ObjMeshLoadContext* pContext, char* buffer, unsigned long bufferSize );
	void ParseObjMesh( ObjMeshLoadContext* pContext, char* buffer, unsigned long bufferSize );

	void OnLoadSuccessed( ObjMeshLoadContext* pContext );
	void OnLoadFailed( ObjMeshLoadContext* pContext );

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

