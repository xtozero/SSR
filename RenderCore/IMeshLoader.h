#pragma once

#include <memory>

class IMesh;
class IRenderer;
class CSurfaceManager;

class IMeshLoader
{
public:
	virtual std::shared_ptr<IMesh> LoadMeshFromFile( IRenderer& renderer, const TCHAR* pFileName, CSurfaceManager* pSurfaceManager ) = 0;

	virtual ~IMeshLoader( ) = default;

protected:
	IMeshLoader( ) = default;
};