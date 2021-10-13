#include "stdafx.h"
#include "UserInput/Controller.h"

#include "common.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Core/IListener.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "UserInput/UserInput.h"

DECLARE_GAME_OBJECT( player_controller, PlayerController );

void InputController::SetGameObject( CGameObject* gameObject )
{
	m_gameObject = gameObject;
}

InputController::InputController( )
{
	m_rootComponent = CreateComponent<SceneComponent>( *this );
}

void PlayerController::ProcessInput( const UserInput& input )
{
	if ( m_gameObject == nullptr )
	{
		return;
	}

	InputComponent* inputComponent = m_gameObject->GetInputComponent( );
	if ( inputComponent != nullptr )
	{
		inputComponent->ProcessInput( input );
	}
}

void PlayerController::Control( CGameObject* gameObject )
{
	Abandon( );

	SetGameObject( gameObject );
	if ( gameObject )
	{
		gameObject->SetInputController( this );
		gameObject->InitializeInputComponent( );
	}
}

void PlayerController::Abandon( )
{
	if ( m_gameObject )
	{
		m_gameObject->SetInputController( nullptr );
	}

	SetGameObject( nullptr );
}
