#pragma once

#include "IListener.h"

#include "../shared/Math/CXMFloat.h"

class IRenderer;

class CCamera : public IListener
{
public:
	// IListener
	virtual void ProcessInput( const UserInput& input );

	const CXMFLOAT4X4& CCamera::GetViewMatrix();
	
	void SetOrigin( const CXMFLOAT3& origin );
	const CXMFLOAT3& GetOrigin( ) const { return m_origin; }

	void Move( const float right, const float up, const float look );
	void Rotate( const float pitch, const float yaw, const float roll );
 
 	void UpdateToRenderer( IRenderer& renderer );
	const CXMFLOAT4X4& GetInvViewMatrix( ) const { return m_invViewMatrix; }
	void SetEnableRotate( bool isEnable ) { m_enableRotate = isEnable; }

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
	virtual ~CCamera( );

private:
	CXMFLOAT4X4 m_viewMatrix;
	CXMFLOAT4X4 m_invViewMatrix;
	
	CXMFLOAT3 m_origin = { 0.f, 0.f, 0.f };
	CXMFLOAT3 m_angles = { 0.f, 0.f, 0.f };
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

