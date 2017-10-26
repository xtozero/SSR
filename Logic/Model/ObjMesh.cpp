#include "stdafx.h"
#include "ObjMesh.h"

#include "Surface.h"

#include "../GameLogic.h"
#include "../../RenderCore/CommonRenderer/IMaterial.h"
#include "../../RenderCore/CommonRenderer/IRenderer.h"

bool CObjMesh::Load( IRenderer& renderer, UINT primitive )
{
	bool loadSuccess = BaseMesh::Load( renderer, primitive );

	FOR_EACH_VEC( m_mtlGroup, i )
	{
		const String& textureName = i->m_pSurface->m_diffuseTexName;
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

void CObjMesh::Draw( CGameLogic& gameLogic )
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
			gameLogic.GetRenderer().DrawIndexed( m_primitiveTopology, m_nIndices, m_nIndexOffset, m_nOffset );
		}
		else
		{
			FOR_EACH_VEC( m_mtlGroup, i )
			{
				m_pMaterial->SetTexture( SHADER_TYPE::PS, 0, i->m_pTexture );
				IBuffer& constantBuffer = gameLogic.GetCommonConstantBuffer( SHARED_CONSTANT_BUFFER::PS_SURFACE );
				SurfaceTrait* surface = static_cast<SurfaceTrait*>(constantBuffer.LockBuffer( ));

				if ( surface )
				{
					memcpy_s( surface, sizeof( SurfaceTrait ), &i->m_pSurface->m_trait, sizeof( SurfaceTrait ) );
				}

				constantBuffer.UnLockBuffer( );
				constantBuffer.SetPSBuffer( PS_CONSTANT_BUFFER::SURFACE );
				gameLogic.GetRenderer( ).DrawIndexed( m_primitiveTopology, i->m_indexCount, i->m_indexOffset, m_nOffset );
			}
		}
	}
	else
	{
		gameLogic.GetRenderer( ).Draw( m_primitiveTopology, m_nVertices, m_nOffset );
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
