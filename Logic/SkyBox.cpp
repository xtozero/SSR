#include "stdafx.h"
#include "Camera.h"
#include "CameraManager.h"
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "SkyBox.h"

#include "../RenderCore/IMeshBuilder.h"
#include "../RenderCore/IRenderer.h"

DECLARE_GAME_OBJECT( skybox, CSkyBox );

namespace
{
	constexpr float SKYBOX_LENGTH = 10.f;
}

void CSkyBox::Think( )
{
	CCamera* pCurCamera = CCameraManager::GetInstance( )->GetCurrentCamera( );

	if ( pCurCamera )
	{
		SetPosition( pCurCamera->GetOrigin( ) );
	}
}

bool CSkyBox::LoadModelMesh( IRenderer& renderer )
{
	IMeshBuilder& meshBuilder = renderer.GetMeshBuilder( );

	if ( GetModel( ) != nullptr )
	{
		return false;
	}

	meshBuilder.Append( MeshVertex( CXMFLOAT3( -SKYBOX_LENGTH, -SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( -SKYBOX_LENGTH, -SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( -SKYBOX_LENGTH, SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( -SKYBOX_LENGTH, SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( SKYBOX_LENGTH, -SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( SKYBOX_LENGTH, -SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( SKYBOX_LENGTH, SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( SKYBOX_LENGTH, SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );

	//반 시계 방향으로 인덱스를 구성.
	meshBuilder.AppendIndex( 4 ); meshBuilder.AppendIndex( 6 ); meshBuilder.AppendIndex( 0 );
	meshBuilder.AppendIndex( 6 ); meshBuilder.AppendIndex( 2 ); meshBuilder.AppendIndex( 0 );
	meshBuilder.AppendIndex( 2 ); meshBuilder.AppendIndex( 3 ); meshBuilder.AppendIndex( 0 );
	meshBuilder.AppendIndex( 3 ); meshBuilder.AppendIndex( 1 ); meshBuilder.AppendIndex( 0 );
	meshBuilder.AppendIndex( 6 ); meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 2 );
	meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 3 ); meshBuilder.AppendIndex( 2 );
	meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 6 ); meshBuilder.AppendIndex( 4 );
	meshBuilder.AppendIndex( 5 ); meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 4 );
	meshBuilder.AppendIndex( 5 ); meshBuilder.AppendIndex( 4 ); meshBuilder.AppendIndex( 0 );
	meshBuilder.AppendIndex( 1 ); meshBuilder.AppendIndex( 5 ); meshBuilder.AppendIndex( 0 );
	meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 5 ); meshBuilder.AppendIndex( 1 );
	meshBuilder.AppendIndex( 3 ); meshBuilder.AppendIndex( 7 ); meshBuilder.AppendIndex( 1 );

	meshBuilder.AppendTextureName( _T( "SkyboxSet/TropicalSunnyDay/TropicalSunnyDay.dds" ) );
	
	SetModelMeshName( GetName( ) );
	SetModel( meshBuilder.Build( renderer, GetMeshName() ) );

	return GetModel() ? true : false;
}
