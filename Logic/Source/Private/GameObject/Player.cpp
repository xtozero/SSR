#include "stdafx.h"
#include "Core/GameLogic.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "GameObject/Player.h"

#include "UserInput/UserInput.h"

#include <memory>

void CPlayer::ProcessInput( const UserInput& input, CGameLogic& gameLogic )
{
	switch ( input.m_code )
	{
	case UIC_SPACEBAR:
		OnMouseLButton( input, gameLogic );
		break;
	}

	m_camera.ProcessInput( input, gameLogic );
}

void CPlayer::OnDeviceRestore( CGameLogic& gameLogic )
{
	m_camera.OnDeviceRestore( gameLogic );
}

void CPlayer::OnMouseLButton( const UserInput& input, CGameLogic& gameLogic )
{
	if ( input.m_axis[UserInput::Z_AXIS] >= 1 )
	{
		if ( Owner<CGameObject*> newObject = GetGameObjectFactory( ).CreateGameObjectByClassName( _T( "ball_projectile" ) ) )
		{
			newObject->SetName( _T( "ball" ) );
			newObject->SetPosition( m_camera.GetOrigin( ) );
			newObject->SetScale( 5, 5, 5 );
			newObject->SetColliderType( COLLIDER::SPHERE );
			newObject->GetRigidBody( )->SetMass( 10 );
			newObject->GetRigidBody( )->SetVelocity( m_camera.GetForwardVector() * 200.f );

			gameLogic.SpawnObject( newObject );
		}
	}
}
