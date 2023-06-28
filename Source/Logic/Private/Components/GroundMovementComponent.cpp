#include "Components/GroundMovementComponent.h"

#include "GameObject/GameObject.h"
#include "Json/Json.hpp"

void GroundMovementComponent::ThinkComponent( float elapsedTime )
{
	Super::ThinkComponent( elapsedTime );

	Vector forwardVector = GetOwner()->GetForwardVector();
	Vector rightVector = GetOwner()->GetRightVector();
	Vector upVector = GetOwner()->GetUpVector();

	Vector delta = GetDelta( elapsedTime );
	Vector deltaMove = ( delta.x * rightVector + delta.y * upVector + delta.z * forwardVector );

	Vector curLocation = GetOwner()->GetPosition();
	GetOwner()->SetPosition( curLocation + deltaMove );
}

void GroundMovementComponent::LoadProperty( const json::Value& json )
{
	Super::LoadProperty( json );

	if ( const json::Value* pMaxForce = json.Find( "Max_Force" ) )
	{
		SetMaxForceMagnitude( static_cast<float>( pMaxForce->AsReal() ) );
	}

	if ( const json::Value* pFriction = json.Find( "Friction" ) )
	{
		const json::Value& friction = *pFriction;

		if ( friction.Size() == 2 )
		{
			SetFriction( { static_cast<float>( friction[0].AsReal() ), static_cast<float>( friction[1].AsReal() ) } );
		}
	}

	if ( const json::Value* pForceScale = json.Find( "Kinetic_Force_Scale" ) )
	{
		m_kineticForceScale = static_cast<float>( pForceScale->AsReal() );
	}
}

void GroundMovementComponent::Update( const Vector& force, float elapsedTime )
{
	Vector scaledForce = force * m_kineticForceScale;

	float magnitude = scaledForce.Length();
	if ( magnitude > m_maxForceMag )
	{
		scaledForce *= m_maxForceMag / magnitude;
		magnitude = m_maxForceMag;
	}

	if ( m_isMoving == false )
	{
		if ( m_friction.first <= magnitude )
		{
			m_isMoving = true;
		}
		else
		{
			return;
		}
	}

	Vector moveDir = m_velocity.GetNormalized();
	if ( m_isMoving )
	{
		// calculate net force
		scaledForce -= ( moveDir * m_friction.second );
	}

	assert( m_mass != 0.f );
	Vector acceleration = scaledForce / m_mass;
	m_velocity += acceleration * elapsedTime;

	if ( ( moveDir | m_velocity ) < 0.f )
	{
		m_isMoving = false;
		m_velocity = Vector::ZeroVector;
	}
}

void GroundMovementComponent::SetFriction( const std::pair<float, float>& friction )
{
	m_friction = friction;
}

void GroundMovementComponent::SetMaxForceMagnitude( float magnitude )
{
	m_maxForceMag = magnitude;
}

GroundMovementComponent::GroundMovementComponent( CGameObject* pOwner, const char* name ) : Super( pOwner, name )
{
	m_think.m_thinkGroup = ThinkingGroup::PostPhysics;
	m_think.m_canEverTick = true;
}

bool GroundMovementComponent::ShouldCreateRenderState() const
{
    return false;
}

bool GroundMovementComponent::ShouldCreatePhysicsState() const
{
    return false;
}

Vector GroundMovementComponent::GetDelta( float elapsedTime ) const
{
	return m_velocity * elapsedTime;
}
