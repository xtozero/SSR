#pragma once

#include "Math/Vector.h"

class Ray
{
public:
	const Vector& GetOrigin( ) const { return m_origin; }
	const Vector& GetDir( ) const { return m_dir; }

	void SetOrigin( const Vector& origin ) { m_origin = origin; }
	void SetDir( const Vector& dir ) { m_dir = dir.GetNormalized(); }

	Ray( const Vector& origin, const Vector& dir ) :
		m_origin( origin ), m_dir( dir.GetNormalized() ) {}
	Ray( ) = default;

private:
	Vector m_origin;
	Vector m_dir;
};

