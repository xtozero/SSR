#include "Matrix3X4.h"

XMMatrix Matrix3X4::Inverse() const
{
    return XMMatrix( *this ).Inverse();
}

XMVector Matrix3X4::Transform( const Vector& v ) const
{
    return XMVector3Transform( XMVector( v ), XMMatrix( *this ) );
}

XMVector Matrix3X4::TransformPosition( const Vector& v ) const
{
    return XMVector3TransformCoord( XMVector( v ), XMMatrix( *this ) );
}

XMVector Matrix3X4::TransformVector( const Vector& v ) const
{
    return XMVector3TransformNormal( XMVector( v ), XMMatrix( *this ) );
}

Matrix3X4::Matrix3X4( const XMMatrix& m )
{
    XMStoreFloat3x4( this, m );
}

XMMatrix operator*( const Matrix3X4& lhs, const Matrix3X4& rhs )
{
    return XMMatrix( lhs ) * XMMatrix( rhs );
}

XMMatrix operator*( const XMMatrix& lhs, const Matrix3X4& rhs )
{
    return lhs * XMMatrix( rhs );
}

XMMatrix operator*( const Matrix3X4& lhs, const XMMatrix& rhs )
{
    return XMMatrix( lhs ) * rhs;
}

XMMatrix& operator*=( XMMatrix& lhs, const Matrix3X4& rhs )
{
    lhs = lhs * rhs;
    return lhs;
}

Matrix3X4& operator*=( Matrix3X4& lhs, const XMMatrix& rhs )
{
    lhs = lhs * rhs;
    return lhs;
}
