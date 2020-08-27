#include "stdafx.h"
#include "Model/ObjMesh.h"

#include "Core/GameLogic.h"
#include "Model/Surface.h"
//#include "Render/IRenderer.h"

//bool CObjMesh::Load( IRenderer& renderer, UINT primitive )
//{
//	bool loadSuccess = BaseMesh::Load( renderer, primitive );
//
//	for ( auto& mtl : m_mtlGroup )
//	{
//		const std::string& textureName = mtl.m_pSurface->m_diffuseTexName;
//		if ( textureName.size() > 0 )
//		{
//			auto texture = renderer.CreateShaderResourceFromFile( textureName );
//
//			if ( texture != RE_HANDLE::InValidHandle( ) )
//			{
//				mtl.m_texture = texture;
//			}
//			else
//			{
//				return false;
//			}
//		}
//	}
//
//	return loadSuccess;
//}

//void CObjMesh::Draw( CGameLogic& gameLogic )
//{
	//if ( m_material == INVALID_MATERIAL )
	//{
	//	return;
	//}

	//if ( m_vertexBuffer == RE_HANDLE::InValidHandle( ) )
	//{
	//	return;
	//}

	//IRenderer& renderer = gameLogic.GetRenderer( );

	//renderer.BindMaterial( m_material );
	//renderer.BindVertexBuffer( &m_vertexBuffer, 0, 1, &m_stride, &m_offset );
	//if ( m_indexBuffer == RE_HANDLE::InValidHandle( ) )
	//{
	//	gameLogic.GetRenderer( ).Draw( m_primitiveTopology, m_nVertices, m_offset );
	//}
	//else
	//{
	//	renderer.BindIndexBuffer( m_indexBuffer, m_nIndexOffset );

	//	if ( m_mtlGroup.size( ) == 0 )
	//	{
	//		gameLogic.GetRenderer( ).DrawIndexed( m_primitiveTopology, m_nIndices, m_nIndexOffset, m_offset );
	//	}
	//	else
	//	{
	//		for ( const auto& mtl : m_mtlGroup )
	//		{
	//			renderer.BindShaderResource( SHADER_TYPE::PS, 0, 1, &mtl.m_texture );
	//			RE_HANDLE constantBuffer = gameLogic.GetCommonConstantBuffer( SHARED_CONSTANT_BUFFER::PS_SURFACE );
	//			SurfaceTrait* surface = static_cast<SurfaceTrait*>( renderer.LockBuffer( constantBuffer ) );

	//			if ( surface )
	//			{
	//				memcpy_s( surface, sizeof( SurfaceTrait ), &mtl.m_pSurface->m_trait, sizeof( SurfaceTrait ) );
	//			}

	//			renderer.UnLockBuffer( constantBuffer );
	//			renderer.BindConstantBuffer( SHADER_TYPE::PS, PS_CONSTANT_BUFFER::SURFACE, 1, &constantBuffer );
	//			gameLogic.GetRenderer( ).DrawIndexed( m_primitiveTopology, mtl.m_indexCount, mtl.m_indexOffset, m_offset );
	//		}
	//	}
	//}
//}

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
