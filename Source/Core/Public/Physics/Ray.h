#pragma once

#include "Math/Vector.h"

class CRay
{
public:
	const Vector& GetOrigin( ) const { return m_origin; }
	const Vector& GetDir( ) const { return m_dir; }

	void SetOrigin( const Vector& origin ) { m_origin = origin; }
	void SetDir( const Vector& dir ) { m_dir = dir.GetNormalized(); }

	CRay( const Vector& origin, const Vector& dir ) :
		m_origin( origin ), m_dir( dir.GetNormalized() ) {}
	CRay( ) = default;

private:
	Vector m_origin;
	Vector m_dir;
};

