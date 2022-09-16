#include "XMMatrix.h"

#include "Matrix.h"
#include "Matrix3X3.h"

#include <memory>

XMMatrix XMMatrix::GetTrasposed() const
{
	return XMMatrix( XMMatrixTranspose( *this ) );
}

XMMatrix XMMatrix::Inverse() const
{
	return XMMatrix( XMMatrixInverse( nullptr, *this ) );
}

bool XMMatrix::IsNaN() const
{
	return XMMatrixIsNaN( *this );
}

XMVector XMMatrix::TransformPosition( const XMVector& v ) const
{
	return XMVector3TransformCoord( v, XMMatrix( *this ) );
}

XMVector XMMatrix::TransformVector( const XMVector& v ) const
{
	return XMVector3TransformNormal( v, XMMatrix( *this ) );
}

XMVector XMMatrix::TransformPosition( const Vector& v ) const
{
	return XMVector3TransformCoord( XMVector( v ), XMMatrix( *this ) );
}

XMVector XMMatrix::TransformVector( const Vector& v ) const
{
	return XMVector3TransformNormal( XMVector( v ), XMMatrix( *this ) );
}

XMMatrix::XMMatrix( const DirectX::XMMATRIX& matrix ) : DirectX::XMMATRIX( matrix )
{
}

XMMatrix::XMMatrix( const Matrix& matrix )
{
	std::construct_at( reinterpret_cast<XMMATRIX*>( this ), XMLoadFloat4x4( &matrix ) );
}

XMMatrix::XMMatrix( const Matrix3X3& matrix )
{
	std::construct_at( reinterpret_cast<XMMATRIX*>( this ), XMLoadFloat3x3( &matrix ) );
}

XMMatrix operator+( const XMMatrix& lhs, const XMMatrix& rhs )
{
	XMMatrix ret;

	for ( size_t i = 0; i < 3; ++i )
	{
		ret.r[i] = _mm_add_ps( lhs.r[i], rhs.r[i] );
	}

	return ret;
}

XMMatrix operator*( const XMMatrix& lhs, const XMMatrix& rhs )
{
	return XMMatrix( XMMatrixMultiply( lhs, rhs ) );
}