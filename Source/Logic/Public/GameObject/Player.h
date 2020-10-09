#pragma once

#include "Core/IListener.h"
#include "Scene/INotifyGraphicsDevice.h"

class CameraComponent;

class CPlayer : public CGameObject, public IListener
{
public:
	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json ) override;
	virtual void ProcessInput( const UserInput& input, CGameLogic& gameLogic ) override;

	const CameraComponent* GetCameraComponent( ) const { return m_cameraComponent; }

	CPlayer( );

private:
	void OnMouseLButton( const UserInput& input, CGameLogic& gameLogic );
	void RotatePrimaryLightDir( CGameLogic& gameLogic, float deltaTheta, float deltaPhi );

	CameraComponent* m_cameraComponent;
};
