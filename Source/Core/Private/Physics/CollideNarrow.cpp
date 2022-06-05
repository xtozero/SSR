#include "CollideNarrow.h"

#include "AxisAlignedBox.h"
#include "Body.h"
#include "BoundingSphere.h"
#include "Contacts.h"
#include "Frustum.h"
#include "OrientedBox.h"

#include <algorithm>

namespace
{
	void FillPointFaceBoxBox( const OrientedBox& lhs, RigidBody* lhsBody, const OrientedBox& rhs, RigidBody* rhsBody, const Vector& toCentre, CollisionData* data, uint32 best, float pen )
	{
		Contact* contact = data->m_contacts;

		Vector normal = lhs.GetAxisVector( best );

		if ( ( normal | toCentre ) > 0 )
		{
			normal = -normal;
		}

		Vector vertex = rhs.GetHalfSize();
		if ( ( rhs.GetAxisVector( 0 ) | normal ) < 0 )
		{
			vertex.x = -vertex.x;
		}
		if ( ( rhs.GetAxisVector( 1 ) | normal ) < 0 )
		{
			vertex.y = -vertex.y;
		}
		if ( ( rhs.GetAxisVector( 2 ) | normal ) < 0 )
		{
			vertex.z = -vertex.z;
		}

		contact->SetContactNormal( normal );
		contact->SetPenetration( pen );
		contact->SetContactPoint( rhs.GetTransform().TransformPosition( vertex ) );
		contact->SetBodyData( lhsBody, rhsBody, data->m_friction, data->m_restitution );
	}

	Point CalcEdgeContactPoint( const Point& pLhs, const Point& dLhs, float lhsSize, const Vector& pRhs, const Vector& dRhs, float rhsSize, bool useLhs )
	{
		float smLhs = dLhs.LengthSqrt();
		float smRhs = dRhs.LengthSqrt();
		float dpLhsRhs = dLhs | dRhs;

		Vector toSt = pLhs - pRhs;
		float dpStaLhs = toSt | dLhs;
		float dpStaRhs = toSt | dRhs;

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
			Point cLhs = pLhs + ( dLhs * mua );
			Point cRhs = pRhs + ( dRhs * mub );

			return ( cLhs + cRhs ) * 0.5;
		}
	}

	bool SweptSpherePlaneIntersection( float& t0, float& t1, const Plane& plane, const Point& origin, float radius, const Vector& sweepDir )
	{
		float bdotn = plane.PlaneDot( origin );
		float ddotn = plane.PlaneDotNormal( sweepDir );

		if ( ddotn == 0 )
		{
			if ( bdotn <= radius )
			{
				t0 = 0;
				t1 = 1e32f;
				return true;
			}

			return false;
		}
		else
		{
			float tmp0 = ( radius - bdotn ) / ddotn;
			float tmp1 = ( -radius - bdotn ) / ddotn;
			t0 = std::min( tmp0, tmp1 );
			t1 = std::max( tmp0, tmp1 );
			return true;
		}
	}
}

void CollisionData::AddContacts( int32 count )
{
	m_contactsLeft -= count;
	m_contactsCount += count;

	m_contacts += count;
}

void CollisionData::Reset( Contact* contactArray, int32 maxContacts )
{
	m_contactsLeft = maxContacts;
	m_contactsCount = 0;
	m_contactArray = contactArray;
	m_contacts = m_contactArray;
}

bool CollisionData::HasMoreContact() const
{
	return m_contactsLeft > 0;
}

