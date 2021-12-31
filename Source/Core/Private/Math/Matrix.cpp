#include "Matrix.h"

#include "Vector.h"

using namespace DirectX;

XMMatrix Matrix::GetTrasposed() const
{
	return XMMatrix( *this ).GetTrasposed();
}

XMMatrix Matrix::Inverse() const
{
	return XMMatrix( *this ).Inverse();
}

bool Matrix::IsNaN() const
{
	return XMMatrix( *this ).IsNaN();
}

XMVector Matrix::TransformPosition( const Vector& v ) const
{
	return XMVector3TransformCoord( XMVector( v ), XMMatrix( *this ) );
}

void Matrix::TransformPosition( Vector* dest, Vector* src, uint32 size ) const
{
	XMVector3TransformCoordStream( dest, sizeof( Vector ), src, sizeof( Vector ), size, XMMatrix( *this ) );
}

XMVector Matrix::TransformVector( const Vector& v ) const
{
	return XMVector3TransformNormal( XMVector( v ), XMMatrix( *this ) );
}

void Matrix::TransformVector( Vector* dest, Vector* src, uint32 size ) const
{
	XMVector3TransformNormalStream( dest, sizeof( Vector ), src, sizeof( Vector ), size, XMMatrix( *this ) );
}

Matrix::Matrix( const XMMatrix& m )
{
	XMStoreFloat4x4( this, m );
}

XMMatrix operator*( const Matrix& lhs, const Matrix& rhs )
{
	return XMMatrix( lhs ) * XMMatrix( rhs );
}

XMMatrix operator*( const XMMatrix& lhs, const Matrix& rhs )
{
	return lhs* XMMatrix( rhs );
}

XMMatrix operator*( const Matrix& lhs, const XMMatrix& rhs )
{
	return XMMatrix( lhs ) * rhs;
}

XMMatrix& operator*=( XMMatrix& lhs, const Matrix& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Matrix& operator*=( Matrix& lhs, const XMMatrix& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

const Matrix Matrix::Identity( 1.f, 0.f, 0.f, 0.f,
							0.f, 1.f, 0.f, 0.f,
							0.f, 0.f, 1.f, 0.f,
							0.f, 0.f, 0.f, 1.f );
