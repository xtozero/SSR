#include "stdafx.h"
#include "GameObject/SkyBox.h"

#include "Core/GameLogic.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
//#include "Model/IModelBuilder.h"
//#include "Render/IRenderer.h"

DECLARE_GAME_OBJECT( skybox, CSkyBox );

namespace
{
	constexpr float SKYBOX_LENGTH = 10.f;
}

void CSkyBox::Think( float /*elapsedTime*/ )
{
	// To Do
	//CameraComponent* pCurCamera = CCameraManager::GetInstance( ).GetCurrentCamera( );

	//if ( pCurCamera )
	//{
	//	SetPosition( pCurCamera->GetOrigin( ) );
	//}
}

//bool CSkyBox::LoadModelMesh( CGameLogic& gameLogic )
//{
	//if ( GetModel( ) != nullptr )
	//{
	//	return false;
	//}

	//IModelBuilder& meshBuilder = gameLogic.GetModelManager( ).GetModelBuilder( );
	//meshBuilder.Clear( );

	//meshBuilder.Append( MeshVertex( CXMFLOAT3( -SKYBOX_LENGTH, -SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	//meshBuilder.Append( MeshVertex( CXMFLOAT3( -SKYBOX_LENGTH, -SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	//meshBuilder.Append( MeshVertex( CXMFLOAT3( -SKYBOX_LENGTH, SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	//meshBuilder.Append( MeshVertex( CXMFLOAT3( -SKYBOX_LENGTH, SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	//meshBuilder.Append( MeshVertex( CXMFLOAT3( SKYBOX_LENGTH, -SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	//meshBuilder.Append( MeshVertex( CXMFLOAT3( SKYBOX_LENGTH, -SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	//meshBuilder.Append( MeshVertex( CXMFLOAT3( SKYBOX_LENGTH, SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	//meshBuilder.Append( MeshVertex( CXMFLOAT3( SKYBOX_LENGTH, SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );

	////반 시계 방향으로 인덱스를 구성.
	//meshBuilder.AppendIndex( 4 ); meshBuilder.AppendIndex( 6 ); meshBuilder.AppendIndex( 0 );
	//meshBuilder.AppendIndex( 6 ); meshBuilder.AppendIndex( 2 ); meshBuilder.AppendIndex( 0 );
	//meshBuilder.AppendIndex( 2 ); meshBuilder.AppendIndex( 3 ); meshBuilder.AppendIndex( 0 );
	//meshBuilder.AppendIndex( 3 ); meshBuilder.AppendIndex( 1 ); meshBuilder.AppendIndex( 0 );
	//meshBuilder.AppendIndex( 6 ); meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 2 );
	//meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 3 ); meshBuilder.AppendIndex( 2 );
	//meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 6 ); meshBuilder.AppendIndex( 4 );
	//meshBuilder.AppendIndex( 5 ); meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 4 );
	//meshBuilder.AppendIndex( 5 ); meshBuilder.AppendIndex( 4 ); meshBuilder.AppendIndex( 0 );
	//meshBuilder.AppendIndex( 1 ); meshBuilder.AppendIndex( 5 ); meshBuilder.AppendIndex( 0 );
	//meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 5 ); meshBuilder.AppendIndex( 1 );
	//meshBuilder.AppendIndex( 3 ); meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 1 );

	//meshBuilder.AppendTextureName( "SkyboxSet/TropicalSunnyDay/TropicalSunnyDay.dds" );
	//
	//SetModelMeshName( GetName( ) );
	//SetModel( meshBuilder.Build( gameLogic.GetRenderer(), GetMeshName() ) );

	//return GetModel() ? true : false;
	//return true;
//}
