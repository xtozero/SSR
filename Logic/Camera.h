#pragma once

#include "common.h"
#include "IListener.h"

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

	const D3DXMATRIX& CCamera::GetViewMatrix();
	
	void SetOrigin( const XMFLOAT3& origin );
	const XMFLOAT3& GetOrigin( ) const { return m_origin; }

	void Move( const float right, const float up, const float look );
	void Rotate( const float pitch, const float yaw, const float roll );
 
 	void UpdateToRenderer( IRenderer& renderer );
	const D3DXMATRIX& GetInvViewMatrix( ) const { return m_invViewMatrix; }
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
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_invViewMatrix;
	
	XMFLOAT3 m_origin;
	XMFLOAT3 m_angles;
	XMFLOAT3 m_lookVector;
	XMFLOAT3 m_upVector;
	XMFLOAT3 m_rightVector;

	bool m_isNeedReclac = false;
	bool m_isNeedUpdateRenderer = true;

	bool m_mouseRotateEnable = false;
	D3DXVECTOR2 m_prevMouseEventPos;
	
	bool m_mouseTranslateEnable = false;
	float m_mouseSensitivity = 0.01f;

	bool m_enableRotate = true;
};

