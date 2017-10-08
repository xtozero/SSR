#include "stdafx.h"
#include "BaseMesh.h"
#include "CommonRenderer/IRenderer.h"
#include "IMesh.h"
#include "Material.h"

#include "MeshBuilder.h"

#include "../Shared/Util.h"

class CMeshBuilderMesh : public BaseMesh
{
public:
	virtual bool Load( IRenderer& renderer, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) override;
	virtual void Draw( IRenderer& renderer ) override;

	virtual void SetTexture( IRenderResource* pTexture ) override;
	virtual IRenderResource* GetTexture( ) const override { return m_pTexture; };

	void SetTextureName( const String& textureName ) { m_textureName = textureName; }

private:
	IRenderResource* m_pTexture;
	String m_textureName;
};

bool CMeshBuilderMesh::Load( IRenderer& renderer, UINT primitive )
{
	bool loadSuccess = BaseMesh::Load( renderer, primitive );

	if ( m_textureName.size( ) > 0 )
	{
		auto texture = renderer.GetShaderResourceFromFile( m_textureName );

		if ( texture )
		{
			m_pTexture = texture;
		}
	}

	return loadSuccess;
}

void CMeshBuilderMesh::Draw( IRenderer& renderer )
{
	if ( !m_pMaterial )
	{
		return;
	}

	if ( !m_pVertexBuffer )
	{
		return;
	}

	m_pMaterial->SetShader( );
	m_pVertexBuffer->SetVertexBuffer( &m_stride, &m_nOffset );
	m_pMaterial->SetTexture( SHADER_TYPE::PS, 0, m_pTexture );
	if ( m_pIndexBuffer )
	{
		m_pIndexBuffer->SetIndexBuffer( sizeof( WORD ), m_nIndexOffset );
		renderer.DrawIndexed( m_primitiveTopology, m_nIndices, m_nIndexOffset, m_nOffset );
	}
	else
	{
		renderer.Draw( m_primitiveTopology, m_nVertices, m_nOffset );
	}
}

void CMeshBuilderMesh::SetTexture( IRenderResource* pTexture )
{
	m_pTexture = pTexture;
}

void CMeshBuilder::Append( const MeshVertex& newVertex )
{
	m_vertices.emplace_back( newVertex );
}

void CMeshBuilder::AppendIndex( const WORD index )
{
	m_indices.push_back( index );
}

void CMeshBuilder::AppendTextureName( const String& textureName )
{
	m_textureName = textureName;
}

IMesh* CMeshBuilder::Build( IRenderer& renderer, const String& meshName, UINT primitive ) const
{
	if ( IMesh* found = renderer.GetModelPtr( meshName.c_str( ) ) )
	{
		return found;
	}

	if ( m_vertices.size( ) == 0 )
	{
		DebugWarning( "CMeshBuilder Build Mesh From Empty Stream\n" );
		return nullptr;
	}

	std::unique_ptr<CMeshBuilderMesh> newMesh = std::make_unique<CMeshBuilderMesh>();

	UINT vertexCount = m_vertices.size( );
	MeshVertex* vertices = new MeshVertex[vertexCount];

	for ( UINT i = 0; i < vertexCount; ++i )
	{
		vertices[i] = m_vertices[i];
	}

	newMesh->SetModelData( vertices, vertexCount );

	UINT indexCount = m_indices.size( );
	WORD* indices = new WORD[indexCount];

	for ( UINT i = 0; i < indexCount; ++i )
	{
		indices[i] = m_indices[i];
	}

	newMesh->SetIndexData( indices, indexCount );

	if ( m_textureName.size( ) > 0 )
	{
		newMesh->SetTextureName( m_textureName );
	}

	if ( newMesh->Load( renderer, primitive ) )
	{
		CMeshBuilderMesh* ret = newMesh.get( );
		renderer.SetModelPtr( meshName, std::move( newMesh ) );
		return ret;
	}
	else
	{
		DebugWarning( "CMeshBuilder Build Fail - %s\n", meshName.c_str() );
		return nullptr;
	}
}

void CMeshBuilder::Clear( )
{
	m_vertices.clear( );
	m_indices.clear( );
}