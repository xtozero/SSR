#include "stdafx.h"
#include "common.h"
#include "Camera.h"
#include "Timer.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/IRenderView.h"
#include "../shared/UserInput.h"
#include "../shared/Util.h"

#include <type_traits>

using namespace DirectX;

void CCamera::ProcessInput( const UserInput& input )
{
	switch ( input.m_code )
	{
	case UIC_MOUSE_MOVE:
		OnMouseMove( input );
		break;
	case UIC_MOUSE_LEFT:
		OnMouseLButton( input );
		break;
	case UIC_MOUSE_RIGHT:
		OnMouseRButton( input );
		break;
	case UIC_MOUSE_WHEELSPIN:
		OnWheelMove( input );
		break;
	case UIC_RIGHT:
	case UIC_LEFT:
	case UIC_UP:
	case UIC_DOWN:
		HandleKeyEvent( input );
		break;
	default:
		break;
	}
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

		view->SetViewMatrix( GetViewMatrix( ) );
		m_isNeedUpdateRenderer = false;
	}
}

void CCamera::OnMouseLButton( const UserInput& input )
{
	m_mouseRotateEnable = input.m_axis[2] < 0;
}

void CCamera::OnMouseRButton( const UserInput& input )
{
	m_mouseTranslateEnable = input.m_axis[2] < 0;
}

void CCamera::OnMouseMove( const UserInput& input )
{
	float dx = input.m_axis[0];
	float dy = input.m_axis[1];

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
}

void CCamera::OnWheelMove( const UserInput& input )
{
	Move( 0, 0, static_cast<float>( input.m_axis[2] ) );
}

void CCamera::HandleKeyEvent( const UserInput& input )
{
	if ( input.m_code == USER_INPUT_CODE::UIC_LEFT )
	{
		m_inputDirection[0] = ( input.m_axis[2] < 0 );
	}
	else if ( input.m_code == USER_INPUT_CODE::UIC_UP )
	{
		m_inputDirection[1] = ( input.m_axis[2] < 0 );
	}
	else if ( input.m_code == USER_INPUT_CODE::UIC_RIGHT )
	{
		m_inputDirection[2] = ( input.m_axis[2] < 0 );
	}
	else if ( input.m_code == USER_INPUT_CODE::UIC_DOWN )
	{
		m_inputDirection[3] = ( input.m_axis[2] < 0 );
	}

	CXMFLOAT2 force( static_cast<float>( m_inputDirection[2] - m_inputDirection[0] ), 
					static_cast<float>( m_inputDirection[1] - m_inputDirection[3] ) );
	Move( force.x, 0.f, force.y );
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
