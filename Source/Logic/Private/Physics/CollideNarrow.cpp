#include "stdafx.h"
#include "Physics/CollideNarrow.h"

#include "Physics/Aaboundingbox.h"
#include "Physics/BoundingSphere.h"
#include "Physics/OrientedBoundingBox.h"

#include <algorithm>

using namespace DirectX;

namespace
{
	void FillPointFaceBoxBox( const COrientedBoundingBox& lhs, RigidBody* lhsBody, const COrientedBoundingBox& rhs, RigidBody* rhsBody, const CXMFLOAT3& toCentre, CollisionData* data, uint32 best, float pen )
	{
		Contact* contact = data->m_contacts;

		CXMFLOAT3 normal = lhs.GetAxisVector( best );

		if ( XMVectorGetX( XMVector3Dot( normal, toCentre ) ) > 0 )
		{
			normal = -normal;
		}

		CXMFLOAT3 vertex = rhs.GetHalfSize( );
		if ( XMVectorGetX( XMVector3Dot( rhs.GetAxisVector( 0 ), normal ) ) < 0 )
		{
			vertex.x = -vertex.x;
		}
		if ( XMVectorGetX( XMVector3Dot( rhs.GetAxisVector( 1 ), normal ) ) < 0 )
		{
			vertex.y = -vertex.y;
		}
		if ( XMVectorGetX( XMVector3Dot( rhs.GetAxisVector( 2 ), normal ) ) < 0 )
		{
			vertex.z = -vertex.z;
		}

		contact->SetContactNormal( normal );
		contact->SetPenetration( pen );
		contact->SetContactPoint( XMVector3TransformCoord( vertex, rhs.GetTransform( ) ) );
		contact->SetBodyData( lhsBody, rhsBody, data->m_friction, data->m_restitution );
	}

	CXMFLOAT3 CalcEdgeContactPoint( const CXMFLOAT3& pLhs, const CXMFLOAT3& dLhs, float lhsSize, const CXMFLOAT3& pRhs, const CXMFLOAT3& dRhs, float rhsSize, bool useLhs )
	{
		float smLhs = XMVectorGetX( XMVector3LengthSq( dLhs ) );
		float smRhs = XMVectorGetX( XMVector3LengthSq( dRhs ) );
		float dpLhsRhs = XMVectorGetX( XMVector3Dot( dLhs, dRhs ) );

		CXMFLOAT3 toSt = pLhs - pRhs;
		float dpStaLhs = XMVectorGetX( XMVector3Dot( toSt, dLhs ) );
		float dpStaRhs = XMVectorGetX( XMVector3Dot( toSt, dRhs ) );

		float denom = smLhs * smRhs - dpLhsRhs * dpLhsRhs;

		if ( fabsf( denom ) < 0.0001f )
		{
			return useLhs ? pLhs : pRhs;
		}

		float  mua = ( dpLhsRhs * dpStaRhs - smRhs * dpStaLhs ) / denom;
		float  mub = ( smLhs * dpStaRhs - dpLhsRhs * dpStaLhs ) / denom;

		if ( mua > lhsSize ||
			mua < -lhsSize ||
			mub > rhsSize ||
			mub < -rhsSize )
		{
			return useLhs ? pLhs : pRhs;
		}
		else
		{
			CXMFLOAT3 cLhs = pLhs + ( dLhs * mua );
			CXMFLOAT3 cRhs = pRhs + ( dRhs * mub );

			return ( cLhs + cRhs ) * 0.5;
		}
	}
}

