#pragma once

#include "BaseMesh.h"
#include <map>
#include <memory>
#include <string>

#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

class CMeshLoader
{
public:
	bool LoadMeshFromFile( const TCHAR* pfileName );

	CMeshLoader( );
	virtual ~CMeshLoader( );

private:
	std::map<tstring, std::shared_ptr<BaseMesh>> m_meshList;
};

