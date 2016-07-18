#pragma once

#include "CommonMeshDefine.h"

#include <memory>
#include <vector>
#include <d3dcommon.h>

class IMesh;
class IRenderer;

class IMeshBuilder
{
public:
	virtual void Append( const MeshVertex& newVertex ) = 0;
	virtual void AppendIndex( const WORD index ) = 0;
	virtual void AppendTextureName( const String& textureName ) = 0;
	virtual std::shared_ptr<IMesh> Build( IRenderer& renderer, const String& meshName, D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) const = 0;

	virtual void Clear( ) = 0;

	virtual ~IMeshBuilder( ) = default;

protected:
	IMeshBuilder( ) = default;
};