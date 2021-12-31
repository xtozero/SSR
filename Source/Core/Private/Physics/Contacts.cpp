#include "Contacts.h"

#include "Body.h"
#include "Math/Util.h"

#include <utility>

void Contact::SetBodyData( RigidBody* one, RigidBody* two, float friction, float restitution )
{
	m_body[0] = one->HasFiniteMass() ? one : nullptr;
	m_body[1] = two->HasFiniteMass() ? two : nullptr;
	m_friction = friction;
	m_restitution = restitution;
	assert( m_body[0] != nullptr || m_body[1] != nullptr );
}

void Contact::CalculateContactBasis( )
{
	Vector contactTangent[2];

	if ( fabsf( m_contactNormal.x ) > fabsf( m_contactNormal.y ) )
	{
		float s = 1.f / sqrtf( m_contactNormal.z * m_contactNormal.z + m_contactNormal.x * m_contactNormal.x );

		contactTangent[0].x = m_contactNormal.z * s;
		contactTangent[0].y = 0.f;
		contactTangent[0].z = -m_contactNormal.x * s;

		contactTangent[1].x = m_contactNormal.y * contactTangent[0].z;
		contactTangent[1].y = m_contactNormal.z * contactTangent[0].x - m_contactNormal.x * contactTangent[0].z;
		contactTangent[1].z = -m_contactNormal.y * contactTangent[0].x;
	}
	else
	{
		float s = 1.f / sqrtf( m_contactNormal.y * m_contactNormal.y + m_contactNormal.z * m_contactNormal.z );

		contactTangent[0].x = 0.f;
		contactTangent[0].y = -m_contactNormal.z * s;
		contactTangent[0].z = m_contactNormal.y * s;

		contactTangent[1].x = m_contactNormal.y * contactTangent[0].z - m_contactNormal.z * contactTangent[0].y;
		contactTangent[1].y = -m_contactNormal.x * contactTangent[0].z;
		contactTangent[1].z = m_contactNormal.x * contactTangent[0].y;
	}


	m_contactToWorld = Matrix3X3( m_contactNormal, contactTangent[0], contactTangent[1] );
}

Vector Contact::CalculateLocalVelocity( uint32 bodyIndex, float duration )
{
	RigidBody* thisBody = m_body[bodyIndex];

	Vector velocity = thisBody->GetRotation() ^ m_relativeContactPosition[bodyIndex];
	velocity += thisBody->GetVelocity( );

	Vector contactVelocity = m_contactToWorld.GetTrasposed().TransformVector( velocity );

	Vector accVelocity = thisBody->GetLastFrameAcceleration( ) * duration;

	accVelocity = m_contactToWorld.GetTrasposed().TransformVector( accVelocity );

	accVelocity.x = 0;

	contactVelocity += accVelocity;

	return contactVelocity;
}

void Contact::CalculateDesiredDeltaVelocity( float duration )
{
	constexpr float velocityLimit = 0.25f;

	float velocityFromAcc = 0;

	if ( m_body[0]->IsAwake( ) )
	{
		velocityFromAcc += ( m_body[0]->GetLastFrameAcceleration( ) * duration ) | m_contactNormal;
	}

	if ( m_body[1] && m_body[1]->IsAwake( ) )
	{
		velocityFromAcc -= ( m_body[1]->GetLastFrameAcceleration( ) * duration ) | m_contactNormal;
	}

	float thisRestitution = m_restitution;
	if ( fabsf( m_contactVelocity.x ) < velocityLimit )
	{
		thisRestitution = 0.f;
	}

	m_desiredDeltaVelocity = -m_contactVelocity.x - thisRestitution * ( m_contactVelocity.x - velocityFromAcc );
}

void Contact::CalculateInternals( float duration )
{
	if ( m_body[0] == nullptr )
	{
		SwapBodies( );
	}
	assert( m_body[0] != nullptr );

	CalculateContactBasis( );

	m_relativeContactPosition[0] = m_contactPoint - m_body[0]->GetPosition( );
	if ( m_body[1] )
	{
		m_relativeContactPosition[1] = m_contactPoint - m_body[1]->GetPosition( );
	}

	m_contactVelocity = CalculateLocalVelocity( 0, duration );
	if ( m_body[1] )
	{
		m_contactVelocity -= CalculateLocalVelocity( 1, duration );
	}

	CalculateDesiredDeltaVelocity( duration );
}

