#include "stdafx.h"
#include "Camera.h"
#include "CameraManager.h"
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "SkyBox.h"

#include "../RenderCore/IMeshBuilder.h"
#include "../RenderCore/IRenderer.h"

extern IMeshBuilder* g_meshBuilder;
extern IRenderer* gRenderer;

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

bool CSkyBox::LoadModelMesh( )
{
	if ( GetModel( ) != nullptr || g_meshBuilder == nullptr )
	{
		return false;
	}

	g_meshBuilder->Clear( );

	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( -SKYBOX_LENGTH, -SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( -SKYBOX_LENGTH, -SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( -SKYBOX_LENGTH, SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( -SKYBOX_LENGTH, SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( SKYBOX_LENGTH, -SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( SKYBOX_LENGTH, -SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );
	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( SKYBOX_LENGTH, SKYBOX_LENGTH, -SKYBOX_LENGTH ) ) );
	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( SKYBOX_LENGTH, SKYBOX_LENGTH, SKYBOX_LENGTH ) ) );

	//�� �ð� �������� �ε����� ����.
	g_meshBuilder->AppendIndex( 4 ); g_meshBuilder->AppendIndex( 6 ); g_meshBuilder->AppendIndex( 0 );
	g_meshBuilder->AppendIndex( 6 ); g_meshBuilder->AppendIndex( 2 ); g_meshBuilder->AppendIndex( 0 );
	g_meshBuilder->AppendIndex( 2 ); g_meshBuilder->AppendIndex( 3 ); g_meshBuilder->AppendIndex( 0 );
	g_meshBuilder->AppendIndex( 3 ); g_meshBuilder->AppendIndex( 1 ); g_meshBuilder->AppendIndex( 0 );
	g_meshBuilder->AppendIndex( 6 ); g_meshBuilder->AppendIndex( 7 ); g_meshBuilder->AppendIndex( 2 );
	g_meshBuilder->AppendIndex( 7 ); g_meshBuilder->AppendIndex( 3 ); g_meshBuilder->AppendIndex( 2 );
	g_meshBuilder->AppendIndex( 7 ); g_meshBuilder->AppendIndex( 6 ); g_meshBuilder->AppendIndex( 4 );
	g_meshBuilder->AppendIndex( 5 ); g_meshBuilder->AppendIndex( 7 ); g_meshBuilder->AppendIndex( 4 );
	g_meshBuilder->AppendIndex( 5 ); g_meshBuilder->AppendIndex( 4 ); g_meshBuilder->AppendIndex( 0 );
	g_meshBuilder->AppendIndex( 1 ); g_meshBuilder->AppendIndex( 5 ); g_meshBuilder->AppendIndex( 0 );
	g_meshBuilder->AppendIndex( 7 ); g_meshBuilder->AppendIndex( 5 ); g_meshBuilder->AppendIndex( 1 );
	g_meshBuilder->AppendIndex( 3 ); g_meshBuilder->AppendIndex( 7 ); g_meshBuilder->AppendIndex( 1 );

	g_meshBuilder->AppendTextureName( _T( "SkyboxSet/TropicalSunnyDay/TropicalSunnyDay.dds" ) );
	
	SetModelMeshName( GetName( ) );
	SetModel( g_meshBuilder->Build( GetMeshName() ) );

	return GetModel() ? true : false;
}
