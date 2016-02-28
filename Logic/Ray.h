#pragma once

#include <d3dX9math.h>

class CRay
{
public:
	const D3DXVECTOR3& GetOrigin( ) const { return m_origin; }
	const D3DXVECTOR3& GetDir( ) const { return m_dir; }

	void SetOrigin( const D3DXVECTOR3& origin ) { m_origin = origin; }
	void SetDir( const D3DXVECTOR3& dir ) { D3DXVec3Normalize( &m_dir, &dir ); }

	CRay( const D3DXVECTOR3& origin, const D3DXVECTOR3& dir );
	CRay( );
	~CRay( );

private:
	D3DXVECTOR3 m_origin;
	D3DXVECTOR3 m_dir;
};

