#pragma once

#include "SizedTypes.h"
#include "Math/Vector4.h"
#include "Math/XMMatrix.h"
#include "Math/XMVector.h"

#include <DirectXMath.h>

struct Matrix : public DirectX::XMFLOAT4X4
{
public:
	XMMatrix GetTrasposed() const;
	XMMatrix Inverse() const;
	bool IsNaN() const;

	XMVector TransformPosition( const Vector& v ) const;
	void TransformPosition( Vector* dest, Vector* src, uint32 size ) const;
	XMVector TransformVector( const Vector& v ) const;
	void TransformVector( Vector* dest, Vector* src, uint32 size ) const;

	using DirectX::XMFLOAT4X4::XMFLOAT4X4;
	Matrix( const XMMatrix& m );
	Matrix() : DirectX::XMFLOAT4X4() {}

	Vector4 operator[]( size_t index ) const
	{
		assert( index < 4 );
		return Vector4( m[index]);
	}

	friend XMMatrix operator*( const Matrix& lhs, const Matrix& rhs );
	friend XMMatrix operator*( const XMMatrix& lhs, const Matrix& rhs );
	friend XMMatrix operator*( const Matrix& lhs, const XMMatrix& rhs );

	friend XMMatrix& operator*=( XMMatrix& lhs, const Matrix& rhs );
	friend Matrix& operator*=( Matrix& lhs, const XMMatrix& rhs );

	static const Matrix Identity;
};
