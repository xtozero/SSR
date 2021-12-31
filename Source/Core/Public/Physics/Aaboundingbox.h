#pragma once

#include "ICollider.h"
#include "Math/Vector.h"
#include "SizedTypes.h"

#include <vector>

class BoxSphereBounds;

struct Matrix;

class CAaboundingbox : public ICollider
{
public:
	virtual void CalcMeshBounds( const MeshData& mesh ) override;
	virtual void Update( const Vector& scaling, const Quaternion& rotation, const Vector& translation, ICollider* original ) override;
	virtual void CalcSubMeshBounds( std::vector<std::unique_ptr<ICollider>>& subColliders ) override;
	virtual float Intersect( const CRay& ray ) const override;
	virtual uint32 Intersect( const Frustum& frustum ) const override;
	uint32 Intersect( const CAaboundingbox& box ) const;

	CAaboundingbox( ) = default;
	explicit CAaboundingbox( const std::vector<CAaboundingbox>& boxes );
	explicit CAaboundingbox( const BoxSphereBounds& bounds );
	CAaboundingbox( const Vector* points, uint32 numPoints );

	void Merge( const Vector& vec );
	Vector Size( ) const
	{
		return m_max - m_min;
	}
	Vector Centroid( ) const
	{
		return ( m_max + m_min ) * 0.5f; 
	}
	void Reset( )
	{
		m_max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		m_min = { FLT_MAX, FLT_MAX, FLT_MAX };
	}
	Vector Point( uint32 i ) const
	{
		return Vector( ( i & 1 ) ? m_min.x : m_max.x, ( i & 2 ) ? m_min.y : m_max.y, ( i & 4 ) ? m_min.z : m_max.z );
	}
	void SetMax( float x, float y, float z ) { m_max = { x, y, z }; }
	void SetMin( float x, float y, float z ) { m_min = { x, y, z }; }

	const Vector& GetMax( ) const { return m_max; }
	const Vector& GetMin( ) const { return m_min; }
	float GetSize( ) const 
	{
		Vector size = Size( );
		return size.x * size.y * size.z; // l * w * h
	}

private:
	Vector m_max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	Vector m_min = { FLT_MAX, FLT_MAX, FLT_MAX };
};

void TransformAABB( CAaboundingbox& result, const CAaboundingbox& src, const Matrix& mat );