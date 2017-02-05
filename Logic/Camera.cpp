#include "stdafx.h"
#include "common.h"
#include "Camera.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/IRenderView.h"
#include "../shared/Util.h"

#include <type_traits>

void CCamera::OnLButtonDown( const int x, const int y )
{
	m_mouseRotateEnable = true;
}

void CCamera::OnLButtonUp( const int x, const int y )
{
	m_mouseRotateEnable = false;
}

void CCamera::OnRButtonDown( const int x, const int y )
{
	m_mouseTranslateEnable = true;
}

void CCamera::OnRButtonUp( const int x, const int y )
{
	m_mouseTranslateEnable = false;
}

void CCamera::OnMouseMove( const int x, const int y )
{
	float xPos = static_cast<float>( x );
	float yPos = static_cast<float>( y );
	float dx = xPos - m_prevMouseEventPos.x;
	float dy = yPos - m_prevMouseEventPos.y;

	dx *= m_mouseSensitivity;
	dy *= m_mouseSensitivity;

	if ( m_mouseRotateEnable )
	{
		Rotate( dy, dx, 0 );
	}
	else if ( m_mouseTranslateEnable )
	{
		Move( dx, dy, 0 );
	}

	m_prevMouseEventPos.x = xPos;
	m_prevMouseEventPos.y = yPos;
}

void CCamera::OnWheelMove( const int zDelta )
{
	Move( 0, 0, static_cast<float>( zDelta ) );
}

const D3DXMATRIX& CCamera::GetViewMatrix( )
{
	if ( m_isNeedReclac )
	{
		ReCalcViewMatrix( );
	}

	return m_viewMatrix; 
}

void CCamera::SetOrigin( const D3DXVECTOR3& origin )
{
	CameraChanged( );
	m_origin = origin;
}

void CCamera::Move( const float right, const float up, const float look )
{
	CameraChanged( );

	if ( right != 0 )
	{
		m_origin += right * m_rightVector;
	}

	if ( up != 0 )
	{
		m_origin += up * m_upVector;
	}

	if ( look != 0 )
	{
		m_origin += look * m_lookVector;
	}
}

void CCamera::Rotate( const float pitch, const float yaw, const float roll )
{
	if ( m_enableRotate )
	{
		if ( pitch != 0.f || yaw != 0.f || roll != 0.f )
		{
			m_angles += D3DXVECTOR3( pitch, yaw, roll );

			constexpr float angleLimit = D3DX_PI * 2.f;
			for ( int i = 0; i < 3; ++i )
			{
				if ( m_angles[i] >= angleLimit )
				{
					m_angles[i] -= angleLimit;
				}
			}

			CameraChanged( );
			D3DXMATRIX rotateMat;
			D3DXMatrixRotationYawPitchRoll( &rotateMat, m_angles.y, m_angles.x, m_angles.z );

			D3DXVec3TransformNormal( &m_lookVector, &D3DXVECTOR3( 0.f, 0.f, 1.0f ), &rotateMat );
			D3DXVec3TransformNormal( &m_upVector, &D3DXVECTOR3( 0.f, 1.f, 0.0f ), &rotateMat );
			D3DXVec3TransformNormal( &m_rightVector, &D3DXVECTOR3( 1.f, 0.f, 0.0f ), &rotateMat );
		}
	}
}

void CCamera::UpdateToRenderer( IRenderer& renderer )
{
	if ( m_isNeedUpdateRenderer )
	{
		IRenderView* view = renderer.GetCurrentRenderView( );
		view->SetViewMatrix( GetViewMatrix() );
		m_isNeedUpdateRenderer = false;
	}
}

void CCamera::ReCalcViewMatrix( )
{
	D3DXVec3Normalize( &m_lookVector, &m_lookVector );
	
	D3DXVec3Cross( &m_upVector, &m_lookVector, &m_rightVector );
	D3DXVec3Normalize( &m_upVector, &m_upVector );
	
	D3DXVec3Cross( &m_rightVector, &m_upVector, &m_lookVector );
	D3DXVec3Normalize( &m_rightVector, &m_rightVector );

	m_viewMatrix._11 = m_rightVector.x;	m_viewMatrix._12 = m_upVector.x;	m_viewMatrix._13 = m_lookVector.x;
	m_viewMatrix._21 = m_rightVector.y;	m_viewMatrix._22 = m_upVector.y;	m_viewMatrix._23 = m_lookVector.y;
	m_viewMatrix._31 = m_rightVector.z;	m_viewMatrix._32 = m_upVector.z;	m_viewMatrix._33 = m_lookVector.z;
	m_viewMatrix._41 = -D3DXVec3Dot( &m_origin, &m_rightVector );
	m_viewMatrix._42 = -D3DXVec3Dot( &m_origin, &m_upVector );
	m_viewMatrix._43 = -D3DXVec3Dot( &m_origin, &m_lookVector );

	D3DXMatrixInverse( &m_invViewMatrix, nullptr, &m_viewMatrix );

	m_isNeedReclac = false;
}

CCamera::CCamera( ) :
m_origin( 0.f, 0.f, 0.f ),
m_angles( 0.f, 0.f, 0.f ),
m_lookVector( 0.f, 0.f, 1.f ),
m_upVector( 0.f, 1.f, 0.f ),
m_rightVector( 1.f, 0.f, 0.f ),
m_prevMouseEventPos( 0.f, 0.f )
{
	D3DXMatrixIdentity( &m_viewMatrix );
	D3DXMatrixIdentity( &m_invViewMatrix );
}


CCamera::~CCamera( )
{
}
