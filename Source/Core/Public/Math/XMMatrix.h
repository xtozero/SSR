#pragma once

#include <DirectXMath.h>

struct Matrix;
struct Matrix3X3;
struct XMVector;
struct Vector;

struct alignas( 16 ) XMMatrix : public DirectX::XMMATRIX
{
public:
	XMMatrix GetTrasposed() const;
	XMMatrix Inverse() const;
	bool IsNaN() const;

	XMVector TransformPosition( const XMVector& v ) const;
	XMVector TransformVector( const XMVector& v ) const;

	XMVector TransformPosition( const Vector& v ) const;
	XMVector TransformVector( const Vector& v ) const;

	explicit XMMatrix( const DirectX::XMMATRIX& matrix );
	XMMatrix( const Matrix& matrix );
	XMMatrix( const Matrix3X3& matrix );
	XMMatrix() : DirectX::XMMATRIX{} {}

	friend XMMatrix operator+( const XMMatrix& lhs, const XMMatrix& rhs );
	friend XMMatrix operator*( const XMMatrix& lhs, const XMMatrix& rhs );
};