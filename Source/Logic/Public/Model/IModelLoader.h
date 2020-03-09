#pragma once

#include "Surface.h"
#include "Util.h"

#include <memory>

class IMesh;
class IRenderer;

class IModelLoader
{
public:
	virtual Owner<IMesh*> LoadMeshFromFile( IRenderer& renderer, const TCHAR* pFileName, SurfaceMap& pSurfaceManager ) = 0;

	virtual ~IModelLoader( ) = default;

protected:
	IModelLoader( ) = default;
};