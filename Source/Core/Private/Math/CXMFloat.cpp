#include "Math/CXMFloat.h"

CXMFLOAT3::CXMFLOAT3( const CXMFLOAT4& other )
{
	x = other.x;
	y = other.y;
	z = other.z;
}

CXMFLOAT4::CXMFLOAT4( const CXMFLOAT3& other )
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = 0;
}

CXMFLOAT3X3::CXMFLOAT3X3( const CXMFLOAT4X4& other )
{
	_11 = other._11; _12 = other._11; _13 = other._11;
	_21 = other._21; _22 = other._21; _23 = other._21;
	_31 = other._31; _32 = other._31; _33 = other._31;
}
