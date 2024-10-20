#pragma once

#include "IModelBuilder.h"

class CModelManager;

class CModelBuilder : public IModelBuilder
{
public:
	virtual void Append( const MeshVertex& newVertex ) override;
	virtual void AppendIndex( const WORD index ) override;
	virtual void AppendTextureName( const String& textureName ) override;
	virtual IMesh* Build( IRenderer& renderer, const String& meshName, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) const override;

	virtual void Clear( ) override;

	CModelBuilder( CModelManager& modelManager ) : m_modelManager( modelManager ) {}

private:
	std::vector<MeshVertex> m_vertices;
	std::vector<WORD> m_indices;
	String m_textureName;

	CModelManager& m_modelManager;
};

