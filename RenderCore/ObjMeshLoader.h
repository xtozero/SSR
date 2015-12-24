#pragma once

#include <memory>

class IMesh;

class CObjMeshLoader
{
public:
	static std::shared_ptr<IMesh> LoadMeshFromFile( const TCHAR* pFileName );

	CObjMeshLoader( );
	~CObjMeshLoader( );
};

