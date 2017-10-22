#pragma once
#include "IModelLoader.h"
#include <memory>

class IMesh;

class CPlyMeshLoader : public IModelLoader
{
public:
	virtual Owner<IMesh*> LoadMeshFromFile( IRenderer& renderer, const TCHAR* pFileName, SurfaceMap& surface ) override;
};

