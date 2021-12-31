#include "Matrix3X3.h"

XMMatrix Matrix3X3::GetTrasposed() const
{
	return XMMatrix( *this ).GetTrasposed();
}

XMMatrix Matrix3X3::Inverse() const
{
	return XMMatrix( *this ).Inverse();
}

bool Matrix3X3::IsNaN() const
{
	return XMMatrix( *this ).IsNaN();
}

XMVector Matrix3X3::Transform( const Vector& v ) const
{
	return XMVector3TransformNormal( XMVector( v ), XMMatrix( *this ) );
}

void Matrix3X3::Transform( Vector* dest, Vector* src, uint32 size ) const
{
	XMVector3TransformNormalStream( dest, sizeof( Vector ), src, sizeof( Vector ), size, XMMatrix( *this ) );
}

Matrix3X3::Matrix3X3( const XMMatrix& m )
{
	XMStoreFloat3x3( this, m );
}

Matrix3X3::Matrix3X3( const Vector& row1, const Vector& row2, const Vector& row3 ) :
	XMFLOAT3X3( row1.x, row1.y, row1.z,
				row2.x, row2.y, row2.z,
				row3.x, row3.y, row3.z )
{
}

XMMatrix operator+( const Matrix3X3& lhs, const Matrix3X3& rhs )
{
	return XMMatrix( lhs ) + XMMatrix( rhs );
}

XMMatrix operator+( const XMMatrix& lhs, const Matrix3X3& rhs )
{
	return lhs + XMMatrix( rhs );
}

XMMatrix operator+( const Matrix3X3& lhs, const XMMatrix& rhs )
{
	return XMMatrix( lhs ) + rhs;
}

XMMatrix operator*( const Matrix3X3& lhs, const Matrix3X3& rhs )
{
	return XMMatrix( lhs ) * XMMatrix( rhs );
}

XMMatrix operator*( const XMMatrix& lhs, const Matrix3X3& rhs )
{
	return lhs * XMMatrix( rhs );
}

XMMatrix operator*( const Matrix3X3& lhs, const XMMatrix& rhs )
{
	return XMMatrix( lhs ) * rhs;
}

XMMatrix& operator+=( XMMatrix& lhs, const Matrix3X3& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Matrix3X3& operator+=( Matrix3X3& lhs, const XMMatrix& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

XMMatrix& operator*=( XMMatrix& lhs, const Matrix3X3& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Matrix3X3& operator*=( Matrix3X3& lhs, const XMMatrix& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

const Matrix3X3 Matrix3X3::Identity( 1.f, 0.f, 0.f,
									0.f, 1.f, 0.f,
									0.f, 0.f, 1.f );