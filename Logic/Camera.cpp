#include "stdafx.h"
#include "common.h"
#include "Camera.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/IRenderView.h"
#include "../shared/Util.h"

#include <type_traits>

using namespace DirectX;

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

const CXMFLOAT4X4& CCamera::GetViewMatrix( )
{
	if ( m_isNeedReclac )
	{
		ReCalcViewMatrix( );
	}

	return m_viewMatrix; 
}

void CCamera::SetOrigin( const CXMFLOAT3& origin )
{
	CameraChanged( );
	m_origin = origin;
}

void CCamera::Move( const float right, const float up, const float look )
{
	CameraChanged( );

	if ( right != 0.f )
	{
		m_origin += right * m_rightVector;
	}

	if ( up != 0.f )
	{
		m_origin += up * m_upVector;
	}

	if ( look != 0.f )
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
			m_angles += CXMFLOAT3( pitch, yaw, roll );

			constexpr float angleLimit = XM_PI * 2.f;
			for ( auto& elem : m_angles )
			{
				if ( angleLimit >= angleLimit )
				{
					elem -= angleLimit;
				}
			}

			CameraChanged( );
			XMMATRIX rotateMat = XMMatrixRotationRollPitchYaw( m_angles.x, m_angles.y, m_angles.z );

			m_rightVector = XMVector3TransformCoord( g_XMIdentityR0, rotateMat );
			m_upVector = XMVector3TransformCoord( g_XMIdentityR1, rotateMat );
			m_lookVector = XMVector3TransformCoord( g_XMIdentityR2, rotateMat );
		}
	}
}

void CCamera::UpdateToRenderer( IRenderer& renderer )
{
	if ( m_isNeedUpdateRenderer )
	{
		IRenderView* view = renderer.GetCurrentRenderView( );

		// fix after refactoring render module
		CXMFLOAT4X4 mat = GetViewMatrix( );
		D3DXMATRIX temp;

		for ( int i = 0; i < 4; ++i )
		{
			for ( int j = 0; j < 4; ++j )
			{
				temp( j, i ) = mat( j, i );
			}
		}

		view->SetViewMatrix( temp );
		m_isNeedUpdateRenderer = false;
	}
}

void CCamera::ReCalcViewMatrix( )
{
	XMVECTOR rightVector = m_rightVector;
	XMVECTOR upVector = m_upVector;
	XMVECTOR lookVector = m_lookVector;

	lookVector = XMVector3Normalize( lookVector );
	
	upVector = XMVector3Cross( lookVector, rightVector );
	upVector = XMVector3Normalize( upVector );
	
	rightVector = XMVector3Cross( upVector, lookVector );
	rightVector = XMVector3Normalize( rightVector );

	m_rightVector = rightVector;
	m_upVector = upVector;
	m_lookVector = lookVector;

	m_viewMatrix._11 = m_rightVector.x;	m_viewMatrix._12 = m_upVector.x; m_viewMatrix._13 = m_lookVector.x;
	m_viewMatrix._21 = m_rightVector.y;	m_viewMatrix._22 = m_upVector.y; m_viewMatrix._23 = m_lookVector.y;
	m_viewMatrix._31 = m_rightVector.z;	m_viewMatrix._32 = m_upVector.z; m_viewMatrix._33 = m_lookVector.z;

	XMVECTOR origin = m_origin;

	m_viewMatrix._41 = -XMVectorGetX( XMVector3Dot( origin, rightVector ) );
	m_viewMatrix._42 = -XMVectorGetX( XMVector3Dot( origin, upVector ) );
	m_viewMatrix._43 = -XMVectorGetX( XMVector3Dot( origin, lookVector ) );

	m_invViewMatrix = XMMatrixInverse( nullptr, m_viewMatrix );

	m_isNeedReclac = false;
}

CCamera::CCamera( )
{
	m_viewMatrix = XMMatrixIdentity( );
	m_invViewMatrix = XMMatrixIdentity( );
}


CCamera::~CCamera( )
{
}
