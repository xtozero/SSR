#include "Aaboundingbox.h"

#include "BoxSphereBounds.h"
#include "CollideNarrow.h"
#include "Frustum.h"
#include "Math/TransformationMatrix.h"
#include "Ray.h"

#include <algorithm> 

using namespace DirectX;

void CAaboundingbox::CalcMeshBounds( const MeshData& mesh )
{
//	m_min = CXMFLOAT3( FLT_MAX, FLT_MAX, FLT_MAX );
//	m_max = CXMFLOAT3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
//
//	uint32 verticesCount = mesh.m_vertices;
//	const MeshVertex* pVertices = static_cast<const MeshVertex*>( mesh.m_pVertexData );
//
//	for ( uint32 i = 0; i < verticesCount; ++i )
//	{
//		m_min.x = m_min.x > pVertices[i].m_position.x ? pVertices[i].m_position.x : m_min.x;
//		m_min.y = m_min.y > pVertices[i].m_position.y ? pVertices[i].m_position.y : m_min.y;
//		m_min.z = m_min.z > pVertices[i].m_position.z ? pVertices[i].m_position.z : m_min.z;
//		m_max.x = m_max.x < pVertices[i].m_position.x ? pVertices[i].m_position.x : m_max.x;
//		m_max.y = m_max.y < pVertices[i].m_position.y ? pVertices[i].m_position.y : m_max.y;
//		m_max.z = m_max.z < pVertices[i].m_position.z ? pVertices[i].m_position.z : m_max.z;
//	}
//
//#ifdef TEST_CODE
//	DebugMsg( "-------------------AABB-------------------\n" );
//	DebugMsg( "max %f, %f, %f\n", m_max.x, m_max.y, m_max.z );
//	DebugMsg( "min %f, %f, %f\n", m_min.x, m_min.y, m_min.z );
//	DebugMsg( "------------------------------------------\n" );
//#endif
}

void CAaboundingbox::Update( const Vector& scaling, const Quaternion& rotation, const Vector& translation, ICollider* original )
{
	CAaboundingbox* orig = dynamic_cast<CAaboundingbox*>( original );
	if ( orig == nullptr )
	{
		return;
	}

	Vector v[8] = {
		Vector( orig->m_max.x, orig->m_max.y, orig->m_max.z ),
		Vector( orig->m_max.x, orig->m_min.y, orig->m_max.z ),
		Vector( orig->m_max.x, orig->m_max.y, orig->m_min.z ),
		Vector( orig->m_min.x, orig->m_max.y, orig->m_max.z ),
		Vector( orig->m_min.x, orig->m_max.y, orig->m_min.z ),
		Vector( orig->m_min.x, orig->m_min.y, orig->m_max.z ),
		Vector( orig->m_max.x, orig->m_min.y, orig->m_min.z ),
		Vector( orig->m_min.x, orig->m_min.y, orig->m_min.z ),
	};
	
	Matrix matrix = ScaleRotationTranslationMatrix( scaling, rotation, translation );

	for ( uint32 i = 0; i < 8; ++i )
	{
		v[i] = matrix.TransformPosition( v[i] );
	}

	m_min = Vector( FLT_MAX, FLT_MAX, FLT_MAX );
	m_max = Vector( -FLT_MAX, -FLT_MAX, -FLT_MAX );

	for ( uint32 i = 0; i < 8; ++i )
	{
		m_min.x = m_min.x > v[i].x ? v[i].x : m_min.x;
		m_min.y = m_min.y > v[i].y ? v[i].y : m_min.y;
		m_min.z = m_min.z > v[i].z ? v[i].z : m_min.z;
		m_max.x = m_max.x < v[i].x ? v[i].x : m_max.x;
		m_max.y = m_max.y < v[i].y ? v[i].y : m_max.y;
		m_max.z = m_max.z < v[i].z ? v[i].z : m_max.z;
	}
}

