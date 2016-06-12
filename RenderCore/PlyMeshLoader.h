#pragma once
#include "IMeshLoader.h"
#include <memory>

class IMesh;

class CPlyMeshLoader : public IMeshLoader
{
public:
	virtual std::shared_ptr<IMesh> LoadMeshFromFile( const TCHAR* pFileName, CSurfaceManager* ) override;
};

