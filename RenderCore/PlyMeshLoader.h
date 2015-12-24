#pragma once
#include "IMesh.h"
#include <memory>

class CPlyMeshLoader
{
public:
	static std::shared_ptr<IMesh> LoadMeshFromFile( const TCHAR* pFileName );

	CPlyMeshLoader( );
	~CPlyMeshLoader( );
};

