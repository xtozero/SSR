#pragma once

#include <D3DX9math.h>

class IRenderer;

class CCamera
{
public:
	const D3DXMATRIX& CCamera::GetViewMatrix();
	
	void SetOrigin( const D3DXVECTOR3& origin );
	const D3DXVECTOR3& GetOrigin( ) { return m_origin; }

	void Move( const float right, const float up, const float look );
	void Rotate( const float pitch, const float yaw, const float roll );
 
 	void UpdateToRenderer( IRenderer* pRenderer );
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

	D3DXVECTOR3 m_origin;
	D3DXVECTOR3 m_lookVector;
	D3DXVECTOR3 m_upVector;
	D3DXVECTOR3 m_rightVector;

	bool m_isNeedReclac;
	bool m_isNeedUpdateRenderer;
};

