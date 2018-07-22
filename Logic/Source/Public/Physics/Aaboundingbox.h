#pragma once

#include "IRigidBody.h"
#include "Math/CXMFloat.h"

#include <vector>

class CAaboundingbox : public IRigidBody
{
public:
	virtual void CreateRigideBody( const IMesh& mesh ) override;
	virtual void Update( const CXMFLOAT4X4& matrix, IRigidBody* original ) override;
	virtual void CalcSubRigidBody( std::vector<std::unique_ptr<IRigidBody>>& subRigidBody ) override;
	virtual float Intersect( const CRay* ray ) const override;
	virtual int Intersect( const CFrustum& frustum ) const override;

	CAaboundingbox( ) = default;
	explicit CAaboundingbox( const std::vector<CAaboundingbox>& boxes );
	explicit CAaboundingbox( const std::vector<CXMFLOAT3>& points );

	void Merge( const CXMFLOAT3& vec );
	void Centroid( CXMFLOAT3& centroid ) const
	{
		using namespace DirectX;
		centroid = ( m_max + m_min ) * 0.5f; 
	}
	void Reset( )
	{
		m_max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		m_min = { FLT_MAX, FLT_MAX, FLT_MAX };
	}
	CXMFLOAT3 Point( int i ) const
	{
		return CXMFLOAT3( ( i & 1 ) ? m_min.x : m_max.x, ( i & 2 ) ? m_min.y : m_max.y, ( i & 4 ) ? m_min.z : m_max.z );
	}
	void SetMax( float x, float y, float z ) { m_max = { x, y, z }; }
	void SetMin( float x, float y, float z ) { m_min = { x, y, z }; }

	const CXMFLOAT3& GetMax( ) const { return m_max; }
	const CXMFLOAT3& GetMin( ) const { return m_min; }

private:
	CXMFLOAT3 m_max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	CXMFLOAT3 m_min = { FLT_MAX, FLT_MAX, FLT_MAX };
};

void TransformAABB( CAaboundingbox& result, const CAaboundingbox& src, const CXMFLOAT4X4& mat );