Vector Contact::CalculateFrictionlessImpulse( const Matrix3X3* inverseInertiaTensor )
{
	Vector deltaVelWorld = m_relativeContactPosition[0] ^ m_contactNormal;
	deltaVelWorld = inverseInertiaTensor[0].Transform( deltaVelWorld );
	deltaVelWorld = deltaVelWorld ^ m_relativeContactPosition[0];

	float deltaVelocity = deltaVelWorld | m_contactNormal;

	deltaVelocity += m_body[0]->GetInverseMass( );

	if ( m_body[1] )
	{
		deltaVelWorld = m_relativeContactPosition[1] ^ m_contactNormal;
		deltaVelWorld = inverseInertiaTensor[1].Transform( deltaVelWorld );
		deltaVelWorld = deltaVelWorld ^ m_relativeContactPosition[1];

		deltaVelocity += deltaVelWorld | m_contactNormal;

		deltaVelocity += m_body[1]->GetInverseMass( );
	}

	return Vector( m_desiredDeltaVelocity / deltaVelocity, 0.f, 0.f );
}

Vector Contact::CalculateFrictionImpulse( const Matrix3X3* inverseInertiaTensor )
{
	Matrix3X3 negativeOne( -Vector::RightVector, -Vector::UpVector, -Vector::ForwardVector );
	float inverseMass = 0;

	inverseMass = m_body[0]->GetInverseMass( );

	Matrix3X3 impulseToTorque = MakeSkewSymmetric( m_relativeContactPosition[0] );

	Matrix3X3 deltaVelWorld = negativeOne;
	deltaVelWorld *= impulseToTorque;
	deltaVelWorld *= inverseInertiaTensor[0];
	deltaVelWorld *= impulseToTorque;

	if ( m_body[1] )
	{
		impulseToTorque = MakeSkewSymmetric( m_relativeContactPosition[1] );

		Matrix3X3 deltaVelWorld2 = negativeOne;
		deltaVelWorld2 *= impulseToTorque;
		deltaVelWorld2 *= inverseInertiaTensor[1];
		deltaVelWorld2 *= impulseToTorque;

		deltaVelWorld += deltaVelWorld2;

		inverseMass += m_body[1]->GetInverseMass( );
	}

	Matrix3X3 deltaVelocity = m_contactToWorld * deltaVelWorld;
	deltaVelocity *= m_contactToWorld.GetTrasposed();

	deltaVelocity._11 += inverseMass;
	deltaVelocity._22 += inverseMass;
	deltaVelocity._33 += inverseMass;

	Matrix3X3 impulseMatrix = deltaVelocity.Inverse();

	Vector velKill( m_desiredDeltaVelocity, -m_contactVelocity.y, -m_contactVelocity.z );

	Vector impulseContact = impulseMatrix.Transform( velKill );

	float planarImpulse = sqrtf( impulseContact.y * impulseContact.y + impulseContact.z * impulseContact.z );

	if ( planarImpulse > impulseContact.x * m_friction )
	{
		impulseContact.y /= planarImpulse;
		impulseContact.z /= planarImpulse;
		impulseContact.x = deltaVelocity._11 + deltaVelocity._21 * m_friction * impulseContact.y + deltaVelocity._31 * m_friction * impulseContact.z;
		impulseContact.x = m_desiredDeltaVelocity / impulseContact.x;
		impulseContact.y *= m_friction * impulseContact.x;
		impulseContact.z *= m_friction * impulseContact.x;
	}

	return impulseContact;
}

