#pragma once

#include "IModelLoader.h"

#include <memory>

class IMesh;

class CPlyMeshLoader : public IModelLoader
{
public:
	virtual Owner<IMesh*> LoadMeshFromFile( IRenderer& renderer, const char* pFileName, SurfaceMap& surface ) override;
};

