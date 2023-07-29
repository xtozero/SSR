#include "stdafx.h"
#include "Components/CameraComponent.h"
#include "Components/GroundMovementComponent.h"
#include "Components/InputComponent.h"
#include "Core/GameLogic.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "GameObject/Player.h"
#include "Json/json.hpp"

#include "UserInput/UserInput.h"

#include <memory>

DECLARE_GAME_OBJECT( player, CPlayer );

void CPlayer::Initialize( CGameLogic& gameLogic, World& world )
{
	Super::Initialize( gameLogic, world );

	if ( InputController* inputController = gameLogic.GetInputController() )
	{
		inputController->Control( this );
	}
}

void CPlayer::Think( float elapsedTime )
{
	Super::Think( elapsedTime );

	Vector force( static_cast<float>( m_inputDirection[2] - m_inputDirection[0] ),
				  0.f,
				  static_cast<float>( m_inputDirection[1] - m_inputDirection[3] ) );

	if ( auto* pMovementComponent = GetComponent<GroundMovementComponent>() )
	{
		pMovementComponent->Update( force, elapsedTime );
	}
}

void CPlayer::ProcessInput( const UserInput& input, CGameLogic& gameLogic )
{
	assert( GetWorld() != nullptr );
	const Timer& timer = GetWorld()->GetTimer();
	constexpr float angleInterval = DirectX::XMConvertToRadians( 30 );

	switch ( input.m_code )
	{
	case UIC_SPACEBAR:
		OnMouseLButton( input, gameLogic );
		break;
	case UIC_U:
		RotatePrimaryLightDir( gameLogic, angleInterval * timer.GetElapsedTime(), 0.f );
		break;
	case UIC_J:
		RotatePrimaryLightDir( gameLogic, -angleInterval * timer.GetElapsedTime(), 0.f );
		break;
	case UIC_I:
		RotatePrimaryLightDir( gameLogic, 0.f, angleInterval * timer.GetElapsedTime() );
		break;
	case UIC_K:
		RotatePrimaryLightDir( gameLogic, 0.f, -angleInterval * timer.GetElapsedTime() );
		break;
	}
}

CPlayer::CPlayer()
{
	m_cameraComponent = CreateComponent<CameraComponent>( *this, "CameraComponent" );
	SetRootComponent( m_cameraComponent );

	CreateComponent<GroundMovementComponent>( *this, "GroundMovementComponent" );

	m_think.m_canEverTick = true;
}

void CPlayer::SetupInputComponent()
{
	Super::SetupInputComponent();

	m_inputComponent->BindInput( UIC_MOUSE_MOVE, this, &CPlayer::OnMouseMove );
	m_inputComponent->BindInput( UIC_MOUSE_LEFT, this, &CPlayer::OnMouseLButton );
	m_inputComponent->BindInput( UIC_MOUSE_RIGHT, this, &CPlayer::OnMouseRButton );
	m_inputComponent->BindInput( UIC_MOUSE_WHEELSPIN, this, &CPlayer::OnWheelMove );
	m_inputComponent->BindInput( UIC_RIGHT, this, &CPlayer::OnMoveKey );
	m_inputComponent->BindInput( UIC_LEFT, this, &CPlayer::OnMoveKey );
	m_inputComponent->BindInput( UIC_UP, this, &CPlayer::OnMoveKey );
	m_inputComponent->BindInput( UIC_DOWN, this, &CPlayer::OnMoveKey );
	m_inputComponent->BindInput( UIC_W, this, &CPlayer::OnMoveKey );
	m_inputComponent->BindInput( UIC_A, this, &CPlayer::OnMoveKey );
	m_inputComponent->BindInput( UIC_S, this, &CPlayer::OnMoveKey );
	m_inputComponent->BindInput( UIC_D, this, &CPlayer::OnMoveKey );
}

void CPlayer::OnMouseLButton( const UserInput& input, CGameLogic& gameLogic )
{
	if ( input.m_axis[UserInput::Z_AXIS] >= 1 )
	{
		if ( Owner<CGameObject*> newObject = GetGameObjectFactory().CreateGameObjectByClassName( "ball_projectile" ) )
		{
			newObject->SetName( "ball" );
			newObject->SetPosition( m_cameraComponent->GetPosition() );
			newObject->SetScale3D( Vector( 5, 5, 5 ) );
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

void CPlayer::OnMouseLButton( const UserInput& input )
{
	m_cameraRotationEnabled = input.m_axis[UserInput::Z_AXIS] < 0;
}

void CPlayer::OnMouseRButton( const UserInput& input )
{
	m_cameraTranslationEnabled = input.m_axis[UserInput::Z_AXIS] < 0;
}

void CPlayer::OnMouseMove( const UserInput& input )
{
	float dx = input.m_axis[UserInput::X_AXIS];
	float dy = input.m_axis[UserInput::Y_AXIS];

	dx *= m_mouseSensitivity;
	dy *= m_mouseSensitivity;

	if ( m_cameraRotationEnabled )
	{
		m_cameraComponent->Rotate( dy, dx, 0 );
	}
	else if ( m_cameraTranslationEnabled )
	{
		m_cameraComponent->Move( dx, dy, 0 );
	}
}

void CPlayer::OnWheelMove( const UserInput& input )
{
	m_cameraComponent->Move( 0, 0, static_cast<float>( input.m_axis[UserInput::Z_AXIS] ) );
}

void CPlayer::OnMoveKey( const UserInput& input )
{
	if ( input.m_code == UserInputCode::UIC_LEFT
		|| input.m_code == UserInputCode::UIC_A )
	{
		m_inputDirection[0] = ( input.m_axis[UserInput::Z_AXIS] < 0 );
	}
	else if ( input.m_code == UserInputCode::UIC_UP
		|| input.m_code == UserInputCode::UIC_W )
	{
		m_inputDirection[1] = ( input.m_axis[UserInput::Z_AXIS] < 0 );
	}
	else if ( input.m_code == UserInputCode::UIC_RIGHT
		|| input.m_code == UserInputCode::UIC_D )
	{
		m_inputDirection[2] = ( input.m_axis[UserInput::Z_AXIS] < 0 );
	}
	else if ( input.m_code == UserInputCode::UIC_DOWN 
		|| input.m_code == UserInputCode::UIC_S )
	{
		m_inputDirection[3] = ( input.m_axis[UserInput::Z_AXIS] < 0 );
	}
}
