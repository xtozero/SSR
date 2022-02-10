#pragma once

#include "GameObject/GameObject.h"

#include <vector>

class InputComponent;
struct UserInput;

class InputController : public CGameObject
{
	GENERATE_CLASS_TYPE_INFO( InputController )

public:
	virtual void ProcessInput( const UserInput& input ) = 0;

	virtual void Control( CGameObject* gameObject ) = 0;
	virtual void Abandon( ) = 0;

	void SetGameObject( CGameObject* gameObject );

	InputController( );

protected:
	CGameObject* m_gameObject = nullptr;
};

class PlayerController : public InputController
{
public:
	virtual void ProcessInput( const UserInput& input ) override;

	virtual void Control( CGameObject* gameObject ) override;
	virtual void Abandon( ) override;

private:
	InputComponent* m_inputComponent = nullptr;
};
