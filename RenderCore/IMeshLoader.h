#pragma once

#include "../shared/Util.h"

#include <memory>

class IMesh;
class IRenderer;
class CSurfaceManager;

class IMeshLoader
{
public:
	virtual Owner<IMesh*> LoadMeshFromFile( IRenderer& renderer, const TCHAR* pFileName, CSurfaceManager* pSurfaceManager ) = 0;

	virtual ~IMeshLoader( ) = default;

protected:
	IMeshLoader( ) = default;
};