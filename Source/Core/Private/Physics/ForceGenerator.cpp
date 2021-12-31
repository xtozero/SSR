#include "ForceGenerator.h"

#include "Body.h"
#include "Math/Util.h"

#include <algorithm>

void Gravity::UpdateForce( RigidBody* body, float /*duration*/ )
{
	if ( body->HasFiniteMass() == false )
	{
		return;
	}
	
	body->AddForce( m_gravity * body->GetMass( ) );
}

void Spring::UpdateForce( RigidBody* body, float /*duration*/ )
{
	Point lws = body->GetPointInWorldSpace( m_connectionPoint );
	Point ows = m_other->GetPointInWorldSpace( m_otherConnectionPoint );

	Vector force = lws - ows;

	float magnitude = force.Length();
	magnitude = fabsf( magnitude - m_restLength );
	magnitude *= m_springConstant;

	force = force.GetNormalized();
	force *= -magnitude;
	body->AddForceAtPoint( force, lws );
}

void Aero::UpdateForce( RigidBody* body, float duration )
{
	UpdateForceFromTensor( body, duration, m_tensor );
}

void Aero::UpdateForceFromTensor( RigidBody* body, float /*duration*/, const Matrix3X3& tensor )
{
	Vector velocity = body->GetVelocity( );
	velocity += m_windSpeed;

	const Matrix& transform = body->GetTransform();

	Vector bodyVel = transform.Inverse().TransformVector( velocity );

	Vector force = tensor.Transform( bodyVel );
	force = transform.TransformVector( force );

	body->AddForceAtBodyPoint( force, m_position );
}

void AeroControl::UpdateForce( RigidBody* body, float duration )
{
	Matrix3X3 tensor = GetTensor( );
	UpdateForceFromTensor( body, duration, tensor );
}

Matrix3X3 AeroControl::GetTensor( )
{
	if ( m_controlSetting <= -1.f )
	{
		return m_minTensor;
	}
	else if ( m_controlSetting >= 1.f )
	{
		return m_maxTensor;
	}
	else if ( m_controlSetting < 0 )
	{
		return lerp( m_minTensor, m_tensor, m_controlSetting + 1.f );
	}
	else if ( m_controlSetting > 0 )
	{
		return lerp( m_tensor, m_maxTensor, m_controlSetting );
	}

	return m_tensor;
}

void Buoyancy::UpdateForce( RigidBody* body, float /*duration*/ )
{
	Point pointInWorld = body->GetPointInWorldSpace( m_centreOfBuoyancy );
	float depth = m_centreOfBuoyancy.y;

	if ( depth >= m_waterHeight + m_maxDepth )
	{
		return;
	}

	Vector force;
	if ( depth <= m_waterHeight - m_maxDepth )
	{
		force.y = m_liquidDensity * m_volume;	
		body->AddForceAtBodyPoint( force, m_centreOfBuoyancy );
	}

	force.y = m_liquidDensity * m_volume * ( depth - m_maxDepth - m_waterHeight ) / 2 * m_maxDepth;
	body->AddForceAtBodyPoint( force, m_centreOfBuoyancy );
}

void ForceRegistry::Add( RigidBody* body, ForceGenerator* fg )
{
	m_registrations.emplace_back( body, fg );
}

void ForceRegistry::Remove( RigidBody* body, ForceGenerator* fg )
{
	auto pred = [body, fg]( const ForceRegistration& value )
				{
					return value.m_body == body && value.m_fg == fg;
				};

	m_registrations.erase( std::remove_if( m_registrations.begin( ), m_registrations.end( ), pred ), m_registrations.end( ) );
}

void ForceRegistry::Remove( RigidBody* body )
{
	auto pred = [body]( const ForceRegistration& value )
	{
		return value.m_body == body;
	};

	m_registrations.erase( std::remove_if( m_registrations.begin( ), m_registrations.end( ), pred ), m_registrations.end( ) );
}

void ForceRegistry::Clear( )
{
	m_registrations.clear( );
}

void ForceRegistry::UpdateForces( float duration )
{
	for ( auto i = m_registrations.begin( ); i != m_registrations.end( ); ++i )
	{
		i->m_fg->UpdateForce( i->m_body, duration );
	}
}
