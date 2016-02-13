#include "stdafx.h"
#include "Ray.h"

CRay::CRay( const D3DXVECTOR3& origin, const D3DXVECTOR3& dir ) :
m_origin( origin )
{
	D3DXVec3Normalize( &m_dir, &dir );
}

CRay::CRay( ) :
m_origin( 0.f, 0.f, 0.f ),
m_dir( 0.f, 0.f, 0.f )
{
}


CRay::~CRay( )
{
}
