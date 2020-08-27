#pragma once

#include "Delegate.h"
#include "Surface.h"
#include "Util.h"

#include <string>

struct MeshDescription;
class Material;

class IModelLoader
{
public:
	using LoadCompletionCallback = Delegate<void, MeshDescription&&, std::vector<Material>&&>;

	virtual Owner<MeshDescription*> RequestAsyncLoad( const char* pFilePath, LoadCompletionCallback completionCallback ) = 0;

	virtual ~IModelLoader( ) = default;

protected:
	IModelLoader( ) = default;
};