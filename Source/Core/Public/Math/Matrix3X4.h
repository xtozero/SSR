#pragma once

#include "Math/XMMatrix.h"

#include <DirectXMath.h>

struct Matrix3X4 : public DirectX::XMFLOAT3X4
{
public:
    XMMatrix Inverse() const;

    XMVector Transform( const Vector& v ) const;
    XMVector TransformPosition( const Vector& v ) const;
    XMVector TransformVector( const Vector& v ) const;

    using ::DirectX::XMFLOAT3X4::XMFLOAT3X4;
    Matrix3X4( const XMMatrix& m );
    Matrix3X4() : DirectX::XMFLOAT3X4() {}

    friend XMMatrix operator*( const Matrix3X4& lhs, const Matrix3X4& rhs );
    friend XMMatrix operator*( const XMMatrix& lhs, const Matrix3X4& rhs );
    friend XMMatrix operator*( const Matrix3X4& lhs, const XMMatrix& rhs );

    friend XMMatrix& operator*=( XMMatrix& lhs, const Matrix3X4& rhs );
    friend Matrix3X4& operator*=( Matrix3X4& lhs, const XMMatrix& rhs );
};