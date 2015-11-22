#include "stdafx.h"
#include "common.h"
#include "Camera.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/IRenderView.h"

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
	if ( pitch != 0.f || yaw != 0.f || roll != 0.f )
	{
		CameraChanged( );
		D3DXMATRIX rotateMat;
		D3DXMatrixRotationYawPitchRoll( &rotateMat, yaw, pitch, roll );

		D3DXVec3TransformNormal( &m_lookVector, &m_lookVector, &rotateMat );
		D3DXVec3TransformNormal( &m_upVector, &m_upVector, &rotateMat );
		D3DXVec3TransformNormal( &m_rightVector, &m_rightVector, &rotateMat );
	}
}

void CCamera::UpdateToRenderer( IRenderer* pRenderer )
{
	if ( m_isNeedUpdateRenderer )
	{
		IRenderView* view = pRenderer->GetCurrentRenderView( );
		view->SetViewMatrix( GetViewMatrix() );
		m_isNeedUpdateRenderer = false;
		DebugMsg( "%s\n", "Camera Matrix Updated To Renderer" );
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
	m_viewMatrix._41 = -m_origin.x;	m_viewMatrix._42 = -m_origin.y;	m_viewMatrix._43 = -m_origin.z;
	
	m_isNeedReclac = false;
}

CCamera::CCamera( ) :
m_origin( 0.f, 0.f, 0.f ),
m_lookVector( 0.f, 0.f, 1.f ),
m_upVector( 0.f, 1.f, 0.f ),
m_rightVector( 1.f, 0.f, 0.f ),
m_isNeedReclac( false ),
m_isNeedUpdateRenderer( true )
{
	D3DXMatrixIdentity( &m_viewMatrix );
}


CCamera::~CCamera( )
{
}