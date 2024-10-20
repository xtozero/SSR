#include "stdafx.h"
#include "Model/ModelBuilder.h"

#include "Core/GameLogic.h"
#include "Model/BaseMesh.h"
#include "Model/IMesh.h"
#include "Model/ModelManager.h"
#include "Render/IRenderer.h"
#include "Util.h"

class CMeshBuilderMesh : public BaseMesh
{
public:
	virtual bool Load( IRenderer& renderer, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) override;
	virtual void Draw( CGameLogic& gameLogic ) override;

	virtual void SetTexture( RE_HANDLE pTexture ) override;
	virtual RE_HANDLE GetTexture( ) const override { return m_texture; };

	void SetTextureName( const String& textureName ) { m_textureName = textureName; }

private:
	RE_HANDLE m_texture;
	String m_textureName;
};

bool CMeshBuilderMesh::Load( IRenderer& renderer, UINT primitive )
{
	bool loadSuccess = BaseMesh::Load( renderer, primitive );

	if ( m_textureName.size( ) > 0 )
	{
		m_texture = renderer.CreateShaderResourceFromFile( m_textureName );
	}

	return loadSuccess;
}

void CMeshBuilderMesh::Draw( CGameLogic& gameLogic )
{
	if ( m_material == INVALID_MATERIAL )
	{
		return;
	}

	if ( m_vertexBuffer == RE_HANDLE::InValidHandle( ) )
	{
		return;
	}

	IRenderer& renderer = gameLogic.GetRenderer( );

	renderer.BindMaterial( m_material );
	renderer.BindVertexBuffer( &m_vertexBuffer, 0, 1, &m_stride, &m_offset );
	renderer.BindShaderResource( SHADER_TYPE::PS, 0, 1, &m_texture );
	if ( m_indexBuffer == RE_HANDLE::InValidHandle( ) )
	{
		gameLogic.GetRenderer( ).Draw( m_primitiveTopology, m_nVertices, m_offset );
	}
	else
	{
		gameLogic.GetRenderer( ).BindIndexBuffer( m_indexBuffer, m_nIndexOffset );
		gameLogic.GetRenderer( ).DrawIndexed( m_primitiveTopology, m_nIndices, m_nIndexOffset, m_offset );
	}
}

void CMeshBuilderMesh::SetTexture( RE_HANDLE texture )
{
	m_texture = texture;
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

	assert( m_vertices.size( ) <= UINT_MAX );
	UINT vertexCount = static_cast<UINT>( m_vertices.size( ) );
	MeshVertex* vertices = new MeshVertex[vertexCount];

	for ( UINT i = 0; i < vertexCount; ++i )
	{
		vertices[i] = m_vertices[i];
	}

	newMesh->SetModelData( vertices, vertexCount );

	assert( m_indices.size( ) <= UINT_MAX );
	UINT indexCount = static_cast<UINT>( m_indices.size( ) );
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