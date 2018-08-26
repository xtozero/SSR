#include "stdafx.h"
#include "GameObject/Camera.h"

#include "common.h"
#include "Core/Timer.h"
#include "DataStructure/KeyValueReader.h"
#include "Render/IRenderer.h"
#include "Scene/RenderView.h"
#include "UserInput/UserInput.h"
#include "Util.h"

#include <type_traits>

using namespace DirectX;

void CCamera::OnDeviceRestore( CGameLogic & gameLogic )
{
	CameraChanged( );
}

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

void CCamera::Think( )
{
	float elapsedTime = CTimer::GetInstance( ).GetElapsedTIme( );

	CXMFLOAT3 force( static_cast<float>( m_inputDirection[2] - m_inputDirection[0] ),
					0.f,
					static_cast<float>( m_inputDirection[1] - m_inputDirection[3] ) );

	force *= m_kineticForceScale;
	m_movement.Update( force, elapsedTime );
	Move( m_movement.GetDelta( elapsedTime ) );
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

void CCamera::Move( CXMFLOAT3 delta )
{
	Move( delta.x, delta.y, delta.z );
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
				if ( elem >= angleLimit )
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

void CCamera::UpdateToRenderer( CRenderView& view )
{
	if ( m_isNeedUpdateRenderer )
	{
		view.SetViewMatrix( GetViewMatrix( ) );
		m_isNeedUpdateRenderer = false;
	}
}

void CCamera::LoadProperty( const KeyValue& keyValue )
{
	if ( const KeyValue* pCamera = keyValue.Find( _T( "Camera" ) ) )
	{
		if ( const KeyValue* pPos = pCamera->Find( _T( "Position" ) ) )
		{
			std::vector<String> param;
			UTIL::Split( pPos->GetValue( ), param, ' ' );

			if ( param.size( ) == 3 )
			{
				CXMFLOAT3 origin( static_cast<float>( _ttof( param[0].c_str( ) ) ),
					static_cast<float>( _ttof( param[1].c_str( ) ) ),
					static_cast<float>( _ttof( param[2].c_str( ) ) ) );
				SetOrigin( origin );
			}
		}
		
		if ( const KeyValue* pMaxForce = pCamera->Find( _T( "Max_Force" ) ) )
		{
			m_movement.SetMaxForceMagnitude( static_cast<float>( _ttof( pMaxForce->GetValue( ).c_str( ) ) ) );
		}
		
		if ( const KeyValue* pFriction = pCamera->Find( _T( "Friction" ) ) )
		{
			std::vector<String> param;
			UTIL::Split( pFriction->GetValue( ), param, ' ' );

			if ( param.size( ) == 2 )
			{
				CXMFLOAT2 friction( static_cast<float>( _ttof( param[0].c_str( ) ) ),
					static_cast<float>( _ttof( param[1].c_str( ) ) ) );
				m_movement.SetFriction( friction );
			}
		}
		
		if ( const KeyValue* pForceScale = pCamera->Find( _T( "Kinetic_Force_Scale" ) ) )
		{
			m_kineticForceScale = pForceScale->GetValue<float>( );
		}
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
