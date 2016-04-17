#pragma once

#include <memory>

class IMesh;
class CSurfaceManager;

class IMeshLoader
{
public:
	virtual std::shared_ptr<IMesh> LoadMeshFromFile( const TCHAR* pFileName, CSurfaceManager* pSurfaceManager ) = 0;

	virtual ~IMeshLoader( ) = default;

protected:
	IMeshLoader( ) = default;
};