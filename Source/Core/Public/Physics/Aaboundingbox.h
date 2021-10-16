#pragma once

#include "ICollider.h"
#include "Math/CXMFloat.h"
#include "SizedTypes.h"

#include <vector>

class CAaboundingbox : public ICollider
{
public:
	virtual void CalcMeshBounds( const MeshData& mesh ) override;
	virtual void Update( const CXMFLOAT3& scaling, const CXMFLOAT4& rotation, const CXMFLOAT3& translation, ICollider* original ) override;
	virtual void CalcSubMeshBounds( std::vector<std::unique_ptr<ICollider>>& subColliders ) override;
	virtual float Intersect( const CRay& ray ) const override;
	virtual uint32 Intersect( const CFrustum& frustum ) const override;
	uint32 Intersect( const CAaboundingbox& box ) const;

	CAaboundingbox( ) = default;
	explicit CAaboundingbox( const std::vector<CAaboundingbox>& boxes );
	explicit CAaboundingbox( const std::vector<CXMFLOAT3>& points );

	void Merge( const CXMFLOAT3& vec );
	void Size( CXMFLOAT3& size ) const
	{
		using namespace DirectX;
		size = m_max - m_min;
	}
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
	CXMFLOAT3 Point( uint32 i ) const
	{
		return CXMFLOAT3( ( i & 1 ) ? m_min.x : m_max.x, ( i & 2 ) ? m_min.y : m_max.y, ( i & 4 ) ? m_min.z : m_max.z );
	}
	void SetMax( float x, float y, float z ) { m_max = { x, y, z }; }
	void SetMin( float x, float y, float z ) { m_min = { x, y, z }; }

	const CXMFLOAT3& GetMax( ) const { return m_max; }
	const CXMFLOAT3& GetMin( ) const { return m_min; }
	float GetSize( ) const 
	{
		CXMFLOAT3 size;
		Size( size );
		return size.x * size.y * size.z; // l * w * h
	}

private:
	CXMFLOAT3 m_max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	CXMFLOAT3 m_min = { FLT_MAX, FLT_MAX, FLT_MAX };
};

void TransformAABB( CAaboundingbox& result, const CAaboundingbox& src, const CXMFLOAT4X4& mat );