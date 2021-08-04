#pragma once

#include "Core/IListener.h"
#include "GameObject.h"
#include "Physics/Movement.h"
#include "Scene/INotifyGraphicsDevice.h"

class CameraComponent;

class CPlayer : public CGameObject, public IListener
{
public:
	virtual void Initialize( CGameLogic& gameLogic, World& world ) override;
	virtual void Think( float elapsedTime );
	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json ) override;
	virtual void ProcessInput( const UserInput& input, CGameLogic& gameLogic ) override;

	const CameraComponent* GetCameraComponent( ) const { return m_cameraComponent; }

	CPlayer( );

protected:
	virtual void SetupInputComponent( ) override;
	
private:
	void OnMouseLButton( const UserInput& input, CGameLogic& gameLogic );
	void RotatePrimaryLightDir( CGameLogic& gameLogic, float deltaTheta, float deltaPhi );

	void OnMouseLButton( const UserInput& input );
	void OnMouseRButton( const UserInput& input );
	void OnMouseMove( const UserInput& input );
	void OnWheelMove( const UserInput& input );
	void OnMoveKey( const UserInput& input );

	CameraComponent* m_cameraComponent = nullptr;

	bool m_cameraRotateEnable = false;
	bool m_cameraTranslateEnable = false;
	float m_mouseSensitivity = 0.01f;

	CGroundMovement m_movement;
	float m_kineticForceScale = 1.f;
	bool m_inputDirection[4] = { false, false, false, false };
};
