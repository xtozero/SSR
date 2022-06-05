#pragma once

#include "Core/IListener.h"
#include "GameObject.h"
#include "Scene/INotifyGraphicsDevice.h"

class CameraComponent;

class CPlayer : public CGameObject, public IListener
{
	GENERATE_CLASS_TYPE_INFO( CPlayer )

public:
	virtual void Initialize( CGameLogic& gameLogic, World& world ) override;
	virtual void Think( float elapsedTime );
	virtual void ProcessInput( const UserInput& input, CGameLogic& gameLogic ) override;

	const CameraComponent* GetCameraComponent() const { return m_cameraComponent; }

	CPlayer();

protected:
	virtual void SetupInputComponent() override;

private:
	void OnMouseLButton( const UserInput& input, CGameLogic& gameLogic );
	void RotatePrimaryLightDir( CGameLogic& gameLogic, float deltaTheta, float deltaPhi );

	void OnMouseLButton( const UserInput& input );
	void OnMouseRButton( const UserInput& input );
	void OnMouseMove( const UserInput& input );
	void OnWheelMove( const UserInput& input );
	void OnMoveKey( const UserInput& input );

	CameraComponent* m_cameraComponent = nullptr;

	bool m_cameraRotationEnabled = false;
	bool m_cameraTranslationEnabled = false;
	float m_mouseSensitivity = 0.01f;

	bool m_inputDirection[4] = { false, false, false, false };
};
