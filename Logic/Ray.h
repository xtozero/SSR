#pragma once

#include "../shared/Math/CXMFloat.h"

class CRay
{
public:
	const CXMFLOAT3& GetOrigin( ) const { return m_origin; }
	const CXMFLOAT3& GetDir( ) const { return m_dir; }

	void SetOrigin( const CXMFLOAT3& origin ) { m_origin = origin; }
	void SetDir( const CXMFLOAT3& dir ) { m_dir = DirectX::XMVector3Normalize( dir ); }

	CRay( const CXMFLOAT3& origin, const CXMFLOAT3& dir );
	CRay( ) = default;

private:
	CXMFLOAT3 m_origin = { 0.f, 0.f, 0.f };
	CXMFLOAT3 m_dir = { 0.f, 0.f, 0.f };
};

