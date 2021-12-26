#pragma once
#include "math/CXMFLOAT.h"

#include <vector>

class CBoundingCone
{
public:
	float GetFovX( ) const { return m_fovX; }
	float GetFovY( ) const { return m_fovY; }
	float GetNear( ) const { return m_near; }
	float GetFar( ) const { return m_far; }
	CXMFLOAT4X4 GetLookAt( ) const { return m_lookAt; }

	CBoundingCone( ) = default;
	CBoundingCone( const std::vector<CXMFLOAT3>& points, const CXMFLOAT4X4& projection, const CXMFLOAT3& apex );
	CBoundingCone( const std::vector<CXMFLOAT3>& points, const CXMFLOAT4X4& projection, const CXMFLOAT3& apex, const CXMFLOAT3& dir );
	~CBoundingCone( ) = default;

private:
	CXMFLOAT3 m_direction = { 0.f, 0.f, 1.f };
	CXMFLOAT3 m_apex = { 0.f, 0.f, 0.f };
	float m_fovY = 0.f;
	float m_fovX = 0.f;
	float m_near = 0.001f;
	float m_far = 1,f;
	CXMFLOAT4X4 m_lookAt;
};

