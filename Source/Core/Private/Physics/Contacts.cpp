#include "Contacts.h"

#include "Body.h"
#include "Math/Util.h"

#include <utility>

using namespace DirectX;

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
	CXMFLOAT3 contactTangent[2];

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


	m_contactToWorld = CXMFLOAT3X3( m_contactNormal, contactTangent[0], contactTangent[1] );
}

CXMFLOAT3 Contact::CalculateLocalVelocity( uint32 bodyIndex, float duration )
{
	RigidBody* thisBody = m_body[bodyIndex];

	CXMFLOAT3 velocity = XMVector3Cross( thisBody->GetRotation( ), m_relativeContactPosition[bodyIndex] );
	velocity += thisBody->GetVelocity( );

	CXMFLOAT3 contactVelocity = XMVector3TransformNormal( velocity, XMMatrixTranspose( m_contactToWorld ) );

	CXMFLOAT3 accVelocity = thisBody->GetLastFrameAcceleration( ) * duration;

	accVelocity = XMVector3TransformNormal( accVelocity, XMMatrixTranspose( m_contactToWorld ) );

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
		velocityFromAcc += XMVectorGetX( XMVector3Dot( m_body[0]->GetLastFrameAcceleration( ) * duration, m_contactNormal ) );
	}

	if ( m_body[1] && m_body[1]->IsAwake( ) )
	{
		velocityFromAcc -= XMVectorGetX( XMVector3Dot( m_body[1]->GetLastFrameAcceleration( ) * duration, m_contactNormal ) );
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

CXMFLOAT3 Contact::CalculateFrictionlessImpulse( const CXMFLOAT3X3* inverseInertiaTensor )
{
	CXMFLOAT3 deltaVelWorld = XMVector3Cross( m_relativeContactPosition[0], m_contactNormal );
	deltaVelWorld = XMVector3TransformNormal( deltaVelWorld, inverseInertiaTensor[0] );
	deltaVelWorld = XMVector3Cross( deltaVelWorld, m_relativeContactPosition[0] );

	float deltaVelocity = XMVectorGetX( XMVector3Dot( deltaVelWorld, m_contactNormal ) );

	deltaVelocity += m_body[0]->GetInverseMass( );

	if ( m_body[1] )
	{
		deltaVelWorld = XMVector3Cross( m_relativeContactPosition[1], m_contactNormal );
		deltaVelWorld = XMVector3TransformNormal( deltaVelWorld, inverseInertiaTensor[1] );
		deltaVelWorld = XMVector3Cross( deltaVelWorld, m_relativeContactPosition[1] );

		deltaVelocity += XMVectorGetX( XMVector3Dot( deltaVelWorld, m_contactNormal ) );

		deltaVelocity += m_body[1]->GetInverseMass( );
	}

	return CXMFLOAT3( m_desiredDeltaVelocity / deltaVelocity, 0.f, 0.f );
}

CXMFLOAT3 Contact::CalculateFrictionImpulse( const CXMFLOAT3X3* inverseInertiaTensor )
{
	XMMATRIX negativeOne( g_XMNegIdentityR0, g_XMNegIdentityR1, g_XMNegIdentityR2, g_XMZero );
	float inverseMass = 0;

	inverseMass = m_body[0]->GetInverseMass( );

	CXMFLOAT3X3 impulseToTorque = MakeSkewSymmetric( m_relativeContactPosition[0] );

	CXMFLOAT3X3 deltaVelWorld = negativeOne;
	deltaVelWorld = XMMatrixMultiply( deltaVelWorld, impulseToTorque );
	deltaVelWorld = XMMatrixMultiply( deltaVelWorld, inverseInertiaTensor[0] );
	deltaVelWorld = XMMatrixMultiply( deltaVelWorld, impulseToTorque );

	if ( m_body[1] )
	{
		impulseToTorque = MakeSkewSymmetric( m_relativeContactPosition[1] );

		CXMFLOAT3X3 deltaVelWorld2 = negativeOne;
		deltaVelWorld2 = XMMatrixMultiply( deltaVelWorld2, impulseToTorque );
		deltaVelWorld2 = XMMatrixMultiply( deltaVelWorld2, inverseInertiaTensor[1] );
		deltaVelWorld2 = XMMatrixMultiply( deltaVelWorld2, impulseToTorque );

		deltaVelWorld += deltaVelWorld2;

		inverseMass += m_body[1]->GetInverseMass( );
	}

	CXMFLOAT3X3 deltaVelocity = XMMatrixMultiply( m_contactToWorld, deltaVelWorld );
	deltaVelocity = XMMatrixMultiply( deltaVelocity, XMMatrixTranspose( m_contactToWorld ) );

	deltaVelocity._11 += inverseMass;
	deltaVelocity._22 += inverseMass;
	deltaVelocity._33 += inverseMass;

	CXMFLOAT3X3 impulseMatrix = XMMatrixInverse( nullptr, deltaVelocity );

	CXMFLOAT3 velKill( m_desiredDeltaVelocity, -m_contactVelocity.y, -m_contactVelocity.z );

	CXMFLOAT3 impulseContact = XMVector3TransformNormal( velKill, impulseMatrix );

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

void Contact::ApplyPositionChange( CXMFLOAT3 linearChange[2], CXMFLOAT3 angularChange[2], float penetration )
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
			CXMFLOAT3X3 inverseInertiaTensor = m_body[i]->GetInverseInertiaTensorWorld( );

			CXMFLOAT3 angularInertiaWorld = XMVector3Cross( m_relativeContactPosition[i], m_contactNormal );
			angularInertiaWorld = XMVector3TransformNormal( angularInertiaWorld, inverseInertiaTensor );
			angularInertiaWorld = XMVector3Cross( angularInertiaWorld, m_relativeContactPosition[i] );
			angularInertia[i] = XMVectorGetX( XMVector3Dot( angularInertiaWorld, m_contactNormal ) );

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

			CXMFLOAT3 projection = m_contactNormal * -XMVectorGetX( XMVector3Dot( m_relativeContactPosition[i], m_contactNormal ) ) + m_relativeContactPosition[i];

			float maxMagnitude = angularLimit * XMVectorGetX( XMVector3Length( projection ) );

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
				angularChange[i] = CXMFLOAT3( 0.f, 0.f, 0.f );
			}
			else
			{
				CXMFLOAT3 targetAngularDirection = XMVector3Cross( m_relativeContactPosition[i], m_contactNormal );

				CXMFLOAT3X3 inverseInertiaTensor = m_body[i]->GetInverseInertiaTensorWorld( );

				angularChange[i] = XMVector3TransformNormal( targetAngularDirection, inverseInertiaTensor ) * ( angularMove[i] / angularInertia[i] );
			}

			linearChange[i] = m_contactNormal * linearMove[i];

			CXMFLOAT3 pos = ( m_contactNormal * linearMove[i] ) + m_body[i]->GetPosition( );
			m_body[i]->SetPosition( pos );

			// quaternion differentiation
			CXMFLOAT4 dq = angularChange[i];
			CXMFLOAT4 q = m_body[i]->GetOrientation( );
			dq = XMQuaternionMultiply( q, dq ) * 0.5f;
			q += dq;

			m_body[i]->SetOrientation( q );

			if ( m_body[i]->IsAwake( ) == false )
			{
				m_body[i]->CalculateDerivedData( );
			}
		}
	}
}