uint32 SphereAndSphere( const BoundingSphere& lhs, RigidBody* lhsBody, const BoundingSphere& rhs, RigidBody* rhsBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	const CXMFLOAT3& lhsPos = lhs.GetCenter( );
	const CXMFLOAT3& rhsPos = rhs.GetCenter( );

	CXMFLOAT3 midline = lhsPos - rhsPos;
	float size = XMVectorGetX( XMVector3Length( midline ) );

	if ( size <= 0.f || size >= lhs.GetRadius( ) + rhs.GetRadius( ) )
	{
		return COLLISION::OUTSIDE;
	}

	CXMFLOAT3 normal = midline / size;

	Contact* contact = data->m_contacts;
	contact->SetContactNormal( normal );
	contact->SetContactPoint( lhsPos - CXMFLOAT3( midline * 0.5f ) );
	contact->SetPenetration( lhs.GetRadius( ) + rhs.GetRadius( ) - size );
	contact->SetBodyData( lhsBody, rhsBody, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return COLLISION::INTERSECTION;
}

uint32 SphereAndHalfSpace( const BoundingSphere& sphere, RigidBody* sphereBody, const CXMFLOAT4& plane, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	const CXMFLOAT3& pos = sphere.GetCenter( );

	float ballDistance = XMVectorGetX( XMPlaneDotCoord( plane, pos ) ) - sphere.GetRadius();

	if ( ballDistance >= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	Contact* contact = data->m_contacts;
	CXMFLOAT3 planeDir( plane.x, plane.y, plane.z );
	contact->SetContactNormal( planeDir );
	contact->SetContactPoint( pos - CXMFLOAT3( planeDir * ( ballDistance + sphere.GetRadius( ) ) ) );
	contact->SetPenetration( -ballDistance );
	contact->SetBodyData( sphereBody, nullptr, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return COLLISION::INTERSECTION;
}

uint32 SphereAndTruePlane( const BoundingSphere& sphere, RigidBody* sphereBody, const CXMFLOAT4& plane, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	const CXMFLOAT3& pos = sphere.GetCenter( );

	float centerDistance = XMVectorGetX( XMPlaneDotCoord( plane, pos ) );

	if ( centerDistance > sphere.GetRadius() )
	{
		return COLLISION::OUTSIDE;
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
	return COLLISION::INTERSECTION;
}

uint32 BoxAndHalfSpace( const CAaboundingbox& box, RigidBody* boxBody, const CXMFLOAT4& plane, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	Contact* contact = data->m_contacts;
	int32 contactsUsed = 0;

	for ( uint32 i = 0; i < 8; ++i )
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

uint32 BoxAndSphere( const CAaboundingbox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	BoundingSphere boxSphere( box );

	if ( boxSphere.Intersect( sphere ) == COLLISION::OUTSIDE )
	{
		return COLLISION::OUTSIDE;
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
		return COLLISION::OUTSIDE;
	}

	Contact* contact = data->m_contacts;
	contact->SetContactNormal( XMVector3Normalize( closestPoint - center ) );
	contact->SetContactPoint( closestPoint );
	contact->SetPenetration( sphere.GetRadius( ) - sqrtf( dist ) );
	contact->SetBodyData( boxBody, sphereBody, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return COLLISION::INTERSECTION;
}

uint32 BoxAndSphere( const COrientedBoundingBox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	const CXMFLOAT3& centre = sphere.GetCenter( );
	CXMFLOAT3 relCentre = XMVector3TransformCoord( centre, XMMatrixInverse( nullptr, box.GetTransform( ) ) );

	float radius = sphere.GetRadius( );
	const CXMFLOAT3& halfSize = box.GetHalfSize( );
	if ( fabsf( relCentre.x ) - radius > halfSize.x ||
		fabsf( relCentre.y ) - radius > halfSize.y || 
		fabsf( relCentre.z ) - radius > halfSize.z )
	{
		return COLLISION::OUTSIDE;
	}

	CXMFLOAT3 closestPoint( relCentre );

	if ( relCentre.x > halfSize.x )
	{
		closestPoint.x = halfSize.x;
	}
	else if ( relCentre.x < -halfSize.x )
	{
		closestPoint.x = -halfSize.x;
	}

	if ( relCentre.y > halfSize.y )
	{
		closestPoint.y = halfSize.y;
	}
	else if ( relCentre.y < -halfSize.y )
	{
		closestPoint.y = -halfSize.y;
	}

	if ( relCentre.z > halfSize.z )
	{
		closestPoint.z = halfSize.z;
	}
	else if ( relCentre.z < -halfSize.z )
	{
		closestPoint.z = -halfSize.z;
	}

	float dist = XMVectorGetX( XMVector3LengthSq( closestPoint - relCentre ) );
	if ( dist > ( sphere.GetRadius( ) * sphere.GetRadius( ) ) )
	{
		return COLLISION::OUTSIDE;
	}

	closestPoint = XMVector3TransformCoord( closestPoint, box.GetTransform( ) );

	Contact* contact = data->m_contacts;
	contact->SetContactNormal( XMVector3Normalize( closestPoint - centre ) );
	contact->SetContactPoint( closestPoint );
	contact->SetPenetration( sphere.GetRadius( ) - sqrtf( dist ) );
	contact->SetBodyData( boxBody, sphereBody, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return COLLISION::INTERSECTION;
}

uint32 BoxAndBox( const CAaboundingbox& lhs, RigidBody* lhsBody, const CAaboundingbox& rhs, RigidBody* rhsBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	if ( lhs.Intersect( rhs ) == COLLISION::OUTSIDE )
	{
		return COLLISION::OUTSIDE;
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
	uint32 bestAxis = 0;

	for ( uint32 i = 0; i < 3; ++i )
	{
		float overlap = ( totalSize[i] - fabsf( std::max( lhsMax[i], rhsMax[i] ) - std::min( lhsMin[i], rhsMin[i] ) ) ) * 0.5f;
		
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
	return COLLISION::INTERSECTION;
}

uint32 BoxAndBox( const COrientedBoundingBox& lhs, RigidBody* lhsBody, const COrientedBoundingBox& rhs, RigidBody* rhsBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	CXMFLOAT3 toCentre = rhs.GetAxisVector( 3 ) - lhs.GetAxisVector( 3 );

	float pen = FLT_MAX;
	uint32 best = UINT_MAX;

	if ( TryAxis( lhs, rhs, lhs.GetAxisVector( 0 ), toCentre, 0, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, lhs.GetAxisVector( 1 ), toCentre, 1, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, lhs.GetAxisVector( 2 ), toCentre, 2, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, rhs.GetAxisVector( 0 ), toCentre, 3, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, rhs.GetAxisVector( 1 ), toCentre, 4, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, rhs.GetAxisVector( 2 ), toCentre, 5, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	uint32 bestSingleAxis = best;

	if ( TryAxis( lhs, rhs, XMVector3Normalize( XMVector3Cross( lhs.GetAxisVector( 0 ), rhs.GetAxisVector( 0 ) ) ), toCentre, 6, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, XMVector3Normalize( XMVector3Cross( lhs.GetAxisVector( 0 ), rhs.GetAxisVector( 1 ) ) ), toCentre, 7, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, XMVector3Normalize( XMVector3Cross( lhs.GetAxisVector( 0 ), rhs.GetAxisVector( 2 ) ) ), toCentre, 8, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, XMVector3Normalize( XMVector3Cross( lhs.GetAxisVector( 1 ), rhs.GetAxisVector( 0 ) ) ), toCentre, 9, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, XMVector3Normalize( XMVector3Cross( lhs.GetAxisVector( 1 ), rhs.GetAxisVector( 1 ) ) ), toCentre, 10, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, XMVector3Normalize( XMVector3Cross( lhs.GetAxisVector( 1 ), rhs.GetAxisVector( 2 ) ) ), toCentre, 11, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, XMVector3Normalize( XMVector3Cross( lhs.GetAxisVector( 2 ), rhs.GetAxisVector( 0 ) ) ), toCentre, 12, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, XMVector3Normalize( XMVector3Cross( lhs.GetAxisVector( 2 ), rhs.GetAxisVector( 1 ) ) ), toCentre, 13, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhs, rhs, XMVector3Normalize( XMVector3Cross( lhs.GetAxisVector( 2 ), rhs.GetAxisVector( 2 ) ) ), toCentre, 14, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( best < 3 )
	{
		FillPointFaceBoxBox( lhs, lhsBody, rhs, rhsBody, toCentre, data, best, pen );
		data->AddContacts( 1 );
		return COLLISION::INTERSECTION;
	}
	else if ( best < 6 )
	{
		FillPointFaceBoxBox( rhs, rhsBody, lhs, lhsBody, -toCentre, data, best - 3, pen );
		data->AddContacts( 1 );
		return COLLISION::INTERSECTION;
	}
	else
	{
		best -= 6;
		uint32 lhsAxisIndex = best / 3;
		uint32 rhsAxisIndex = best % 3;
		CXMFLOAT3 lhsAxis = lhs.GetAxisVector( lhsAxisIndex );
		CXMFLOAT3 rhsAxis = rhs.GetAxisVector( rhsAxisIndex );
		CXMFLOAT3 axis = XMVector3Normalize( XMVector3Cross( lhsAxis, rhsAxis ) );

		if ( XMVectorGetX( XMVector3Dot( axis, toCentre ) ) > 0 )
		{
			axis = -axis;
		}

		CXMFLOAT3 ptOnLhsEdge = lhs.GetHalfSize( );
		CXMFLOAT3 ptOnRhsEdge = rhs.GetHalfSize( );

		for ( uint32 i = 0; i < 3; ++i )
		{
			if ( i == lhsAxisIndex )
			{
				ptOnLhsEdge[i] = 0;
			}
			else if ( XMVectorGetX( XMVector3Dot( lhs.GetAxisVector( i ), axis ) ) > 0 )
			{
				ptOnLhsEdge[i] = -ptOnLhsEdge[i];
			}

			if ( i == rhsAxisIndex )
			{
				ptOnRhsEdge[i] = 0;
			}
			else if ( XMVectorGetX( XMVector3Dot( rhs.GetAxisVector( i ), axis ) ) < 0 )
			{
				ptOnRhsEdge[i] = -ptOnRhsEdge[i];
			}
		}

		ptOnLhsEdge = XMVector3TransformCoord( ptOnLhsEdge, lhs.GetTransform( ) );
		ptOnRhsEdge = XMVector3TransformCoord( ptOnRhsEdge, rhs.GetTransform( ) );

		CXMFLOAT3 vertex = CalcEdgeContactPoint( ptOnLhsEdge, lhsAxis, lhs.GetHalfSize( )[lhsAxisIndex],
												ptOnRhsEdge, rhsAxis, rhs.GetHalfSize( )[rhsAxisIndex],
												bestSingleAxis > 2 );

		Contact* contact = data->m_contacts;

		contact->SetPenetration( pen );
		contact->SetContactNormal( axis );
		contact->SetContactPoint( vertex );
		contact->SetBodyData( lhsBody, rhsBody, data->m_friction, data->m_restitution );
		data->AddContacts( 1 );
		return COLLISION::INTERSECTION;
	}
}

uint32 BoxAndBox( const CAaboundingbox& lhs, RigidBody* lhsBody, const COrientedBoundingBox& rhs, RigidBody* rhsBody, CollisionData* data )
{
	COrientedBoundingBox lhsOBB( lhs );

	return BoxAndBox( lhsOBB, lhsBody, rhs, rhsBody, data );
}

float RayAndBox( const CXMFLOAT3& rayOrigin, const CXMFLOAT3& rayDir, const CXMFLOAT3& max, const CXMFLOAT3& min )
{
	float t_min = 0;
	float t_max = FLT_MAX;

	for ( uint32 i = 0; i < 3; ++i )
	{
		if ( abs( rayDir[i] ) < FLT_EPSILON )
		{
			if ( rayOrigin[i] < min[i] || rayOrigin[i] > max[i] )
			{
				return -1.f;
			}
		}
		else
		{
			float d = 1.0f / rayDir[i];
			float t1 = ( min[i] - rayOrigin[i] ) * d;
			float t2 = ( max[i] - rayOrigin[i] ) * d;

			if ( t1 > t2 )
			{
				std::swap( t1, t2 );
			}

			t_min = std::max( t1, t_min );
			t_max = std::min( t2, t_max );

			if ( t_min > t_max )
			{
				return -1.f;
			}
		}
	}

	return t_min;
}

float RayAndSphere( const CXMFLOAT3& rayOrigin, const CXMFLOAT3& rayDir, const CXMFLOAT3& origin, float radius )
{
	XMVECTOR toShpere = origin - rayOrigin;

	float toShpereSqr = XMVectorGetX( XMVector3LengthSq( toShpere ) );
	float tangentSqr = XMVectorGetX( XMVector3Dot( toShpere, rayDir ) );
	tangentSqr *= tangentSqr;

	float normalVectorSqr = toShpereSqr - tangentSqr;

	float radiusSqr = radius * radius;
	if ( normalVectorSqr > radiusSqr )
	{
		return -1.f;
	}

	return std::max( 0.f, sqrtf( tangentSqr ) - sqrtf( radiusSqr - normalVectorSqr ) );
}
