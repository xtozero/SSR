#include "stdafx.h"
#include "Physics/Body.h"

using namespace DirectX;

namespace
{
	constexpr float g_sleepEpsilon = 0.3f;
}

void RigidBody::SetMass( float mass )
{
	assert( mass != 0.f );
	m_inverseMass = 1.f / mass;
}

float RigidBody::GetMass( ) const
{
	if ( m_inverseMass == 0 )
	{
		return FLT_MAX;
	}
	else
	{
		return 1.f / m_inverseMass;
	}
}

void RigidBody::SetInverseMass( float inverseMass )
{
	m_inverseMass = inverseMass;
}

float RigidBody::GetInverseMass( ) const
{
	return m_inverseMass;
}

bool RigidBody::HasFiniteMass( ) const
{
	return m_inverseMass > 0.f;
}

void RigidBody::SetDamping( float linearDamping, float angularDamping )
{
	m_linearDamping = linearDamping;
	m_angularDamping = angularDamping;
}

void RigidBody::SetLinearDamping( float linearDamping )
{
	m_linearDamping = linearDamping;
}

float RigidBody::GetLinearDamping( ) const
{
	return m_linearDamping;
}

void RigidBody::SetAngularDamping( float angularDamping )
{
	m_angularDamping = angularDamping;
}

float RigidBody::GetAngularDamping( ) const
{
	return m_angularDamping;
}

void RigidBody::SetPosition( const CXMFLOAT3& position )
{
	m_position = position;
}

CXMFLOAT3 RigidBody::GetPosition( ) const
{
	return m_position;
}

void RigidBody::SetOrientation( const CXMFLOAT4& orientation )
{
	m_orientation = orientation;
	m_orientation = XMQuaternionNormalize( m_orientation );
}

void RigidBody::SetOrientation( float pitch, float yaw, float roll )
{
	SetOrientation( XMQuaternionRotationRollPitchYaw( pitch, yaw, roll ) );
}

CXMFLOAT4 RigidBody::GetOrientation( ) const
{
	return m_orientation;
}

void RigidBody::SetVelocity( const CXMFLOAT3& velocity )
{
	m_velocity = velocity;
}

CXMFLOAT3 RigidBody::GetVelocity( ) const
{
	return m_velocity;
}

void RigidBody::AddVelocity( const CXMFLOAT3& deltaVelocity )
{
	m_velocity += deltaVelocity;
}

void RigidBody::SetRotation( const CXMFLOAT3& rotation )
{
	m_rotation = rotation;
}

CXMFLOAT3 RigidBody::GetRotation( ) const
{
	return m_rotation;
}

void RigidBody::AddRotation( const CXMFLOAT3& deltaRotation )
{
	m_rotation += deltaRotation;
}

void RigidBody::SetAwake( bool awake )
{
	m_isAwake = awake;

	if ( awake )
	{
		m_motion = g_sleepEpsilon * 2.f;
	}
	else
	{
		m_velocity = { 0.f, 0.f, 0.f };
		m_rotation = { 0.f, 0.f, 0.f };
	}
}

void RigidBody::SetCanSleep( bool canSleep )
{
	m_canSleep = canSleep;

	if ( m_canSleep == false && m_isAwake == false )
	{
		SetAwake( );
	}
}

CXMFLOAT4X4 RigidBody::GetTransform( ) const
{
	return m_transformMatrix;
}

void RigidBody::SetInertiaTensor( const CXMFLOAT3X3& inertiaTensor )
{
	m_inverseInertiaTensor = XMMatrixInverse( nullptr, inertiaTensor );

	if ( XMMatrixIsNaN( m_inverseInertiaTensor ) )
	{
		m_inverseInertiaTensor = CXMFLOAT3X3( 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f );
	}
}

CXMFLOAT3X3 RigidBody::GetInertiaTensor( ) const
{
	return XMMatrixInverse( nullptr, m_inverseInertiaTensor );
}

void RigidBody::SetInverseInertiaTensor( const CXMFLOAT3X3& inverseInertiaTensor )
{
	m_inverseInertiaTensor = inverseInertiaTensor;
}

CXMFLOAT3X3 RigidBody::GetInverseInertiaTensor( ) const
{
	return m_inverseInertiaTensor;
}

