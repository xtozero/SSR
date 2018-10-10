#include "stdafx.h"
#include "Physics/Aaboundingbox.h"
#include "Physics/BoundingSphere.h"
#include "Physics/CollideNarrow.h"

using namespace DirectX;

unsigned int SphereAndSphere( const BoundingSphere& lhs, RigidBody* lhsBody, const BoundingSphere& rhs, RigidBody* rhsBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return 0;
	}

	const CXMFLOAT3& lhsPos = lhs.GetCenter( );
	const CXMFLOAT3& rhsPos = rhs.GetCenter( );

	CXMFLOAT3 midline = lhsPos - rhsPos;
	float size = XMVectorGetX( XMVector3Length( midline ) );

	if ( size <= 0.f || size >= lhs.GetRadius( ) + rhs.GetRadius( ) )
	{
		return 0;
	}

	CXMFLOAT3 normal = midline / size;

	Contact* contact = data->m_contacts;
	contact->SetContactNormal( normal );
	contact->SetContactPoint( lhsPos + CXMFLOAT3( midline * 0.5f ) );
	contact->SetPenetration( lhs.GetRadius( ) + rhs.GetRadius( ) - size );
	contact->SetBodyData( lhsBody, rhsBody, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return 1;
}

unsigned int SphereAndHalfSpace( const BoundingSphere& sphere, RigidBody* sphereBody, const CXMFLOAT4& plane, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return 0;
	}

	const CXMFLOAT3& pos = sphere.GetCenter( );

	float ballDistance = XMVectorGetX( XMPlaneDotCoord( plane, pos ) ) - sphere.GetRadius();

	if ( ballDistance >= 0 )
	{
		return 0;
	}

	Contact* contact = data->m_contacts;
	CXMFLOAT3 planeDir( plane.x, plane.y, plane.z );
	contact->SetContactNormal( planeDir );
	contact->SetContactPoint( pos - CXMFLOAT3( planeDir * ( ballDistance + sphere.GetRadius( ) ) ) );
	contact->SetPenetration( -ballDistance );
	contact->SetBodyData( sphereBody, nullptr, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return 1;
}

unsigned int SphereAndTruePlane( const BoundingSphere & sphere, RigidBody * sphereBody, const CXMFLOAT4 & plane, CollisionData * data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return 0;
	}

	const CXMFLOAT3& pos = sphere.GetCenter( );

	float centerDistance = XMVectorGetX( XMPlaneDotCoord( plane, pos ) );

	if ( centerDistance > sphere.GetRadius() )
	{
		return 0;
	}

	CXMFLOAT3 normal( plane.x, plane.y, plane.z );
	float penetration = -centerDistance;
	if ( centerDistance < 0 )
	{
		normal *= -1;
		penetration = -penetration;
	}
	penetration += sphere.GetRadius( );

	Contact* contact = data->m_contacts;
	contact->SetContactNormal( normal );
	contact->SetContactPoint( pos - CXMFLOAT3( normal * centerDistance ) );
	contact->SetPenetration( penetration );
	contact->SetBodyData( sphereBody, nullptr, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return 1;
}

unsigned int BoxAndHalfSpace( const CAaboundingbox& box, RigidBody* boxBody, const CXMFLOAT4& plane, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return 0;
	}

	Contact* contact = data->m_contacts;
	int contactsUsed = 0;

	for ( int i = 0; i < 8; ++i )
	{
		CXMFLOAT3 vertexPos = box.Point( i );

		float vertexDistance = XMVectorGetX( XMPlaneDotNormal( plane, vertexPos ) );
		if ( vertexDistance + plane.w <= 0.f )
		{
			CXMFLOAT3 normal( plane.x, plane.y, plane.z );
			contact->SetContactNormal( normal );
			contact->SetContactPoint( vertexPos + CXMFLOAT3( normal * ( vertexDistance + plane.w ) ) );
			contact->SetPenetration( vertexDistance + plane.w );
			contact->SetBodyData( boxBody, nullptr, data->m_friction, data->m_restitution );

			++contact;
			++contactsUsed;
			if ( contactsUsed == data->m_contactsLeft )
			{
				return contactsUsed;
			}
		}
	}

	data->AddContacts( contactsUsed );
	return contactsUsed;
}

