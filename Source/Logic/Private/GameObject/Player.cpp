#include "stdafx.h"

#include "Components/CameraComponent.h"
#include "Components/GroundMovementComponent.h"
#include "Components/InputComponent.h"
#include "Core/GameLogic.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "GameObject/Player.h"
#include "Json/json.hpp"
#include "Math/Rotator.h"

#include "UserInput/UserInput.h"

#include <memory>

using ::DirectX::XMConvertToDegrees;
using ::engine::UserInput;
using enum ::engine::UserInputCode;

namespace logic
{
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

	void CPlayer::OnMouseLButton( const UserInput& input )
	{
		
	}

	void CPlayer::OnMouseRButton( const UserInput& input )
	{
		m_cameraRotationEnabled = input.m_axis[UserInput::Z_AXIS] < 0;
	}

	void CPlayer::OnMouseMove( const UserInput& input )
	{
		if ( m_cameraRotationEnabled )
		{
			float dx = input.m_axis[UserInput::X_AXIS];
			float dy = input.m_axis[UserInput::Y_AXIS];

			dx *= m_mouseSensitivity;
			dx = XMConvertToDegrees( dx );

			dy *= m_mouseSensitivity;
			dy = XMConvertToDegrees( dy );

			Rotator curRotator = GetRotation().ToRotator();
			Rotator rotator = curRotator + Rotator( dy, dx, 0.f );

			SetRotation( rotator.ToQuat() );
		}
	}

	void CPlayer::OnWheelMove( const UserInput& input )
	{
	}

	void CPlayer::OnMoveKey( const UserInput& input )
	{
		if ( input.m_code == UIC_LEFT
			|| input.m_code == UIC_A )
		{
			m_inputDirection[0] = ( input.m_axis[UserInput::Z_AXIS] < 0 );
		}
		else if ( input.m_code == UIC_UP
			|| input.m_code == UIC_W )
		{
			m_inputDirection[1] = ( input.m_axis[UserInput::Z_AXIS] < 0 );
		}
		else if ( input.m_code == UIC_RIGHT
			|| input.m_code == UIC_D )
		{
			m_inputDirection[2] = ( input.m_axis[UserInput::Z_AXIS] < 0 );
		}
		else if ( input.m_code == UIC_DOWN
			|| input.m_code == UIC_S )
		{
			m_inputDirection[3] = ( input.m_axis[UserInput::Z_AXIS] < 0 );
		}
	}
}
