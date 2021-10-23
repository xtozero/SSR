#include "stdafx.h"
#include "Components/SceneComponent.h"

using namespace DirectX;

void SceneComponent::SetPosition( const float x, const float y, const float z )
{
	if ( m_vecPos.x == x && m_vecPos.y == y && m_vecPos.z == z )
	{
		return;
	}

	m_vecPos = CXMFLOAT3( x, y, z );
	m_needRebuildTransform = true;
}

void SceneComponent::SetPosition( const CXMFLOAT3& pos )
{
	SetPosition( pos.x, pos.y, pos.z );
}

void SceneComponent::SetScale( const float xScale, const float yScale, const float zScale )
{
	if ( m_vecScale.x == xScale && m_vecScale.y == yScale && m_vecScale.z == zScale )
	{
		return;
	}

	m_vecScale = CXMFLOAT3( xScale, yScale, zScale );
	m_needRebuildTransform = true;
}

void SceneComponent::SetRotate( const CXMFLOAT4& rotate )
{
	if ( m_vecRotate == rotate )
	{
		return;
	}

	m_vecRotate = rotate;
	m_needRebuildTransform = true;
}

void SceneComponent::SetRotate( const float pitch, const float yaw, const float roll )
{
	SetRotate( static_cast<CXMFLOAT4>( XMQuaternionRotationRollPitchYaw( pitch, yaw, roll ) ) );
}

void SceneComponent::SetRotate( const CXMFLOAT3& pitchYawRoll )
{
	SetRotate( static_cast<CXMFLOAT4>( XMQuaternionRotationRollPitchYaw( pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z ) ) );
}

const CXMFLOAT3& SceneComponent::GetPosition( ) const
{
	return m_vecPos;
}

const CXMFLOAT3& SceneComponent::GetScale( ) const
{
	return m_vecScale;
}

const CXMFLOAT4& SceneComponent::GetRotate( ) const
{
	return m_vecRotate;
}

const CXMFLOAT4X4& SceneComponent::GetTransformMatrix( )
{
	RebuildTransform( );
	return m_matTransform;
}

const CXMFLOAT4X4& SceneComponent::GetInvTransformMatrix( )
{
	RebuildTransform( );
	return m_invMatTransform;
}

BoxSphereBounds SceneComponent::CalcBounds( [[maybe_unused]] const CXMFLOAT4X4& transform )
{
	return BoxSphereBounds( CXMFLOAT3( 0, 0, 0 ), CXMFLOAT3( 0, 0, 0 ), 0.f );
}

void SceneComponent::UpdateBounds( )
{
	m_bounds = CalcBounds( GetTransformMatrix( ) );
}

void SceneComponent::RebuildTransform( )
{
	if ( m_needRebuildTransform )
	{
		//STR
		XMMATRIX scale;
		XMMATRIX rotate;

		scale = XMMatrixScaling( m_vecScale.x, m_vecScale.y, m_vecScale.z );
		rotate = XMMatrixRotationQuaternion( m_vecRotate );

		m_matTransform = scale * rotate;

		m_matTransform._41 = m_vecPos.x;
		m_matTransform._42 = m_vecPos.y;
		m_matTransform._43 = m_vecPos.z;

		m_invMatTransform = XMMatrixInverse( nullptr, m_matTransform );

		m_needRebuildTransform = false;
	}
}
