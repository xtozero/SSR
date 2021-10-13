#include "stdafx.h"
#include "Components/CameraComponent.h"

#include "common.h"
#include "Core/Timer.h"
#include "Json/json.hpp"
#include "UserInput/UserInput.h"

#include <type_traits>

using namespace DirectX;

void CameraComponent::Move( const float right, const float up, const float look )
{
	if ( right != 0.f || up != 0.f || look != 0.f )
	{
		CXMFLOAT3 deltaMove = ( right * m_rightVector + up * m_upVector + look * m_lookVector );
		const CXMFLOAT3& curPos = GetPosition( );
		SetPosition( curPos + deltaMove );
		MarkCameraTransformDirty( );
	}
}

void CameraComponent::Move( CXMFLOAT3 delta )
{
	Move( delta.x, delta.y, delta.z );
}

void CameraComponent::Rotate( const float pitch, const float yaw, const float roll )
{
	if ( m_enableRotate )
	{
		if ( pitch != 0.f || yaw != 0.f || roll != 0.f )
		{
			XMVECTOR curRotate = GetRotate( );
			XMVECTOR deltaRotate = XMQuaternionRotationRollPitchYaw( pitch, yaw, roll );
			CXMFLOAT4 newRotate = XMQuaternionMultiply( deltaRotate, curRotate );
			SetRotate( newRotate );

			XMMATRIX rotateMat = XMMatrixRotationQuaternion( GetRotate() );

			m_rightVector = XMVector3TransformCoord( g_XMIdentityR0, rotateMat );
			m_upVector = XMVector3TransformCoord( g_XMIdentityR1, rotateMat );
			m_lookVector = XMVector3TransformCoord( g_XMIdentityR2, rotateMat );

			m_lookVector = XMVector3Normalize( m_lookVector );

			m_upVector = XMVector3Cross( m_lookVector, m_rightVector );
			m_upVector = XMVector3Normalize( m_upVector );

			m_rightVector = XMVector3Cross( m_upVector, m_lookVector );
			m_rightVector = XMVector3Normalize( m_rightVector );

			MarkCameraTransformDirty( );
		}
	}
}

const CXMFLOAT4X4& CameraComponent::GetViewMatrix( ) const
{
	ReCalcViewMatrix( );
	return m_viewMatrix;
}

const CXMFLOAT4X4 & CameraComponent::GetInvViewMatrix( ) const
{
	ReCalcViewMatrix( );
	return m_invViewMatrix;
}

void CameraComponent::LoadProperty( CGameLogic& gameLogic, const JSON::Value& json )
{
	if ( const JSON::Value* pPos = json.Find( "Position" ) )
	{
		const JSON::Value& pos = *pPos;

		if ( pos.Size( ) == 3 )
		{
			CXMFLOAT3 origin( static_cast<float>( pos[0].AsReal( ) ),
				static_cast<float>( pos[1].AsReal( ) ),
				static_cast<float>( pos[2].AsReal( ) ) );
			SetPosition( origin );
		}
	}
}

CameraComponent::CameraComponent( CGameObject* pOwner ) : SceneComponent( pOwner )
{
	m_viewMatrix = XMMatrixIdentity( );
	m_invViewMatrix = XMMatrixIdentity( );
}

void CameraComponent::ReCalcViewMatrix( ) const
{
	if ( m_needRecalc )
	{
		m_viewMatrix._11 = m_rightVector.x;	m_viewMatrix._12 = m_upVector.x; m_viewMatrix._13 = m_lookVector.x;
		m_viewMatrix._21 = m_rightVector.y;	m_viewMatrix._22 = m_upVector.y; m_viewMatrix._23 = m_lookVector.y;
		m_viewMatrix._31 = m_rightVector.z;	m_viewMatrix._32 = m_upVector.z; m_viewMatrix._33 = m_lookVector.z;

		XMVECTOR origin = GetPosition( );

		m_viewMatrix._41 = -XMVectorGetX( XMVector3Dot( origin, m_rightVector ) );
		m_viewMatrix._42 = -XMVectorGetX( XMVector3Dot( origin, m_upVector ) );
		m_viewMatrix._43 = -XMVectorGetX( XMVector3Dot( origin, m_lookVector ) );

		m_invViewMatrix = XMMatrixInverse( nullptr, m_viewMatrix );

		m_needRecalc = false;
	}
}
