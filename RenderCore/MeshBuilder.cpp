#include "stdafx.h"
#include "BaseMesh.h"
#include "IMesh.h"
#include "IRenderer.h"
#include "Material.h"

#include "MeshBuilder.h"

#include "../Shared/Util.h"

class CMeshBuilderMesh : public BaseMesh
{
public:
	virtual bool Load( IRenderer& renderer, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) override;
	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) override;

	virtual void SetTexture( IShaderResource* pTexture ) override;
	virtual IShaderResource* GetTexture( ) const override { return m_pTexture; };

	void SetTextureName( const String& textureName ) { m_textureName = textureName; }

private:
	IShaderResource* m_pTexture;
	String m_textureName;
};

bool CMeshBuilderMesh::Load( IRenderer& renderer, D3D_PRIMITIVE_TOPOLOGY topology )
{
	bool loadSuccess = BaseMesh::Load( renderer, topology );

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

void CMeshBuilderMesh::Draw( ID3D11DeviceContext* pDeviceContext )
{
	if ( !pDeviceContext )
	{
		return;
	}

	if ( !m_pMaterial )
	{
		return;
	}

	if ( !m_pVertexBuffer )
	{
		return;
	}

	m_pMaterial->SetShader( pDeviceContext );
	m_pMaterial->SetPrimitiveTopology( pDeviceContext, m_primitiveTopology );
	m_pVertexBuffer->SetIABuffer( pDeviceContext, &m_nOffset );
	m_pMaterial->SetTexture( pDeviceContext, SHADER_TYPE::PS, 0, m_pTexture );
	if ( m_pIndexBuffer )
	{
		m_pIndexBuffer->SetIABuffer( pDeviceContext, &m_nIndexOffset );
		m_pMaterial->DrawIndexed( pDeviceContext, m_nIndices, m_nIndexOffset, m_nOffset );
	}
	else
	{
		m_pMaterial->Draw( pDeviceContext, m_nVertices, m_nOffset );
	}
}

void CMeshBuilderMesh::SetTexture( IShaderResource* pTexture )
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

IMesh* CMeshBuilder::Build( IRenderer& renderer, const String& meshName, D3D_PRIMITIVE_TOPOLOGY topology ) const
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

	if ( newMesh->Load( renderer, topology ) )
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