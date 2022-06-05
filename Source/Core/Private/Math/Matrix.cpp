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

XMVector Matrix::ExtractScaling( float tolerance )
{
	Vector scale3D;

	float squareSum0 = m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2];
	float squareSum1 = m[1][0] * m[1][0] + m[1][1] * m[1][1] + m[1][2] * m[1][2];
	float squareSum2 = m[2][0] * m[2][0] + m[2][1] * m[2][1] + m[2][2] * m[2][2];

	if ( squareSum0 > tolerance )
	{
		float scale0 = sqrtf( squareSum0 );
		scale3D[0] = scale0;
		float invScale0 = 1.f / scale0;
		m[0][0] *= invScale0;
		m[0][1] *= invScale0;
		m[0][2] *= invScale0;
	}
	else
	{
		scale3D[0] = 0.f;
	}

	if ( squareSum1 > tolerance )
	{
		float scale1 = sqrtf( squareSum1 );
		scale3D[1] = scale1;
		float invScale1 = 1.f / scale1;
		m[1][0] *= invScale1;
		m[1][1] *= invScale1;
		m[1][2] *= invScale1;
	}
	else
	{
		scale3D[1] = 0.f;
	}

	if ( squareSum2 > tolerance )
	{
		float scale2 = sqrtf( squareSum2 );
		scale3D[2] = scale2;
		float invScale2 = 1.f / scale2;
		m[2][0] *= invScale2;
		m[2][1] *= invScale2;
		m[2][2] *= invScale2;
	}
	else
	{
		scale3D[2] = 0.f;
	}

	return scale3D;
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
