#pragma once

#include "Math/Matrix.h"
#include "Math/Vector.h"

#include <vector>

class CBoundingCone
{
public:
	float GetFovX( ) const { return m_fovX; }
	float GetFovY( ) const { return m_fovY; }
	float GetNear( ) const { return m_near; }
	float GetFar( ) const { return m_far; }
	Matrix GetLookAt( ) const { return m_lookAt; }

	CBoundingCone( ) = default;
	CBoundingCone( const std::vector<Vector>& points, const Matrix& projection, const Vector& apex );
	CBoundingCone( const std::vector<Vector>& points, const Matrix& projection, const Vector& apex, const Vector& dir );
	~CBoundingCone( ) = default;

private:
	Vector m_direction = Vector::ZAxisVector;
	Vector m_apex;
	float m_fovY = 0.f;
	float m_fovX = 0.f;
	float m_near = 0.001f;
	float m_far = 1,f;
	Matrix m_lookAt;
};