uint32 SphereAndSphere( const BoundingSphere& lhs, RigidBody* lhsBody, const BoundingSphere& rhs, RigidBody* rhsBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	BoundingSphere lhsWS( lhsBody->GetTransform().TransformPosition( lhs.GetCenter() ), lhs.GetRadius());
	BoundingSphere rhsWS( rhsBody->GetTransform().TransformPosition( rhs.GetCenter() ), rhs.GetRadius());

	const Point& lhsPos = lhsWS.GetCenter();
	const Point& rhsPos = rhsWS.GetCenter();

	Vector midline = lhsPos - rhsPos;
	float size = midline.Length();

	if ( size <= 0.f || size >= lhsWS.GetRadius() + rhsWS.GetRadius() )
	{
		return COLLISION::OUTSIDE;
	}

	Vector normal = midline / size;

	Contact* contact = data->m_contacts;
	contact->SetContactNormal( normal );
	contact->SetContactPoint( lhsPos - ( midline * 0.5f ) );
	contact->SetPenetration( lhsWS.GetRadius() + rhsWS.GetRadius() - size );
	contact->SetBodyData( lhsBody, rhsBody, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return COLLISION::INTERSECTION;
}

uint32 SphereAndHalfSpace( const BoundingSphere& sphere, RigidBody* sphereBody, const Plane& plane, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	BoundingSphere sphereWS( sphereBody->GetTransform().TransformPosition( sphere.GetCenter() ), sphere.GetRadius() );
	const Point& pos = sphereWS.GetCenter();

	float ballDistance = plane.PlaneDot( pos ) - sphereWS.GetRadius();

	if ( ballDistance >= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	Contact* contact = data->m_contacts;
	Vector planeDir = plane.GetNormal();
	contact->SetContactNormal( planeDir );
	contact->SetContactPoint( pos - ( planeDir * ( ballDistance + sphereWS.GetRadius() ) ) );
	contact->SetPenetration( -ballDistance );
	contact->SetBodyData( sphereBody, nullptr, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return COLLISION::INTERSECTION;
}

uint32 SphereAndTruePlane( const BoundingSphere& sphere, RigidBody* sphereBody, const Plane& plane, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	BoundingSphere sphereWS( sphereBody->GetTransform().TransformPosition( sphere.GetCenter() ), sphere.GetRadius() );
	const Point& pos = sphereWS.GetCenter();

	float centerDistance = plane.PlaneDot( pos );

	if ( centerDistance > sphereWS.GetRadius() )
	{
		return COLLISION::OUTSIDE;
	}

	Vector normal = plane.GetNormal();
	float penetration = -centerDistance;
	if ( centerDistance < 0 )
	{
		normal *= -1;
		penetration = -penetration;
	}
	penetration += sphereWS.GetRadius();

	Contact* contact = data->m_contacts;
	contact->SetContactNormal( normal );
	contact->SetContactPoint( pos - ( normal * centerDistance ) );
	contact->SetPenetration( penetration );
	contact->SetBodyData( sphereBody, nullptr, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return COLLISION::INTERSECTION;
}

uint32 SphereAndFrusturm( const Point& origin, float radius, const Frustum& frustum )
{
	const Plane( &planes )[6] = frustum.GetPlanes();

	bool inside = true;

	for ( uint32 i = 0; ( i < 6 ) && inside; i++ )
	{
		inside = inside && ( ( planes[i].PlaneDot( origin ) + radius ) >= 0.f );
	}

	return inside;
}

bool SphereAndFrusturm( const Point& origin, float radius, const Frustum& frustum, const Vector& sweepDir )
{
	float displacement[12];
	uint32 count = 0;
	float t0 = -1;
	float t1 = -1;
	bool inFrustum = false;

	const Plane( &planes )[6] = frustum.GetPlanes();

	for ( uint32 i = 0; i < 6; ++i )
	{
		if ( SweptSpherePlaneIntersection( t0, t1, planes[i], origin, radius, sweepDir ) )
		{
			if ( t0 >= 0.f )
			{
				displacement[count++] = t0;
			}
			if ( t1 >= 0.f )
			{
				displacement[count++] = t1;
			}
		}
	}

	for ( uint32 i = 0; i < count; ++i )
	{
		float extendRadius = radius * 1.1f;
		Point center( origin + sweepDir * displacement[i] );
		uint32 result = SphereAndFrusturm( center, extendRadius, frustum );
		inFrustum |= ( result > COLLISION::OUTSIDE );
	}

	return inFrustum;
}

uint32 BoxAndHalfSpace( const AxisAlignedBox& box, RigidBody* boxBody, const Plane& plane, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	Vector point[8];
	for ( uint32 i = 0; i < 8; ++i )
	{
		point[i] = boxBody->GetTransform().TransformPosition( box.Point( i ) );
	}

	AxisAlignedBox boxWS( point, std::extent_v<decltype( point )> );

	Contact* contact = data->m_contacts;
	int32 contactsUsed = 0;

	for ( uint32 i = 0; i < 8; ++i )
	{
		Point vertexPos = boxWS.Point( i );

		float vertexDistance = plane.PlaneDotNormal( vertexPos );
		if ( vertexDistance + plane.w <= 0.f )
		{
			Vector normal = plane.GetNormal();
			contact->SetContactNormal( normal );
			contact->SetContactPoint( vertexPos + ( normal * ( vertexDistance + plane.w ) ) );
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

uint32 BoxAndSphere( const AxisAlignedBox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	Vector point[8];
	for ( uint32 i = 0; i < 8; ++i )
	{
		point[i] = boxBody->GetTransform().TransformPosition( box.Point( i ) );
	}

	AxisAlignedBox boxWS( point, std::extent_v<decltype( point )> );

	BoundingSphere boxSphere( boxWS );

	if ( boxSphere.Intersect( sphere ) == COLLISION::OUTSIDE )
	{
		return COLLISION::OUTSIDE;
	}

	const Point& center = sphere.GetCenter();
	Point closestPoint = center;

	const Point& boxMax = boxWS.GetMax();
	const Point& boxMin = boxWS.GetMin();
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

	float dist = ( closestPoint - center ).LengthSqrt();
	if ( dist > sphere.GetRadius() * sphere.GetRadius() )
	{
		return COLLISION::OUTSIDE;
	}

	Contact* contact = data->m_contacts;
	contact->SetContactNormal( ( closestPoint - center ).GetNormalized() );
	contact->SetContactPoint( closestPoint );
	contact->SetPenetration( sphere.GetRadius() - sqrtf( dist ) );
	contact->SetBodyData( boxBody, sphereBody, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return COLLISION::INTERSECTION;
}

uint32 BoxAndSphere( const OrientedBox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	OrientedBox boxWS( box.GetHalfSize(), boxBody->GetTransform() * box.GetTransform() );
	BoundingSphere sphereWS( sphereBody->GetTransform().TransformPosition( sphere.GetCenter() ), sphere.GetRadius());

	const Point& centre = sphereWS.GetCenter();
	Point relCentre = boxWS.GetTransform().Inverse().TransformPosition( centre );

	float radius = sphereWS.GetRadius();
	const Vector& halfSize = boxWS.GetHalfSize();
	if ( fabsf( relCentre.x ) - radius > halfSize.x ||
		fabsf( relCentre.y ) - radius > halfSize.y ||
		fabsf( relCentre.z ) - radius > halfSize.z )
	{
		return COLLISION::OUTSIDE;
	}

	Point closestPoint( relCentre );

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

	float dist = ( closestPoint - relCentre ).LengthSqrt();
	if ( dist > ( sphereWS.GetRadius() * sphereWS.GetRadius() ) )
	{
		return COLLISION::OUTSIDE;
	}

	closestPoint = boxWS.GetTransform().TransformPosition( closestPoint );

	Contact* contact = data->m_contacts;
	contact->SetContactNormal( ( closestPoint - centre ).GetNormalized() );
	contact->SetContactPoint( closestPoint );
	contact->SetPenetration( sphereWS.GetRadius() - sqrtf( dist ) );
	contact->SetBodyData( boxBody, sphereBody, data->m_friction, data->m_restitution );

	data->AddContacts( 1 );
	return COLLISION::INTERSECTION;
}

uint32 BoxAndBox( const AxisAlignedBox& lhs, RigidBody* lhsBody, const AxisAlignedBox& rhs, RigidBody* rhsBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	Vector point[8];
	for ( uint32 i = 0; i < 8; ++i )
	{
		point[i] = lhsBody->GetTransform().TransformPosition( lhs.Point( i ) );
	}

	AxisAlignedBox lhsWS( point, std::extent_v<decltype( point )> );

	for ( uint32 i = 0; i < 8; ++i )
	{
		point[i] = rhsBody->GetTransform().TransformPosition( rhs.Point( i ) );
	}

	AxisAlignedBox rhsWS( point, std::extent_v<decltype( point )> );

	if ( lhsWS.Intersect( rhsWS ) == COLLISION::OUTSIDE )
	{
		return COLLISION::OUTSIDE;
	}

	const Point& lhsMin = lhsWS.GetMin();
	const Point& lhsMax = lhsWS.GetMax();
	const Point& rhsMin = rhsWS.GetMin();
	const Point& rhsMax = rhsWS.GetMax();

	Vector totalSize = lhsWS.Size();
	Vector rhsSize = rhsWS.Size();
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

	Vector axis[3] = {
		{ 1.f, 0.f, 0.f },
		{ 0.f, 1.f, 0.f },
		{ 0.f, 0.f, 1.f}
	};

	Vector normal = axis[bestAxis];
	Point lhsCentre = lhsWS.Centroid();
	Point rhsCentre = rhsWS.Centroid();

	Vector toCentre = ( rhsCentre - lhsCentre ).GetNormalized();
	if ( ( normal | toCentre ) > 0 )
	{
		normal *= -1.f;
	}

	Vector vertex = rhsMax;
	if ( ( axis[0] | toCentre ) >= 0 )
	{
		vertex.x = rhsMin.x;
	}

	if ( ( axis[1] | toCentre ) >= 0 )
	{
		vertex.y = rhsMin.y;
	}

	if ( ( axis[2] | toCentre ) >= 0 )
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

uint32 BoxAndBox( const OrientedBox& lhs, RigidBody* lhsBody, const OrientedBox& rhs, RigidBody* rhsBody, CollisionData* data )
{
	if ( data->m_contactsLeft <= 0 )
	{
		return COLLISION::OUTSIDE;
	}

	OrientedBox lhsWS( lhs.GetHalfSize(), lhsBody->GetTransform() * lhs.GetTransform());
	OrientedBox rhsWS( rhs.GetHalfSize(), rhsBody->GetTransform() * rhs.GetTransform());

	Vector toCentre = rhsWS.GetAxisVector( 3 ) - lhsWS.GetAxisVector( 3 );

	float pen = FLT_MAX;
	uint32 best = UINT_MAX;

	if ( TryAxis( lhsWS, rhsWS, lhsWS.GetAxisVector( 0 ), toCentre, 0, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, lhsWS.GetAxisVector( 1 ), toCentre, 1, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, lhsWS.GetAxisVector( 2 ), toCentre, 2, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, rhsWS.GetAxisVector( 0 ), toCentre, 3, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, rhsWS.GetAxisVector( 1 ), toCentre, 4, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, rhsWS.GetAxisVector( 2 ), toCentre, 5, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	uint32 bestSingleAxis = best;

	if ( TryAxis( lhsWS, rhsWS, ( lhsWS.GetAxisVector( 0 ) ^ rhsWS.GetAxisVector( 0 ) ).GetNormalized(), toCentre, 6, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, ( lhsWS.GetAxisVector( 0 ) ^ rhsWS.GetAxisVector( 1 ) ).GetNormalized(), toCentre, 7, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, ( lhsWS.GetAxisVector( 0 ) ^ rhsWS.GetAxisVector( 2 ) ).GetNormalized(), toCentre, 8, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, ( lhsWS.GetAxisVector( 1 ) ^ rhsWS.GetAxisVector( 0 ) ).GetNormalized(), toCentre, 9, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, ( lhsWS.GetAxisVector( 1 ) ^ rhsWS.GetAxisVector( 1 ) ).GetNormalized(), toCentre, 10, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, ( lhsWS.GetAxisVector( 1 ) ^ rhsWS.GetAxisVector( 2 ) ).GetNormalized(), toCentre, 11, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, ( lhsWS.GetAxisVector( 2 ) ^ rhsWS.GetAxisVector( 0 ) ).GetNormalized(), toCentre, 12, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, ( lhsWS.GetAxisVector( 2 ) ^ rhsWS.GetAxisVector( 1 ) ).GetNormalized(), toCentre, 13, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( TryAxis( lhsWS, rhsWS, ( lhsWS.GetAxisVector( 2 ) ^ rhsWS.GetAxisVector( 2 ) ).GetNormalized(), toCentre, 14, pen, best ) == false )
	{
		return COLLISION::OUTSIDE;
	}

	if ( best < 3 )
	{
		FillPointFaceBoxBox( lhsWS, lhsBody, rhsWS, rhsBody, toCentre, data, best, pen );
		data->AddContacts( 1 );
		return COLLISION::INTERSECTION;
	}
	else if ( best < 6 )
	{
		FillPointFaceBoxBox( rhsWS, rhsBody, lhsWS, lhsBody, -toCentre, data, best - 3, pen );
		data->AddContacts( 1 );
		return COLLISION::INTERSECTION;
	}
	else
	{
		best -= 6;
		uint32 lhsAxisIndex = best / 3;
		uint32 rhsAxisIndex = best % 3;
		Vector lhsAxis = lhsWS.GetAxisVector( lhsAxisIndex );
		Vector rhsAxis = rhsWS.GetAxisVector( rhsAxisIndex );
		Vector axis = ( lhsAxis ^ rhsAxis ).GetNormalized();

		if ( ( axis | toCentre ) > 0 )
		{
			axis = -axis;
		}

		Vector ptOnLhsEdge = lhsWS.GetHalfSize();
		Vector ptOnRhsEdge = rhsWS.GetHalfSize();

		for ( uint32 i = 0; i < 3; ++i )
		{
			if ( i == lhsAxisIndex )
			{
				ptOnLhsEdge[i] = 0;
			}
			else if ( ( lhsWS.GetAxisVector( i ) | axis ) > 0 )
			{
				ptOnLhsEdge[i] = -ptOnLhsEdge[i];
			}

			if ( i == rhsAxisIndex )
			{
				ptOnRhsEdge[i] = 0;
			}
			else if ( ( rhsWS.GetAxisVector( i ) | axis ) < 0 )
			{
				ptOnRhsEdge[i] = -ptOnRhsEdge[i];
			}
		}

		ptOnLhsEdge = lhsWS.GetTransform().TransformPosition( ptOnLhsEdge );
		ptOnRhsEdge = rhsWS.GetTransform().TransformPosition( ptOnRhsEdge );

		Point vertex = CalcEdgeContactPoint( ptOnLhsEdge, lhsAxis, lhsWS.GetHalfSize()[lhsAxisIndex],
			ptOnRhsEdge, rhsAxis, rhsWS.GetHalfSize()[rhsAxisIndex],
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

uint32 BoxAndBox( const AxisAlignedBox& lhs, RigidBody* lhsBody, const OrientedBox& rhs, RigidBody* rhsBody, CollisionData* data )
{
	OrientedBox lhsOBB( lhs );

	return BoxAndBox( lhsOBB, lhsBody, rhs, rhsBody, data );
}

uint32 BoxAndFrustum( const Point& min, const Point& max, const Frustum& frustum )
{
	const Frustum::LookUpTable& lut = frustum.GetVertexLUT();
	const Plane( &planes )[6] = frustum.GetPlanes();

	uint32 result = COLLISION::INSIDE;
	for ( uint32 i = 0; i < 6; ++i )
	{
		Vector p( ( lut[i] & Frustum::X_MAX ) ? max.x : min.x, ( lut[i] & Frustum::Y_MAX ) ? max.y : min.y, ( lut[i] & Frustum::Z_MAX ) ? max.z : min.z );
		Vector n( ( lut[i] & Frustum::X_MAX ) ? min.x : max.x, ( lut[i] & Frustum::Y_MAX ) ? min.y : max.y, ( lut[i] & Frustum::Z_MAX ) ? min.z : max.z );

		if ( planes[i].PlaneDot( p ) < 0 )
		{
			return COLLISION::OUTSIDE;
		}

		if ( planes[i].PlaneDot( n ) < 0 )
		{
			result = COLLISION::INTERSECTION;
		}
	}

	return result;
}

float RayAndBox( const Point& rayOrigin, const Vector& rayDir, const Point& max, const Point& min )
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

float RayAndSphere( const Point& rayOrigin, const Vector& rayDir, const Point& origin, float radius )
{
	Vector toShpere = origin - rayOrigin;

	float toShpereSqr = toShpere.LengthSqrt();
	float tangentSqr = toShpere | rayDir;
	tangentSqr *= tangentSqr;

	float normalVectorSqr = toShpereSqr - tangentSqr;

	float radiusSqr = radius * radius;
	if ( normalVectorSqr > radiusSqr )
	{
		return -1.f;
	}

	return std::max( 0.f, sqrtf( tangentSqr ) - sqrtf( radiusSqr - normalVectorSqr ) );
}