CXMFLOAT3X3 RigidBody::GetInertiaTensorWorld( ) const
{
	return XMMatrixInverse( nullptr, m_inverseInertiaTenserWorld );
}

CXMFLOAT3X3 RigidBody::GetInverseInertiaTensorWorld( ) const
{
	return m_inverseInertiaTenserWorld;
}

CXMFLOAT3 RigidBody::GetPointInWorldSpace( const CXMFLOAT3& point )
{
	return XMVector3TransformCoord( point, m_transformMatrix );
}

void RigidBody::AddForce( const CXMFLOAT3& force )
{
	m_forceAccum += force;
	m_isAwake = true;
}

void RigidBody::AddForceAtPoint( const CXMFLOAT3& force, const CXMFLOAT3& point )
{
	CXMFLOAT3 pt = GetPointInWorldSpace( point );
	AddForceAtBodyPoint( force, pt );
}

void RigidBody::AddForceAtBodyPoint( const CXMFLOAT3& force, const CXMFLOAT3& point )
{
	CXMFLOAT3 pt = point;
	pt -= m_position;
	m_forceAccum += force;
	m_torqueAccum += XMVector3Cross( pt, force );
	m_isAwake = true;
}

void RigidBody::AddTouque( const CXMFLOAT3& touque )
{
	m_torqueAccum += touque;
	m_isAwake = true;
}

void RigidBody::SetAcceleration( const CXMFLOAT3& acceleration )
{
	m_acceleration = acceleration;
}

CXMFLOAT3 RigidBody::GetAcceleration( )
{
	return m_acceleration;
}

CXMFLOAT3 RigidBody::GetLastFrameAcceleration( )
{
	return m_lastFrameAcceleration;
}

void RigidBody::ClearAccumulators( )
{
	m_forceAccum = { 0.f, 0.f, 0.f };
	m_torqueAccum = { 0.f, 0.f, 0.f };
}

void RigidBody::Integrate( float duration )
{
	if ( m_isAwake == false )
	{
		return;
	}

	if ( HasFiniteMass( ) == false ) 
	{
		SetAwake( false );
		return;
	}

	m_lastFrameAcceleration = m_acceleration;
	m_lastFrameAcceleration += m_forceAccum * m_inverseMass;

	CXMFLOAT3 angularAcceleration = XMVector3TransformNormal( m_torqueAccum, m_inverseInertiaTenserWorld );

	m_velocity += m_lastFrameAcceleration * duration;
	m_rotation += angularAcceleration * duration;

	m_velocity *= powf( m_linearDamping, duration );
	m_rotation *= powf( m_angularDamping, duration );

	m_position += m_velocity * duration;

	CXMFLOAT4 dq = m_rotation * duration;
	dq = XMQuaternionMultiply( m_orientation, dq ) * 0.5f;
	m_orientation += dq;

	CalculateDerivedData( );
	ClearAccumulators( );

	if ( m_canSleep )
	{
		float currentMotion = XMVectorGetX( XMVector3Dot( m_velocity, m_velocity ) ) +
			XMVectorGetX( XMVector3Dot( m_rotation, m_rotation ) );

		float bias = powf( 0.5f, duration );
		m_motion = bias * m_motion + ( 1.f - bias ) * currentMotion;

		if ( ( m_lastFrameMotion > m_motion ) && ( m_motion < g_sleepEpsilon ) )
		{
			SetAwake( false );
		}
		else if ( m_motion > g_sleepEpsilon * 10.f )
		{
			m_motion = 10.f * g_sleepEpsilon;
		}

		m_lastFrameMotion = m_motion;
	}
}

void RigidBody::CalculateDerivedData( )
{
	m_orientation = XMQuaternionNormalize( m_orientation );

	CXMFLOAT4X4 rotateMatrix = XMMatrixRotationQuaternion( m_orientation );
	m_transformMatrix = XMMatrixMultiply( rotateMatrix, XMMatrixTranslationFromVector( m_position ) );

	// M^-1 * I * M
	CXMFLOAT3X3 transformMat3x3 = m_transformMatrix;
	m_inverseInertiaTenserWorld = XMMatrixMultiply( XMMatrixTranspose( transformMat3x3 ), m_inverseInertiaTensor );
	m_inverseInertiaTenserWorld = XMMatrixMultiply( m_inverseInertiaTenserWorld, transformMat3x3 );
}
