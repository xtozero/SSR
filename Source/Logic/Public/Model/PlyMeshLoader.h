#pragma once

#include "IModelLoader.h"

#include <memory>

struct MeshDescription;

class CPlyMeshLoader : public IModelLoader
{
public:
	virtual ModelLoaderSharedHandle RequestAsyncLoad( const char* pFileName, LoadCompletionCallback completionCallback ) override;
};

