#include "stdafx.h"
#include "Core/GameLogic.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "GameObject/Player.h"

#include "UserInput/UserInput.h"

#include <memory>

void CPlayer::ProcessInput( const UserInput& input, CGameLogic& gameLogic )
{
	const CTimer& timer = gameLogic.GetTimer( );
	constexpr float angleInterval = DirectX::XMConvertToRadians( 30 );

	switch ( input.m_code )
	{
	case UIC_SPACEBAR:
		OnMouseLButton( input, gameLogic );
		break;
	case UIC_U:
		RotatePrimaryLightDir( gameLogic, angleInterval * timer.GetElapsedTime( ), 0.f );
		break;
	case UIC_J:
		RotatePrimaryLightDir( gameLogic, -angleInterval * timer.GetElapsedTime( ), 0.f );
		break;
	case UIC_I:
		RotatePrimaryLightDir( gameLogic, 0.f, angleInterval * timer.GetElapsedTime( ) );
		break;
	case UIC_K:
		RotatePrimaryLightDir( gameLogic, 0.f, -angleInterval * timer.GetElapsedTime( ) );
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
		if ( Owner<CGameObject*> newObject = GetGameObjectFactory( ).CreateGameObjectByClassName( "ball_projectile" ) )
		{
			newObject->SetName( "ball" );
			newObject->SetPosition( m_camera.GetOrigin( ) );
			newObject->SetScale( 5, 5, 5 );
			newObject->SetColliderType( COLLIDER::SPHERE );
			newObject->GetRigidBody( )->SetMass( 10 );
			newObject->GetRigidBody( )->SetVelocity( m_camera.GetForwardVector() * 200.f );

			gameLogic.SpawnObject( newObject );
		}
	}
}

void CPlayer::RotatePrimaryLightDir( CGameLogic& gameLogic, float deltaTheta, float deltaPhi )
{
	//CLightManager& lightMgr = gameLogic.GetLightManager( );

	//if ( CLight* pLight = lightMgr.GetPrimaryLight( ) )
	//{
	//	std::pair<float, float> sphericalCoord = CartesianToSpherical( pLight->GetDirection( ) );
	//	sphericalCoord.first += deltaTheta;
	//	sphericalCoord.second += deltaPhi;
	//	pLight->SetDiection( SphericalToCartesian( sphericalCoord.first, sphericalCoord.second ) );
	//}

	//lightMgr.OnLightPropertyUpdated( );
}
