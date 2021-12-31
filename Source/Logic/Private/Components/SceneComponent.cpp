#include "stdafx.h"
#include "Components/SceneComponent.h"

#include "Math/TransformationMatrix.h"

using namespace DirectX;

void SceneComponent::SetPosition( const float x, const float y, const float z )
{
	if ( m_vecPos.x == x && m_vecPos.y == y && m_vecPos.z == z )
	{
		return;
	}

	m_vecPos = Vector( x, y, z );
	m_needRebuildTransform = true;
}

void SceneComponent::SetPosition( const Vector& pos )
{
	SetPosition( pos.x, pos.y, pos.z );
}

void SceneComponent::SetScale( const float xScale, const float yScale, const float zScale )
{
	if ( m_vecScale.x == xScale && m_vecScale.y == yScale && m_vecScale.z == zScale )
	{
		return;
	}

	m_vecScale = Vector( xScale, yScale, zScale );
	m_needRebuildTransform = true;
}

void SceneComponent::SetRotate( const Quaternion& rotate )
{
	if ( m_vecRotate == rotate )
	{
		return;
	}

	m_vecRotate = rotate;
	m_needRebuildTransform = true;
}

const Vector& SceneComponent::GetPosition( ) const
{
	return m_vecPos;
}

const Vector& SceneComponent::GetScale( ) const
{
	return m_vecScale;
}

const Quaternion& SceneComponent::GetRotate( ) const
{
	return m_vecRotate;
}

const Matrix& SceneComponent::GetTransformMatrix( )
{
	RebuildTransform( );
	return m_matTransform;
}

const Matrix& SceneComponent::GetInvTransformMatrix( )
{
	RebuildTransform( );
	return m_invMatTransform;
}

BoxSphereBounds SceneComponent::CalcBounds( [[maybe_unused]] const Matrix& transform )
{
	return BoxSphereBounds( Vector( 0, 0, 0 ), Vector( 0, 0, 0 ), 0.f );
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
		auto scale = ScaleMatrix( m_vecScale );
		auto rotate = RotateMatrix( m_vecRotate );

		m_matTransform = scale * rotate;

		m_matTransform._41 = m_vecPos.x;
		m_matTransform._42 = m_vecPos.y;
		m_matTransform._43 = m_vecPos.z;

		m_invMatTransform = m_matTransform.Inverse();

		m_needRebuildTransform = false;
	}
}
