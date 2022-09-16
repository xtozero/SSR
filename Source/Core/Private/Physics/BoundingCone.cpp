#include "BoundingCone.h"

#include "BoundingSphere.h"
#include "SizedTypes.h"
#include "TransformationMatrix.h"

#include <algorithm>

CBoundingCone::CBoundingCone( const Vector* points, size_t count, const Matrix& projection, const Vector& apex )
	: m_apex( apex )
{
	switch ( count )
	{
	case 0:
	{
		m_direction = -Vector::ZAxisVector;
		m_fovX = 0.f;
		m_fovY = 0.f;
		m_lookAt = Matrix::Identity;
		break;
	}
	default:
	{
		std::vector<Vector> projectedPoints( count );
		for ( size_t i = 0; i < count; ++i )
		{
			projectedPoints[i] = projection.TransformPosition( points[i] );
		}

		BoundingSphere sphere( points, count );

		m_direction = ( sphere.GetCenter() - m_apex ).GetNormalized();

		Vector axis = Vector::YAxisVector;
		if ( fabsf( axis | m_direction ) > 0.99f )
		{
			axis = Vector::ZAxisVector;
		}

		m_lookAt = LookAtMatrix( m_apex, m_apex + m_direction, axis );

		float maxTanX = 0.f;
		float maxTanY = 0.f;
		m_near = FLT_MAX;
		m_far = -FLT_MAX;

		for ( size_t i = 0; i < count; ++i )
		{
			const auto& point = points[i];
			Vector tmp = m_lookAt.TransformPosition( point );

			maxTanX = std::max( maxTanX, abs( tmp.x / tmp.z ) );
			maxTanY = std::max( maxTanY, abs( tmp.y / tmp.z ) );
			m_near = std::min( m_near, tmp.z );
			m_far = std::max( m_far, tmp.z );
		}

		m_fovX = atanf( maxTanX );
		m_fovY = atanf( maxTanY );
		break;
	}
	}
}

CBoundingCone::CBoundingCone( const Vector* points, size_t count, const Matrix& projection, const Vector& apex, const Vector& dir ) :
	m_apex( apex )
{
	m_direction = dir.GetNormalized();

	Vector axis = Vector::YAxisVector;
	if ( fabsf( axis | m_direction ) > 0.99f )
	{
		axis = Vector::ZAxisVector;
	}

	m_lookAt = LookAtMatrix( m_apex, m_apex + m_direction, axis );

	Matrix concatMatrix = projection * m_lookAt;

	float maxTanX = 0.f;
	float maxTanY = 0.f;
	m_near = FLT_MAX;
	m_far = -FLT_MAX;

	for ( size_t i = 0; i < count; ++i )
	{
		const auto& point = points[i];
		Vector newPoint = concatMatrix.TransformPosition( point );

		maxTanX = std::max( maxTanX, abs( newPoint.x / newPoint.z ) );
		maxTanY = std::max( maxTanY, abs( newPoint.y / newPoint.z ) );
		m_near = std::min( m_near, newPoint.z );
		m_far = std::max( m_far, newPoint.z );
	}

	m_fovX = atanf( maxTanX );
	m_fovY = atanf( maxTanY );
}
