#pragma once
#include "IMeshLoader.h"
#include <memory>

class IMesh;

class CPlyMeshLoader : public IMeshLoader
{
public:
	virtual Owner<IMesh*> LoadMeshFromFile( IRenderer& renderer, const TCHAR* pFileName, CSurfaceManager* ) override;
};

