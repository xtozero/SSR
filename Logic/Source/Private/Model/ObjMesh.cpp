#include "stdafx.h"
#include "Model/ObjMesh.h"

#include "Core/GameLogic.h"
#include "Model/Surface.h"
#include "Render/IRenderer.h"

bool CObjMesh::Load( IRenderer& renderer, UINT primitive )
{
	bool loadSuccess = BaseMesh::Load( renderer, primitive );

	FOR_EACH_VEC( m_mtlGroup, i )
	{
		const String& textureName = i->m_pSurface->m_diffuseTexName;
		if ( textureName.size() > 0 )
		{
			auto texture = renderer.CreateShaderResourceFromFile( textureName );

			if ( texture )
			{
				i->m_texture = texture;
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
	if ( m_material == INVALID_MATERIAL )
	{
		return;
	}

	if ( m_vertexBuffer == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return;
	}

	IRenderer& renderer = gameLogic.GetRenderer( );

	renderer.BindMaterial( m_material );
	renderer.BindVertexBuffer( &m_vertexBuffer, 0, 1, &m_stride, &m_offset );
	if ( m_indexBuffer == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		gameLogic.GetRenderer( ).Draw( m_primitiveTopology, m_nVertices, m_offset );
	}
	else
	{
		renderer.BindIndexBuffer( m_indexBuffer, m_nIndexOffset );

		if ( m_mtlGroup.size( ) == 0 )
		{
			gameLogic.GetRenderer( ).DrawIndexed( m_primitiveTopology, m_nIndices, m_nIndexOffset, m_offset );
		}
		else
		{
			FOR_EACH_VEC( m_mtlGroup, i )
			{
				renderer.BindShaderResource( SHADER_TYPE::PS, 0, 1, &i->m_texture );
				RE_HANDLE constantBuffer = gameLogic.GetCommonConstantBuffer( SHARED_CONSTANT_BUFFER::PS_SURFACE );
				SurfaceTrait* surface = static_cast<SurfaceTrait*>( renderer.LockBuffer( constantBuffer ) );

				if ( surface )
				{
					memcpy_s( surface, sizeof( SurfaceTrait ), &i->m_pSurface->m_trait, sizeof( SurfaceTrait ) );
				}

				renderer.UnLockBuffer( constantBuffer );
				renderer.BindConstantBuffer( SHADER_TYPE::PS, PS_CONSTANT_BUFFER::SURFACE, 1, &constantBuffer );
				gameLogic.GetRenderer( ).DrawIndexed( m_primitiveTopology, i->m_indexCount, i->m_indexOffset, m_offset );
			}
		}
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
