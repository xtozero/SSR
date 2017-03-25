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

void CCamera::SetOrigin( const XMFLOAT3& origin )
{
	CameraChanged( );
	m_origin = origin;
}

void CCamera::Move( const float right, const float up, const float look )
{
	CameraChanged( );

	XMVECTOR newOrigin;
	newOrigin = XMLoadFloat3( &m_origin );

	if ( right != 0 )
	{
		XMVECTOR rightVector = XMLoadFloat3( &m_rightVector );
		newOrigin += right * rightVector;
	}

	if ( up != 0 )
	{
		XMVECTOR upVector = XMLoadFloat3( &m_upVector );
		newOrigin += up * upVector;
	}

	if ( look != 0 )
	{
		XMVECTOR lookVector = XMLoadFloat3( &m_rightVector );
		newOrigin += look * lookVector;
	}

	XMStoreFloat3( &m_origin, newOrigin );
}

void CCamera::Rotate( const float pitch, const float yaw, const float roll )
{
	if ( m_enableRotate )
	{
		if ( pitch != 0.f || yaw != 0.f || roll != 0.f )
		{
			m_angles.x += pitch;
			m_angles.y += yaw;
			m_angles.z += roll;

			constexpr float angleLimit = XM_PI * 2.f;
			if ( m_angles.x >= angleLimit )
			{
				m_angles.x -= angleLimit;
			}
			
			if ( m_angles.y >= angleLimit )
			{
				m_angles.y -= angleLimit;
			}
			
			if ( m_angles.z >= angleLimit )
			{
				m_angles.z -= angleLimit;
			}


			CameraChanged( );
			XMMATRIX rotateMat = XMMatrixRotationRollPitchYaw( m_angles.x, m_angles.y, m_angles.z );

			XMVECTOR rightVector = XMVector3TransformCoord( g_XMIdentityR0, rotateMat );
			XMVECTOR upVector = XMVector3TransformCoord( g_XMIdentityR1, rotateMat );
			XMVECTOR lookVector = XMVector3TransformCoord( g_XMIdentityR2, rotateMat );

			XMStoreFloat3( &m_rightVector, rightVector );
			XMStoreFloat3( &m_upVector, upVector );
			XMStoreFloat3( &m_lookVector, lookVector );
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
	XMVECTOR rightVector = XMLoadFloat3( &m_rightVector );
	XMVECTOR upVector = XMLoadFloat3( &m_upVector );
	XMVECTOR lookVector = XMLoadFloat3( &m_lookVector );

	lookVector = XMVector3Normalize( lookVector );
	
	upVector = XMVector3Cross( lookVector, rightVector );
	upVector = XMVector3Normalize( upVector );
	
	rightVector = XMVector3Cross( upVector, lookVector );
	rightVector = XMVector3Normalize( rightVector );

	XMStoreFloat3( &m_rightVector, rightVector );
	XMStoreFloat3( &m_upVector, upVector );
	XMStoreFloat3( &m_lookVector, lookVector );

	m_viewMatrix._11 = m_rightVector.x;	m_viewMatrix._12 = m_upVector.x; m_viewMatrix._13 = m_lookVector.x;
	m_viewMatrix._21 = m_rightVector.y;	m_viewMatrix._22 = m_upVector.y; m_viewMatrix._23 = m_lookVector.y;
	m_viewMatrix._31 = m_rightVector.z;	m_viewMatrix._32 = m_upVector.z; m_viewMatrix._33 = m_lookVector.z;

	XMVECTOR origin = XMLoadFloat3( &m_origin );

	m_viewMatrix._41 = -XMVectorGetX( XMVector3Dot( origin, rightVector ) );
	m_viewMatrix._42 = -XMVectorGetX( XMVector3Dot( origin, upVector ) );
	m_viewMatrix._43 = -XMVectorGetX( XMVector3Dot( origin, lookVector ) );

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
