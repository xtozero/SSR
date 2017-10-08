#include "stdafx.h"
#include "ObjMesh.h"

#include "CommonRenderer/ConstantBufferDefine.h"
#include "CommonRenderer/IRenderer.h"
#include "CommonRenderer/ISurface.h"
#include "Material.h"

#include "../shared/Util.h"

bool CObjMesh::Load( IRenderer& renderer, UINT primitive )
{
	bool loadSuccess = BaseMesh::Load( renderer, primitive );

	FOR_EACH_VEC( m_mtlGroup, i )
	{
		const String& textureName = i->m_pSurface->GetTextureName( );
		if ( textureName.size() > 0 )
		{
			auto texture = renderer.GetShaderResourceFromFile( textureName );

			if ( texture )
			{
				i->m_pTexture = texture;
			}
			else
			{
				return false;
			}
		}
	}

	return loadSuccess;
}

void CObjMesh::Draw( IRenderer& renderer )
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
	if ( m_pIndexBuffer )
	{
		m_pIndexBuffer->SetIndexBuffer( sizeof( WORD ), m_nIndexOffset );

		if ( m_mtlGroup.size( ) == 0 )
		{
			renderer.DrawIndexed( m_primitiveTopology, m_nIndices, m_nIndexOffset, m_nOffset );
		}
		else
		{
			FOR_EACH_VEC( m_mtlGroup, i )
			{
				m_pMaterial->SetTexture( SHADER_TYPE::PS, 0, i->m_pTexture );
				m_pMaterial->SetSurface( SHADER_TYPE::PS, static_cast<UINT>( PS_CONSTANT_BUFFER::SURFACE ), i->m_pSurface );
				renderer.DrawIndexed( m_primitiveTopology, i->m_indexCount, i->m_indexOffset, m_nOffset );
			}
		}
	}
	else
	{
		renderer.Draw( m_primitiveTopology, m_nVertices, m_nOffset );
	}
}

void CObjMesh::AddMaterialGroup( const ObjSurfaceTrait& trait )
{
	m_mtlGroup.emplace_back( trait );
}

CObjMesh::CObjMesh( )
{
}


CObjMesh::~CObjMesh( )
{
}
