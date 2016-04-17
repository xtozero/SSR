#pragma once

#include <d3dX9math.h>
#include "IListener.h"

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
	
	void SetOrigin( const D3DXVECTOR3& origin );
	const D3DXVECTOR3& GetOrigin( ) const { return m_origin; }

	void Move( const float right, const float up, const float look );
	void Rotate( const float pitch, const float yaw, const float roll );
 
 	void UpdateToRenderer( IRenderer* pRenderer );
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
	
	D3DXVECTOR3 m_origin;
	D3DXVECTOR3 m_lookVector;
	D3DXVECTOR3 m_upVector;
	D3DXVECTOR3 m_rightVector;

	bool m_isNeedReclac;
	bool m_isNeedUpdateRenderer;

	bool m_mouseRotateEnable;
	D3DXVECTOR2 m_prevMouseEventPos;
	
	bool m_mouseTranslateEnable;
	float m_mouseSensitivity;

	bool m_enableRotate;
};

