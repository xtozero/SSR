#include "stdafx.h"
#include "GameObject/BallProjectile.h"

#include "Core/GameLogic.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "Model/IModelBuilder.h"
#include "Model/MeshBuildHelper.h"
//#include "Render/IRenderer.h"

DECLARE_GAME_OBJECT( ball_projectile, CBallProjectile );

namespace
{
	constexpr float PROJECTILE_HEIGHT_LOWER_LIMIT = -100.f;
}

void CBallProjectile::Think( float /*elapsedTime*/ )
{
	if ( GetPosition( ).y < PROJECTILE_HEIGHT_LOWER_LIMIT )
	{
		RemoveObject( *this );
	}
}

CBallProjectile::CBallProjectile( )
{
	SetMaterialName( "mat_default" );
}

//bool CBallProjectile::LoadModelMesh( CGameLogic& gameLogic )
//{
	//if ( GetModel( ) != nullptr )
	//{
	//	return false;
	//}

	//MeshData sphere = MeshBuildHelper::CreateSphere( 0.5f, 15, 15 );

	//IModelBuilder& meshBuilder = gameLogic.GetModelManager( ).GetModelBuilder( );
	//meshBuilder.Clear( );

	//for ( const auto& vertex : sphere.m_vertices )
	//{
	//	meshBuilder.Append( MeshVertex( vertex.m_position, vertex.m_normal, CXMFLOAT3( 1.f, 0.f, 0.f ) ) );
	//}

	//for ( const auto index : sphere.m_indices )
	//{
	//	meshBuilder.AppendIndex( static_cast<WORD>( index ) );
	//}
	//
	//SetModelMeshName( _T("ball_projectile") );
	//SetModel( meshBuilder.Build( gameLogic.GetRenderer(), GetMeshName() ) );

//	return true;
//}
