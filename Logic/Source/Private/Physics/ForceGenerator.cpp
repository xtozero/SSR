#include "stdafx.h"
#include "Physics/ForceGenerator.h"

#include "Math/Util.h"
#include "Physics/Body.h"

#include <algorithm>

using namespace DirectX;

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
	CXMFLOAT3 lws = body->GetPointInWorldSpace( m_connectionPoint );
	CXMFLOAT3 ows = m_other->GetPointInWorldSpace( m_otherConnectionPoint );

	CXMFLOAT3 force = lws - ows;

	float magnitude = XMVectorGetX( XMVector3Length( force ) );
	magnitude = fabsf( magnitude - m_restLength );
	magnitude *= m_springConstant;

	force = XMVector3Normalize( force );
	force *= -magnitude;
	body->AddForceAtPoint( force, lws );
}

void Aero::UpdateForce( RigidBody* body, float duration )
{
	UpdateForceFromTensor( body, duration, m_tensor );
}

void Aero::UpdateForceFromTensor( RigidBody* body, float /*duration*/, const CXMFLOAT3X3& tensor )
{
	CXMFLOAT3 velocity = body->GetVelocity( );
	velocity += m_windSpeed;

	const CXMFLOAT4X4& transform = body->GetTransform();

	CXMFLOAT3 bodyVel = XMVector3TransformNormal( velocity, XMMatrixInverse( nullptr, transform ) );

	CXMFLOAT3 force = XMVector3TransformNormal( bodyVel, tensor );
	force = XMVector3TransformNormal( force, transform );

	body->AddForceAtBodyPoint( force, m_position );
}

void AeroControl::UpdateForce( RigidBody* body, float duration )
{
	CXMFLOAT3X3 tensor = GetTensor( );
	UpdateForceFromTensor( body, duration, tensor );
}

CXMFLOAT3X3 AeroControl::GetTensor( )
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
	CXMFLOAT3 pointInWorld = body->GetPointInWorldSpace( m_centreOfBuoyancy );
	float depth = m_centreOfBuoyancy.y;

	if ( depth >= m_waterHeight + m_maxDepth )
	{
		return;
	}

	CXMFLOAT3 force( 0.f, 0.f, 0.f );

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
