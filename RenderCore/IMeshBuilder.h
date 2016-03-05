#pragma once

#include "CommonMeshDefine.h"

#include <memory>
#include <vector>

class IMesh;

class IMeshBuilder
{
public:
	virtual void Append( const MeshVertex& newVertex ) = 0;
	virtual void AppendIndex( const WORD index ) = 0;
	virtual void AppendTextureName( const String& textureName ) = 0;
	virtual std::shared_ptr<IMesh> Build( D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) const = 0;

	virtual void Clear( ) = 0;

	virtual ~IMeshBuilder( ) = default;

protected:
	IMeshBuilder( ) = default;
};