void CAaboundingbox::CalcSubMeshBounds( std::vector<std::unique_ptr<ICollider>>& subColliders )
{
	subColliders.clear( );

	Vector length = m_max - m_min;

	for ( uint32 i = 0; i < 8 * 8 * 8; ++i )
	{
		float ix = static_cast<float>( i & 0x7 );
		float iy = static_cast<float>( ( i >> 3 ) & 0x7 );
		float iz = static_cast<float>( ( i >> 6 ) & 0x7 );

		CAaboundingbox* subBox = new CAaboundingbox;
		subBox->SetMin( m_min.x + ( ix / 8.f ) * length.x, m_min.y + ( iy / 8.f ) * length.y, m_min.z + ( iz / 8.f ) * length.z );
		subBox->SetMax( m_min.x + ( ( ix + 1 ) / 8.f ) * length.x, m_min.y + ( ( iy + 1 ) / 8.f ) * length.y, m_min.z + ( ( iz + 1 ) / 8.f ) * length.z );

		subColliders.emplace_back( subBox );
	}
}

float CAaboundingbox::Intersect( const CRay& ray ) const
{
	return RayAndBox( ray.GetOrigin( ), ray.GetDir( ), m_max, m_min );
}

uint32 CAaboundingbox::Intersect( const Frustum& frustum ) const
{
	using namespace DirectX;

	const Frustum::LookUpTable& lut = frustum.GetVertexLUT( );
	const Plane( &planes )[6] = frustum.GetPlanes( );

	uint32 result = COLLISION::INSIDE;
	for ( uint32 i = 0; i < 6; ++i )
	{
		Vector p( ( lut[i] & Frustum::X_MAX ) ? m_max.x : m_min.x, ( lut[i] & Frustum::Y_MAX ) ? m_max.y : m_min.y, ( lut[i] & Frustum::Z_MAX ) ? m_max.z : m_min.z );
		Vector n( ( lut[i] & Frustum::X_MAX ) ? m_min.x : m_max.x, ( lut[i] & Frustum::Y_MAX ) ? m_min.y : m_max.y, ( lut[i] & Frustum::Z_MAX ) ? m_min.z : m_max.z );

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

uint32 CAaboundingbox::Intersect( const CAaboundingbox& box ) const
{
	for ( uint32 i = 0; i < 3; ++i )
	{
		if ( m_max[i] < box.m_min[i] || m_min[i] > box.m_max[i] )
		{
			return COLLISION::OUTSIDE;
		}
	}

	if ( m_min.x <= box.m_min.x && box.m_max.x <= m_max.x &&
		m_min.y <= box.m_min.y && box.m_max.y <= m_max.y &&
		m_min.z <= box.m_min.z && box.m_max.z <= m_max.z )
	{
		return COLLISION::INSIDE;
	}

	return COLLISION::INTERSECTION;
}

CAaboundingbox::CAaboundingbox( const std::vector<CAaboundingbox>& boxes )
{
	for ( const auto& box : boxes )
	{
		Merge( box.m_max );
		Merge( box.m_min );
	}
}

CAaboundingbox::CAaboundingbox( const BoxSphereBounds& bounds )
{
	Merge( bounds.Origin( ) - bounds.HalfSize( ) );
	Merge( bounds.Origin( ) + bounds.HalfSize( ) );
}

CAaboundingbox::CAaboundingbox( const Vector* points, uint32 numPoints )
{
	for ( uint32 i = 0; i < numPoints; ++i )
	{
		Merge( points[i] );
	}
}

void CAaboundingbox::Merge( const Vector& vec )
{
	m_max.x = std::max( m_max.x, vec.x );
	m_max.y = std::max( m_max.y, vec.y );
	m_max.z = std::max( m_max.z, vec.z );
	m_min.x = std::min( m_min.x, vec.x );
	m_min.y = std::min( m_min.y, vec.y );
	m_min.z = std::min( m_min.z, vec.z );
}

void TransformAABB( CAaboundingbox& result, const CAaboundingbox& src, const Matrix& mat )
{
	Vector point[8];
	for ( uint32 i = 0; i < 8; ++i )
	{
		point[i] = src.Point( i );
	}

	result.Reset( );

	for ( uint32 i = 0; i < 8; ++i )
	{
		point[i] = mat.TransformPosition( point[i] );
		result.Merge( point[i] );
	}
}