void Contact::ApplyPositionChange( Vector linearChange[2], Vector angularChange[2], float penetration )
{
	constexpr float angularLimit = 0.2f;
	float angularMove[2];
	float linearMove[2];

	float totalInertia = 0;
	float linearInertia[2];
	float angularInertia[2];

	for ( uint32 i = 0; i < 2; ++i )
	{
		if ( m_body[i] )
		{
			Matrix3X3 inverseInertiaTensor = m_body[i]->GetInverseInertiaTensorWorld( );

			Vector angularInertiaWorld = m_relativeContactPosition[i] ^ m_contactNormal;
			angularInertiaWorld = inverseInertiaTensor.Transform( angularInertiaWorld );
			angularInertiaWorld = angularInertiaWorld ^ m_relativeContactPosition[i];
			angularInertia[i] = angularInertiaWorld | m_contactNormal;

			linearInertia[i] = m_body[i]->GetInverseMass( );

			totalInertia += linearInertia[i] + angularInertia[i];
		}
	}

	for ( uint32 i = 0; i < 2; ++i )
	{
		if ( m_body[i] )
		{
			float sign = ( i == 0 ) ? 1.f : -1.f;

			angularMove[i] = sign * penetration * ( angularInertia[i] / totalInertia );
			linearMove[i] = sign * penetration * ( linearInertia[i] / totalInertia );

			Vector projection = m_contactNormal * -( m_relativeContactPosition[i] | m_contactNormal ) + m_relativeContactPosition[i];

			float maxMagnitude = angularLimit * projection.Length();

			float totalMove = angularMove[i] + linearMove[i];
			if ( angularMove[i] < -maxMagnitude )
			{
				angularMove[i] = -maxMagnitude;
				linearMove[i] = totalMove - angularMove[i];
			}
			else if ( angularMove[i] > maxMagnitude )
			{
				angularMove[i] = maxMagnitude;
				linearMove[i] = totalMove - angularMove[i];
			}

			if ( angularMove[i] == 0 )
			{
				angularChange[i] = Vector::ZeroVector;
			}
			else
			{
				Vector targetAngularDirection = m_relativeContactPosition[i] ^ m_contactNormal;

				Matrix3X3 inverseInertiaTensor = m_body[i]->GetInverseInertiaTensorWorld( );

				angularChange[i] = inverseInertiaTensor.Transform( targetAngularDirection ) * ( angularMove[i] / angularInertia[i] );
			}

			linearChange[i] = m_contactNormal * linearMove[i];

			Point pos = ( m_contactNormal * linearMove[i] ) + m_body[i]->GetPosition( );
			m_body[i]->SetPosition( pos );

			// quaternion differentiation
			auto dq = Quaternion( angularChange[i] );
			Quaternion q = m_body[i]->GetOrientation( );
			dq = ( dq * q ) * 0.5f;
			q += dq;

			m_body[i]->SetOrientation( q );

			if ( m_body[i]->IsAwake( ) == false )
			{
				m_body[i]->CalculateDerivedData( );
			}
		}
	}
}

void Contact::ApplyVelocityChange( Vector velocityChange[2], Vector rotationChange[2] )
{
	Matrix3X3 inverseInertiaTensor[2];

	inverseInertiaTensor[0] = m_body[0]->GetInverseInertiaTensorWorld();
	if ( m_body[1] )
	{
		inverseInertiaTensor[1] = m_body[1]->GetInverseInertiaTensorWorld( );
	}

	Vector impulseContact;

	if ( m_friction == 0.f )
	{
		impulseContact = CalculateFrictionlessImpulse( inverseInertiaTensor );
	}
	else
	{
		impulseContact = CalculateFrictionImpulse( inverseInertiaTensor );
	}

	Vector impulse = m_contactToWorld.Transform( impulseContact );

	Vector impulsiveTorque = m_relativeContactPosition[0] ^ impulse;
	rotationChange[0] = inverseInertiaTensor[0].Transform( impulsiveTorque );
	velocityChange[0] = impulse * m_body[0]->GetInverseMass( );

	m_body[0]->AddVelocity( velocityChange[0] );
	m_body[0]->AddRotation( rotationChange[0] );

	if ( m_body[1] )
	{
		impulsiveTorque = impulse ^ m_relativeContactPosition[1];
		rotationChange[1] = inverseInertiaTensor[1].Transform( impulsiveTorque );
		velocityChange[1] = impulse * -m_body[1]->GetInverseMass( );

		m_body[1]->AddVelocity( velocityChange[1] );
		m_body[1]->AddRotation( rotationChange[1] );
	}
}

void Contact::SwapBodies( )
{
	std::swap( m_body[0], m_body[1] );
	m_contactNormal *= -1.f;
}

void Contact::MatchAwakeState( )
{
	if ( m_body[1] == nullptr )
	{
		return;
	}

	bool bodyAwake[2] = { m_body[0]->IsAwake(), m_body[1]->IsAwake() };

	if ( bodyAwake[0] ^ bodyAwake[1] )
	{
		if ( bodyAwake[0] )
		{
			m_body[1]->SetAwake( );
		}
		else
		{
			m_body[0]->SetAwake( );
		}
	}
}

