#include "BoundingCone.h"

#include "BoundingSphere.h"
#include "SizedTypes.h"

#include <algorithm>

CBoundingCone::CBoundingCone( const std::vector<CXMFLOAT3>& points, const CXMFLOAT4X4& projection, const CXMFLOAT3& apex )
	: m_apex( apex )
{
	using namespace DirectX;

	switch ( points.size() )
	{
	case 0:
	{
		m_direction = CXMFLOAT3( 0.f, 0.f, -1.f );
		m_fovX = 0.f;
		m_fovY = 0.f;
		m_lookAt = XMMatrixIdentity( );
		break;
	}
	default:
	{
		std::vector<CXMFLOAT3> projectedPoints = points;
		for ( auto& point : projectedPoints )
		{
			point = XMVector3TransformCoord( point, projection );
		}

		BoundingSphere sphere( points );

		m_direction = XMVector3Normalize( sphere.GetCenter( ) - m_apex );

		CXMFLOAT3 axis( 0.f, 1.f, 0.f );

		if ( fabsf( XMVectorGetX( XMVector3Dot( axis, m_direction ) ) ) > 0.99f )
		{
			axis = { 0.f, 0.f, 1.f };
		}

		m_lookAt = XMMatrixLookAtLH( m_apex, m_apex + m_direction, axis );

		float maxTanX = 0.f;
		float maxTanY = 0.f;
		m_near = FLT_MAX;
		m_far = -FLT_MAX;

		for ( const auto& point : points )
		{
			CXMFLOAT3 tmp = XMVector3TransformCoord( point, m_lookAt );

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

CBoundingCone::CBoundingCone( const std::vector<CXMFLOAT3>& points, const CXMFLOAT4X4& projection, const CXMFLOAT3& apex, const CXMFLOAT3& dir ) :
	m_apex( apex )
{
	using namespace DirectX;

	m_direction = XMVector3Normalize( dir );

	CXMFLOAT3 axis( 0.f, 1.f, 0.f );

	if ( fabsf( XMVectorGetX( XMVector3Dot( axis, m_direction ) ) ) > 0.99f )
	{
		axis = { 0.f, 0.f, 1.f };
	}

	m_lookAt = XMMatrixLookAtLH( m_apex, m_apex + m_direction, axis );

	CXMFLOAT4X4 concatMatrix = XMMatrixMultiply( projection, m_lookAt );

	float maxTanX = 0.f;
	float maxTanY = 0.f;
	m_near = FLT_MAX;
	m_far = -FLT_MAX;

	for ( const CXMFLOAT3& point : points )
	{
		CXMFLOAT3 newPoint = XMVector3TransformCoord( point, concatMatrix );

		maxTanX = std::max( maxTanX, abs( newPoint.x / newPoint.z ) );
		maxTanY = std::max( maxTanY, abs( newPoint.y / newPoint.z ) );
		m_near = std::min( m_near, newPoint.z );
		m_far = std::max( m_far, newPoint.z );
	}

	m_fovX = atanf( maxTanX );
	m_fovY = atanf( maxTanY );
}
