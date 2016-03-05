#pragma once

#include <memory>

class IMesh;

class IMeshLoader
{
public:
	virtual std::shared_ptr<IMesh> LoadMeshFromFile( const TCHAR* pFileName ) = 0;

	virtual ~IMeshLoader( ) = default;

protected:
	IMeshLoader( ) = default;
};