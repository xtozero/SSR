#pragma once

#include "Core/IListener.h"
#include "Math/CXMFloat.h"
#include "Physics/Movement.h"
#include "Scene/INotifyGraphicsDevice.h"

class CRenderView;
class IRenderer;
class KeyValue;

class CCamera : public IListener, public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic );
	// IListener
	virtual void ProcessInput( const UserInput& input );

	void Think( );

	const CXMFLOAT4X4& CCamera::GetViewMatrix();
	
	void SetOrigin( const CXMFLOAT3& origin );
	const CXMFLOAT3& GetOrigin( ) const { return m_origin; }

	void Move( const float right, const float up, const float look );
	void Move( CXMFLOAT3 delta );
	void Rotate( const float pitch, const float yaw, const float roll );
 
 	void UpdateToRenderer( CRenderView& view );
	const CXMFLOAT4X4& GetInvViewMatrix( ) const { return m_invViewMatrix; }
	void SetEnableRotate( bool isEnable ) { m_enableRotate = isEnable; }

	void LoadProperty( const KeyValue& keyValue );

private:
	void OnMouseLButton( const UserInput& input );
	void OnMouseRButton( const UserInput& input );
	void OnMouseMove( const UserInput& input );
	void OnWheelMove( const UserInput& input );
	void HandleKeyEvent( const UserInput& input );

	void ReCalcViewMatrix( );
	void CameraChanged( )
	{
		m_isNeedReclac = true;
		m_isNeedUpdateRenderer = true;
	}

public:
	CCamera( );
	virtual ~CCamera( ) = default;

private:
	CGroundMovement m_movement;
	float m_kineticForceScale = 1.f;

	CXMFLOAT4X4 m_viewMatrix;
	CXMFLOAT4X4 m_invViewMatrix;
	
	CXMFLOAT3 m_origin;
	CXMFLOAT3 m_angles;
	CXMFLOAT3 m_lookVector = { 0.f, 0.f, 1.f };
	CXMFLOAT3 m_upVector = { 0.f, 1.f, 0.f };
	CXMFLOAT3 m_rightVector = { 1.f, 0.f, 0.f };

	bool m_inputDirection[4] = { false, false, false, false };

	bool m_isNeedReclac = false;
	bool m_isNeedUpdateRenderer = true;

	bool m_mouseRotateEnable = false;

	bool m_mouseTranslateEnable = false;
	float m_mouseSensitivity = 0.01f;

	bool m_enableRotate = true;
};

