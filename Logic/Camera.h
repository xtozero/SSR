#pragma once

#include "IListener.h"

#include "../shared/CDirectXMath.h"

#include <d3dX9math.h>

class IRenderer;

class CCamera : public IListener
{
public:
	//IListener
	virtual void OnLButtonDown( const int x, const int y ) override;
	virtual void OnLButtonUp( const int x, const int y ) override;
	virtual void OnRButtonDown( const int x, const int y ) override;
	virtual void OnRButtonUp( const int x, const int y ) override;
	virtual void OnMouseMove( const int x, const int y ) override;
	virtual void OnWheelMove( const int zDelta ) override;

	const CXMFLOAT4X4& CCamera::GetViewMatrix();
	
	void SetOrigin( const CXMFLOAT3& origin );
	const CXMFLOAT3& GetOrigin( ) const { return m_origin; }

	void Move( const float right, const float up, const float look );
	void Rotate( const float pitch, const float yaw, const float roll );
 
 	void UpdateToRenderer( IRenderer& renderer );
	const CXMFLOAT4X4& GetInvViewMatrix( ) const { return m_invViewMatrix; }
	void SetEnableRotate( bool isEnable ) { m_enableRotate = isEnable; }
private:
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

	bool m_isNeedReclac = false;
	bool m_isNeedUpdateRenderer = true;

	bool m_mouseRotateEnable = false;
	CXMFLOAT2 m_prevMouseEventPos = { 0.f, 0.f };
	
	bool m_mouseTranslateEnable = false;
	float m_mouseSensitivity = 0.01f;

	bool m_enableRotate = true;
};

