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
	IMeshBuilder* pMeshBuilder = renderer.GetMeshBuilder( );

	if ( GetModel( ) != nullptr || pMeshBuilder == nullptr )
	{
		return false;
	}

	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( -SKYBOX_LENGTH, -SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( -SKYBOX_LENGTH, -SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( -SKYBOX_LENGTH, SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( -SKYBOX_LENGTH, SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( SKYBOX_LENGTH, -SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( SKYBOX_LENGTH, -SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( SKYBOX_LENGTH, SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( SKYBOX_LENGTH, SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );

	//반 시계 방향으로 인덱스를 구성.
	pMeshBuilder->AppendIndex( 4 ); pMeshBuilder->AppendIndex( 6 ); pMeshBuilder->AppendIndex( 0 );
	pMeshBuilder->AppendIndex( 6 ); pMeshBuilder->AppendIndex( 2 ); pMeshBuilder->AppendIndex( 0 );
	pMeshBuilder->AppendIndex( 2 ); pMeshBuilder->AppendIndex( 3 ); pMeshBuilder->AppendIndex( 0 );
	pMeshBuilder->AppendIndex( 3 ); pMeshBuilder->AppendIndex( 1 ); pMeshBuilder->AppendIndex( 0 );
	pMeshBuilder->AppendIndex( 6 ); pMeshBuilder->AppendIndex( 7 ); pMeshBuilder->AppendIndex( 2 );
	pMeshBuilder->AppendIndex( 7 ); pMeshBuilder->AppendIndex( 3 ); pMeshBuilder->AppendIndex( 2 );
	pMeshBuilder->AppendIndex( 7 ); pMeshBuilder->AppendIndex( 6 ); pMeshBuilder->AppendIndex( 4 );
	pMeshBuilder->AppendIndex( 5 ); pMeshBuilder->AppendIndex( 7 ); pMeshBuilder->AppendIndex( 4 );
	pMeshBuilder->AppendIndex( 5 ); pMeshBuilder->AppendIndex( 4 ); pMeshBuilder->AppendIndex( 0 );
	pMeshBuilder->AppendIndex( 1 ); pMeshBuilder->AppendIndex( 5 ); pMeshBuilder->AppendIndex( 0 );
	pMeshBuilder->AppendIndex( 7 ); pMeshBuilder->AppendIndex( 5 ); pMeshBuilder->AppendIndex( 1 );
	pMeshBuilder->AppendIndex( 3 ); pMeshBuilder->AppendIndex( 7 ); pMeshBuilder->AppendIndex( 1 );

	pMeshBuilder->AppendTextureName( _T( "SkyboxSet/TropicalSunnyDay/TropicalSunnyDay.dds" ) );
	
	SetModelMeshName( GetName( ) );
	SetModel( pMeshBuilder->Build( renderer, GetMeshName() ) );

	return GetModel() ? true : false;
}