void Contact::ApplyVelocityChange( CXMFLOAT3 velocityChange[2], CXMFLOAT3 rotationChange[2] )
{
	CXMFLOAT3X3 inverseInertiaTensor[2];

	inverseInertiaTensor[0] = m_body[0]->GetInverseInertiaTensorWorld();
	if ( m_body[1] )
	{
		inverseInertiaTensor[1] = m_body[1]->GetInverseInertiaTensorWorld( );
	}

	CXMFLOAT3 impulseContact;

	if ( m_friction == 0.f )
	{
		impulseContact = CalculateFrictionlessImpulse( inverseInertiaTensor );
	}
	else
	{
		impulseContact = CalculateFrictionImpulse( inverseInertiaTensor );
	}

	CXMFLOAT3 impulse = XMVector3TransformCoord( impulseContact, m_contactToWorld );

	CXMFLOAT3 impulsiveTorque = XMVector3Cross( m_relativeContactPosition[0], impulse );
	rotationChange[0] = XMVector3TransformCoord( impulsiveTorque, inverseInertiaTensor[0] );
	velocityChange[0] = impulse * m_body[0]->GetInverseMass( );

	m_body[0]->AddVelocity( velocityChange[0] );
	m_body[0]->AddRotation( rotationChange[0] );

	if ( m_body[1] )
	{
		impulsiveTorque = XMVector3Cross( impulse, m_relativeContactPosition[1] );
		rotationChange[1] = XMVector3TransformCoord( impulsiveTorque, inverseInertiaTensor[1] );
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
	CXMFLOAT3 linearChange[2];
	CXMFLOAT3 angularChange[2];
	CXMFLOAT3 deltaPosition;

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
							deltaPosition = XMVector3Cross( angularChange[d], contactArray[i].m_relativeContactPosition[b] ) + linearChange[d];
							contactArray[i].m_penetration += XMVectorGetX( XMVector3Dot( contactArray[i].m_contactNormal, deltaPosition ) ) * ( b ? 1.f : -1.f );
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
	CXMFLOAT3 velocityChange[2];
	CXMFLOAT3 rotationChange[2];
	CXMFLOAT3 deltaVel;

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
							deltaVel = XMVector3Cross( rotationChange[d], contactArray[i].m_relativeContactPosition[b] ) + velocityChange[d];
							contactArray[i].m_contactVelocity += XMVector3TransformNormal( deltaVel, XMMatrixTranspose( contactArray[i].m_contactToWorld ) ) * ( b ? -1.f : 1.f );
							contactArray[i].CalculateDesiredDeltaVelocity( duration );
						}
					}
				}
			}
		}

		++m_velocityIterationsUsed;
	}
}
