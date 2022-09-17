#pragma once

#include "SizedTypes.h"
#include "Math/Vector.h"
#include "Math/XMMatrix.h"
#include "Math/XMVector.h"

#include <cassert>
#include <DirectXMath.h>

struct Matrix3X3 : public DirectX::XMFLOAT3X3
{
public:
	XMMatrix GetTrasposed() const;
	XMMatrix Inverse() const;
	bool IsNaN() const;

	XMVector Transform( const Vector& v ) const;
	void Transform( Vector* dest, Vector* src, uint32 size ) const;

	using ::DirectX::XMFLOAT3X3::XMFLOAT3X3;
	Matrix3X3( const XMMatrix& m );
	Matrix3X3() : DirectX::XMFLOAT3X3() {}
	Matrix3X3( const Vector& row1, const Vector& row2, const Vector& row3 );

	Vector operator[]( size_t index ) const
	{
		assert( index < 3 );
		return Vector( m[index] );
	}

	friend XMMatrix operator+( const Matrix3X3& lhs, const Matrix3X3& rhs );
	friend XMMatrix operator+( const XMMatrix& lhs, const Matrix3X3& rhs );
	friend XMMatrix operator+( const Matrix3X3& lhs, const XMMatrix& rhs );

	friend XMMatrix operator*( const Matrix3X3& lhs, const Matrix3X3& rhs );
	friend XMMatrix operator*( const XMMatrix& lhs, const Matrix3X3& rhs );
	friend XMMatrix operator*( const Matrix3X3& lhs, const XMMatrix& rhs );

	friend XMMatrix& operator+=( XMMatrix& lhs, const Matrix3X3& rhs );
	friend Matrix3X3& operator+=( Matrix3X3& lhs, const XMMatrix& rhs );

	friend XMMatrix& operator*=( XMMatrix& lhs, const Matrix3X3& rhs );
	friend Matrix3X3& operator*=( Matrix3X3& lhs, const XMMatrix& rhs );

	static const Matrix3X3 Identity;
};

using BasisVectorMatrix = Matrix3X3;