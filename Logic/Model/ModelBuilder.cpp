#include "stdafx.h"
#include "ModelBuilder.h"

#include "BaseMesh.h"
#include "IMesh.h"
#include "ModelManager.h"
#include "../GameLogic.h"
#include "../../RenderCore/CommonRenderer/IMaterial.h"
#include "../../RenderCore/CommonRenderer/IRenderer.h"

#include "../../Shared/Util.h"

class CMeshBuilderMesh : public BaseMesh
{
public:
	virtual bool Load( IRenderer& renderer, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) override;
	virtual void Draw( CGameLogic& gameLogic ) override;

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

void CMeshBuilderMesh::Draw( CGameLogic& gameLogic )
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
		gameLogic.GetRenderer().DrawIndexed( m_primitiveTopology, m_nIndices, m_nIndexOffset, m_nOffset );
	}
	else
	{
		gameLogic.GetRenderer( ).Draw( m_primitiveTopology, m_nVertices, m_nOffset );
	}
}

void CMeshBuilderMesh::SetTexture( IRenderResource* pTexture )
{
	m_pTexture = pTexture;
}

void CModelBuilder::Append( const MeshVertex& newVertex )
{
	m_vertices.emplace_back( newVertex );
}

void CModelBuilder::AppendIndex( const WORD index )
{
	m_indices.push_back( index );
}

void CModelBuilder::AppendTextureName( const String& textureName )
{
	m_textureName = textureName;
}

IMesh* CModelBuilder::Build( IRenderer& renderer, const String& meshName, UINT primitive ) const
{
	if ( IMesh* found = m_modelManager.FindModel( meshName.c_str( ) ) )
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
		m_modelManager.RegisterMesh( meshName, std::move( newMesh ) );
		return ret;
	}
	else
	{
		DebugWarning( "CMeshBuilder Build Fail - %s\n", meshName.c_str() );
		return nullptr;
	}
}

void CModelBuilder::Clear( )
{
	m_vertices.clear( );
	m_indices.clear( );
}