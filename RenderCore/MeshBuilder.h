#pragma once

#include "IMeshBuilder.h"

class CMeshBuilder : public IMeshBuilder
{
public:
	virtual void Append( const MeshVertex& newVertex ) override;
	virtual void AppendIndex( const WORD index ) override;
	virtual void AppendTextureName( const String& textureName ) override;
	virtual IMesh* Build( IRenderer& renderer, const String& meshName, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) const override;

	virtual void Clear( ) override;

private:
	std::vector<MeshVertex> m_vertices;
	std::vector<WORD> m_indices;
	String m_textureName;
};

