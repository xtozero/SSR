#pragma once

#include "SceneComponent.h"
#include "Core/IListener.h"
#include "Math/CXMFloat.h"
#include "Physics/Movement.h"
#include "Scene/INotifyGraphicsDevice.h"

class IRenderer;

namespace JSON
{
	class Value;
}

class CameraComponent : public SceneComponent, public IListener
{
public:
	// IListener
	virtual void ProcessInput( const UserInput& input, CGameLogic& gameLogic ) override;

	void Think( float elapsedTime );
	
	const CXMFLOAT3& GetForwardVector( ) const { return m_lookVector; }
	const CXMFLOAT3& GetRightVector( ) const { return m_rightVector; }
	const CXMFLOAT3& GetUpVector( ) const { return m_upVector; }

	void Move( const float right, const float up, const float look );
	void Move( CXMFLOAT3 delta );
	void Rotate( const float pitch, const float yaw, const float roll );
 
	const CXMFLOAT4X4& GetViewMatrix( ) const;
	const CXMFLOAT4X4& GetInvViewMatrix( ) const;
	void SetEnableRotate( bool isEnable ) { m_enableRotate = isEnable; }

	void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json );

	CameraComponent( CGameObject* pOwner );

private:
	void OnMouseLButton( const UserInput& input );
	void OnMouseRButton( const UserInput& input );
	void OnMouseMove( const UserInput& input );
	void OnWheelMove( const UserInput& input );
	void HandleKeyEvent( const UserInput& input );

	void ReCalcViewMatrix( ) const;
	void MarkCameraTransformDirty( )
	{
		m_needRecalc = true;
	}

private:
	CGroundMovement m_movement;
	float m_kineticForceScale = 1.f;

	mutable CXMFLOAT4X4 m_viewMatrix;
	mutable CXMFLOAT4X4 m_invViewMatrix;
	
	CXMFLOAT3 m_lookVector = { 0.f, 0.f, 1.f };
	CXMFLOAT3 m_upVector = { 0.f, 1.f, 0.f };
	CXMFLOAT3 m_rightVector = { 1.f, 0.f, 0.f };

	bool m_inputDirection[4] = { false, false, false, false };

	mutable bool m_needRecalc = false;

	bool m_mouseRotateEnable = false;

	bool m_mouseTranslateEnable = false;
	float m_mouseSensitivity = 0.01f;

	bool m_enableRotate = true;
};

