#include "stdafx.h"
#include "Ray.h"

using namespace DirectX;

CRay::CRay( const CXMFLOAT3& origin, const CXMFLOAT3& dir ) : 
	m_origin( origin )
{
	m_dir = XMVector3Normalize( dir );
}
