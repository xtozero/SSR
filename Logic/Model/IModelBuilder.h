#pragma once

#include "CommonMeshDefine.h"

#include "../../RenderCore/CommonRenderer/Resource.h"

#include <memory>
#include <vector>

class IRenderer;
class IMesh;

class IModelBuilder
{
public:
	virtual void Append( const MeshVertex& newVertex ) = 0;
	virtual void AppendIndex( const WORD index ) = 0;
	virtual void AppendTextureName( const String& textureName ) = 0;
	virtual IMesh* Build( IRenderer& renderer, const String& meshName, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) const = 0;

	virtual void Clear( ) = 0;

	virtual ~IModelBuilder( ) = default;

protected:
	IModelBuilder( ) = default;
};