unsigned int BoxAndSphere( const CAaboundingbox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return 0;
	}

	BoundingSphere boxSphere( box );

	if ( boxSphere.Intersect( sphere ) == 0 )
	{
		return 0;
	}

	const CXMFLOAT3& center = sphere.GetCenter( );
	CXMFLOAT3 closestPoint = center;

	const CXMFLOAT3& boxMax = box.GetMax( );
	const CXMFLOAT3& boxMin = box.GetMin( );
	if ( closestPoint.x > boxMax.x )
	{
		closestPoint.x = boxMax.x;
	}
	else if ( closestPoint.x < boxMin.x )
	{
		closestPoint.x = boxMin.x;
	}

	if ( closestPoint.y > boxMax.y )
	{
		closestPoint.y = boxMax.y;
	}
	else if ( closestPoint.y < boxMin.y )
	{
		closestPoint.y = boxMin.y;
	}

	if ( closestPoint.z > boxMax.z )
	{
		closestPoint.z = boxMax.z;
	}
	else if ( closestPoint.z < boxMin.z )
	{
		closestPoint.z = boxMin.z;
	}

	float dist = XMVectorGetX( XMVector3LengthSq( closestPoint - center ) );
	if ( dist > sphere.GetRadius( ) * sphere.GetRadius( ) )
	{
		return 0;
	}

	Contact* contact = data->m_contacts;
	contact->SetContactNormal( XMVector3Normalize( closestPoint - center ) );
	contact->SetContactPoint( closestPoint );
	contact->SetPenetration( sphere.GetRadius( ) - sqrtf( dist ) );
	contact->SetBodyData( boxBody, sphereBody, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return 1;
}

unsigned int BoxAndBox( const CAaboundingbox& lhs, RigidBody* lhsBody, const CAaboundingbox& rhs, RigidBody* rhsBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return 0;
	}

	const CXMFLOAT3& lhsMin = lhs.GetMin();
	const CXMFLOAT3& lhsMax = lhs.GetMax();
	const CXMFLOAT3& rhsMin = rhs.GetMin();
	const CXMFLOAT3& rhsMax = rhs.GetMax();

	CXMFLOAT3 totalSize;
	lhs.Size( totalSize );

	CXMFLOAT3 rhsSize;
	rhs.Size( rhsSize );
	totalSize += rhsSize;

	float bestOverlap = FLT_MAX;
	int bestAxis = 0;

	for ( int i = 0; i < 3; ++i )
	{
		float overlap = totalSize[i] - ( ( lhsMax[i] > rhsMax[i] ) ? lhsMax[i] : rhsMax[i] - ( lhsMin[i] < rhsMin[i] ) ? lhsMin[i] : rhsMin[i] );
		
		if ( overlap < bestOverlap )
		{
			bestOverlap = overlap;
			bestAxis = i;
		}
	}

	CXMFLOAT3 axis[3] = {
		{ 1.f, 0.f, 0.f },
		{ 0.f, 1.f, 0.f },
		{ 0.f, 0.f, 1.f}
	};

	CXMFLOAT3 normal = axis[bestAxis];
	CXMFLOAT3 lhsCentre;
	lhs.Centroid( lhsCentre );
	CXMFLOAT3 rhsCentre;
	rhs.Centroid( rhsCentre );

	CXMFLOAT3 toCentre = XMVector3Normalize( rhsCentre - lhsCentre );
	if ( XMVectorGetX( XMVector3Dot( normal, toCentre ) ) > 0 )
	{
		normal *= -1.f;
	}

	CXMFLOAT3 vertex = rhsMax;
	if ( XMVectorGetX( XMVector3Dot( axis[0], toCentre ) ) >= 0 )
	{
		vertex.x = rhsMin.x;
	}

	if ( XMVectorGetX( XMVector3Dot( axis[1], toCentre ) ) >= 0 )
	{
		vertex.y = rhsMin.y;
	}

	if ( XMVectorGetX( XMVector3Dot( axis[2], toCentre ) ) >= 0 )
	{
		vertex.z = rhsMin.z;
	}

	Contact* contact = data->m_contacts;
	contact->SetContactNormal( normal );
	contact->SetContactPoint( vertex );
	contact->SetPenetration( bestOverlap );
	contact->SetBodyData( lhsBody, rhsBody, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return 1;
}
