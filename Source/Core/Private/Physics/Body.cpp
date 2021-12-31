#include "Body.h"

#include "TransformationMatrix.h"

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

void RigidBody::SetPosition( const Point& position )
{
	m_position = position;
}

Point RigidBody::GetPosition( ) const
{
	return m_position;
}

void RigidBody::SetOrientation( const Quaternion& orientation )
{
	m_orientation = orientation.GetNormalized();
}

void RigidBody::SetOrientation( float pitch, float yaw, float roll )
{
	SetOrientation( Quaternion( pitch, yaw, roll ) );
}

Quaternion RigidBody::GetOrientation( ) const
{
	return m_orientation;
}

void RigidBody::SetVelocity( const Vector& velocity )
{
	m_velocity = velocity;
}

Vector RigidBody::GetVelocity( ) const
{
	return m_velocity;
}

void RigidBody::AddVelocity( const Vector& deltaVelocity )
{
	m_velocity += deltaVelocity;
}

void RigidBody::SetRotation( const Vector& rotation )
{
	m_rotation = rotation;
}

Vector RigidBody::GetRotation( ) const
{
	return m_rotation;
}

void RigidBody::AddRotation( const Vector& deltaRotation )
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
		m_velocity = Vector::ZeroVector;
		m_rotation = Vector::ZeroVector;
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

Matrix RigidBody::GetTransform( ) const
{
	return m_transformMatrix;
}

void RigidBody::SetInertiaTensor( const Matrix3X3& inertiaTensor )
{
	m_inverseInertiaTensor = inertiaTensor.Inverse();

	if ( m_inverseInertiaTensor.IsNaN() )
	{
		m_inverseInertiaTensor = Matrix3X3( 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f );
	}
}

Matrix3X3 RigidBody::GetInertiaTensor( ) const
{
	return m_inverseInertiaTensor.Inverse();
}

void RigidBody::SetInverseInertiaTensor( const Matrix3X3& inverseInertiaTensor )
{
	m_inverseInertiaTensor = inverseInertiaTensor;
}

Matrix3X3 RigidBody::GetInverseInertiaTensor( ) const
{
	return m_inverseInertiaTensor;
}

Matrix3X3 RigidBody::GetInertiaTensorWorld( ) const
{
	return m_inverseInertiaTenserWorld.Inverse();
}

Matrix3X3 RigidBody::GetInverseInertiaTensorWorld( ) const
{
	return m_inverseInertiaTenserWorld;
}

Point RigidBody::GetPointInWorldSpace( const Point& point )
{
	return m_transformMatrix.TransformPosition( point );
}

void RigidBody::AddForce( const Vector& force )
{
	m_forceAccum += force;
	m_isAwake = true;
}

void RigidBody::AddForceAtPoint( const Vector& force, const Point& point )
{
	Point pt = GetPointInWorldSpace( point );
	AddForceAtBodyPoint( force, pt );
}

void RigidBody::AddForceAtBodyPoint( const Vector& force, const Point& point )
{
	Point pt = point;
	pt -= m_position;
	m_forceAccum += force;
	m_torqueAccum += pt ^ force;
	m_isAwake = true;
}

void RigidBody::AddTouque( const Vector& touque )
{
	m_torqueAccum += touque;
	m_isAwake = true;
}

void RigidBody::SetAcceleration( const Vector& acceleration )
{
	m_acceleration = acceleration;
}

Vector RigidBody::GetAcceleration( )
{
	return m_acceleration;
}

Vector RigidBody::GetLastFrameAcceleration( )
{
	return m_lastFrameAcceleration;
}

void RigidBody::ClearAccumulators( )
{
	m_forceAccum = Vector::ZeroVector;
	m_torqueAccum = Vector::ZeroVector;
}

bool RigidBody::Integrate( float duration )
{
	if ( m_isAwake == false )
	{
		return false;
	}

	if ( HasFiniteMass( ) == false ) 
	{
		SetAwake( false );
		return false;
	}

	m_lastFrameAcceleration = m_acceleration;
	m_lastFrameAcceleration += m_forceAccum * m_inverseMass;

	Vector angularAcceleration = m_inverseInertiaTenserWorld.Transform( m_torqueAccum );

	m_velocity += m_lastFrameAcceleration * duration;
	m_rotation += angularAcceleration * duration;

	m_velocity *= powf( m_linearDamping, duration );
	m_rotation *= powf( m_angularDamping, duration );

	Point prevPos = m_position;
	m_position += m_velocity * duration;

	Quaternion prevOrient = m_orientation;
	Quaternion dq = m_rotation * duration;
	dq = ( dq * m_orientation ) * 0.5f;
	m_orientation += dq;

	CalculateDerivedData( );
	ClearAccumulators( );

	if ( m_canSleep )
	{
		float currentMotion = ( m_velocity | m_velocity ) + ( m_rotation | m_rotation );

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

	return ( prevPos != m_position ) || ( prevOrient != m_orientation );
}

void RigidBody::CalculateDerivedData( )
{
	m_orientation = m_orientation.GetNormalized();

	Matrix rotateMatrix = RotateMatrix( m_orientation );
	m_transformMatrix = rotateMatrix * TranslationMatrix( m_position );

	// M^-1 * I * M
	auto transformMat3x3 = Matrix3X3( m_transformMatrix );
	m_inverseInertiaTenserWorld = transformMat3x3.GetTrasposed() * m_inverseInertiaTensor;
	m_inverseInertiaTenserWorld *= transformMat3x3;
}
