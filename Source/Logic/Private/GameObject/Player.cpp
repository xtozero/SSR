#include "stdafx.h"
#include "Components/CameraComponent.h"
#include "Core/GameLogic.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "GameObject/Player.h"
#include "Json/json.hpp"

#include "UserInput/UserInput.h"

#include <memory>

DECLARE_GAME_OBJECT( player, CPlayer );

void CPlayer::LoadProperty( CGameLogic& gameLogic, const JSON::Value& json )
{
	CGameObject::LoadProperty( gameLogic, json );

	if ( const JSON::Value* pCamera = json.Find( "Camera" ) )
	{
		m_cameraComponent->LoadProperty( gameLogic, *pCamera );
	}
}

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

	m_cameraComponent->ProcessInput( input, gameLogic );
}

CPlayer::CPlayer( )
{
	m_cameraComponent = CreateComponent<CameraComponent>( *this );
	m_rootComponent = m_cameraComponent;
}

void CPlayer::OnMouseLButton( const UserInput& input, CGameLogic& gameLogic )
{
	if ( input.m_axis[UserInput::Z_AXIS] >= 1 )
	{
		if ( Owner<CGameObject*> newObject = GetGameObjectFactory( ).CreateGameObjectByClassName( "ball_projectile" ) )
		{
			newObject->SetName( "ball" );
			newObject->SetPosition( m_cameraComponent->GetPosition( ) );
			newObject->SetScale( 5, 5, 5 );
			newObject->SetColliderType( COLLIDER::SPHERE );
			//newObject->GetRigidBody( )->SetMass( 10 );
			//newObject->GetRigidBody( )->SetVelocity( m_camera.GetForwardVector() * 200.f );

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
