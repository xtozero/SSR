#pragma once

#include "CommonMeshDefine.h"
//#include "Render/Resource.h"

#include <memory>
#include <vector>

class IRenderer;
class IMesh;

class IModelBuilder
{
public:
	virtual void Append( const MeshVertex& newVertex ) = 0;
	virtual void AppendIndex( const WORD index ) = 0;
	virtual void AppendTextureName( const std::string& textureName ) = 0;
	//virtual IMesh* Build( IRenderer& renderer, const std::string& meshName, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) const = 0;

	virtual void Clear( ) = 0;

	virtual ~IModelBuilder( ) = default;

protected:
	IModelBuilder( ) = default;
};