void ContactResolver::Initialize( uint32 iterations, float velocityEpsilon, float positionEpsilon )
{
	m_positionIterations = iterations;
	m_velocityIterations = iterations;
	m_positionEpsilon = positionEpsilon;
	m_velocityEpsilon = velocityEpsilon;
}

void ContactResolver::ResolveContacts( Contact* contactArray, uint32 numContacts, float duration )
{
	if ( numContacts == 0 )
	{
		return;
	}

	PrepareContacts( contactArray, numContacts, duration );

	AdjustPositions( contactArray, numContacts, duration );

	AdjustVelocities( contactArray, numContacts, duration );
}

void ContactResolver::PrepareContacts( Contact* contactArray, uint32 numContacts, float duration )
{
	Contact* lastContact = contactArray + numContacts;
	for ( Contact* contact = contactArray; contact < lastContact; ++contact )
	{
		contact->CalculateInternals( duration );
	}
}

void ContactResolver::AdjustPositions( Contact* contactArray, uint32 numContacts, float /*duration*/ )
{
	Vector linearChange[2];
	Vector angularChange[2];
	Vector deltaPosition;

	m_positionIterationsUsed = 0;
	while ( m_positionIterationsUsed < m_positionIterations )
	{
		float max = m_positionEpsilon;
		uint32 index = numContacts;

		for ( uint32 i = 0; i < numContacts; ++i )
		{
			const Contact& c = contactArray[i];

			if ( c.m_penetration > max )
			{
				max = c.m_penetration;
				index = i;
			}
		}

		if ( index == numContacts )
		{
			break;
		}

		contactArray[index].MatchAwakeState( );

		contactArray[index].ApplyPositionChange( linearChange, angularChange, max );

		for ( uint32 i = 0; i < numContacts; ++i )
		{
			for ( uint32 b = 0; b < 2; ++b )
			{
				if ( contactArray[i].m_body[b] )
				{
					for ( uint32 d = 0; d < 2; ++d )
					{
						if ( contactArray[i].m_body[b] == contactArray[index].m_body[d] )
						{
							deltaPosition = ( angularChange[d] ^ contactArray[i].m_relativeContactPosition[b] ) + linearChange[d];
							contactArray[i].m_penetration += ( contactArray[i].m_contactNormal | deltaPosition ) * ( b ? 1.f : -1.f );
						}
					}
				}
			}
		}

		++m_positionIterationsUsed;
	}
}

void ContactResolver::AdjustVelocities( Contact* contactArray, uint32 numContacts, float duration )
{
	Vector velocityChange[2];
	Vector rotationChange[2];
	Vector deltaVel;

	m_velocityIterationsUsed = 0;
	while ( m_velocityIterationsUsed < m_velocityIterations )
	{
		float max = m_velocityEpsilon;
		uint32 index = numContacts;

		for ( uint32 i = 0; i < numContacts; ++i )
		{
			Contact& c = contactArray[i];

			if ( c.m_desiredDeltaVelocity > max )
			{
				max = c.m_desiredDeltaVelocity;
				index = i;
			}
		}

		if ( index == numContacts )
		{
			break;
		}

		contactArray[index].MatchAwakeState( );

		contactArray[index].ApplyVelocityChange( velocityChange, rotationChange );

		for ( uint32 i = 0; i < numContacts; ++i )
		{
			for ( uint32 b = 0; b < 2; ++b )
			{
				if ( contactArray[i].m_body[b] )
				{
					for ( uint32 d = 0; d < 2; ++d )
					{
						if ( contactArray[i].m_body[b] == contactArray[index].m_body[d] )
						{
							deltaVel = ( rotationChange[d] ^ contactArray[i].m_relativeContactPosition[b] ) + velocityChange[d];
							contactArray[i].m_contactVelocity += contactArray[i].m_contactToWorld.GetTrasposed().TransformVector( deltaVel ) * ( b ? -1.f : 1.f );
							contactArray[i].CalculateDesiredDeltaVelocity( duration );
						}
					}
				}
			}
		}

		++m_velocityIterationsUsed;
	}
}
