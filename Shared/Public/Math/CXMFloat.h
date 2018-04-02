#pragma once

#include <DirectXMath.h>

//------------------------------
// 2D Vector : simple wrapper struct for support operation
struct CXMFLOAT2 : public DirectX::XMFLOAT2
{
	CXMFLOAT2( ) : XMFLOAT2{} {}
	CXMFLOAT2( float x, float y ) : XMFLOAT2( x, y ) {}
	explicit CXMFLOAT2( const float *pArray ) : XMFLOAT2( pArray ) {}

	using XMFLOAT2::operator=;
	CXMFLOAT2( const DirectX::FXMVECTOR vec )
	{
		DirectX::XMStoreFloat2( this, vec );
	}

	// casting
	operator float*() { return &x; }
	operator const float*() { return &x; }
	operator DirectX::XMVECTOR( ) const { return DirectX::XMLoadFloat2( this ); }
	operator const DirectX::XMVECTOR( ) { return DirectX::XMLoadFloat2( this ); }
	operator const XMFLOAT2&() { return *this; }

	// binary operators
	DirectX::XMVECTOR operator+( const CXMFLOAT2& rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;
		XMVECTOR rVec = rhs;

		return lVec + rVec;
	}

	DirectX::XMVECTOR operator-( const CXMFLOAT2& rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;
		XMVECTOR rVec = rhs;

		return lVec - rVec;
	}

	DirectX::XMVECTOR operator*( float rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;

		return lVec * rhs;
	}

	friend DirectX::XMVECTOR operator*( float lhs, const CXMFLOAT2& rhs )
	{
		return rhs * lhs;
	}

	DirectX::XMVECTOR operator/( float rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;

		return lVec / rhs;
	}

	// assignment operators
	CXMFLOAT2& operator+=( const CXMFLOAT2& rhs )
	{
		*this = *this + rhs;
		return *this;
	}

	CXMFLOAT2& operator-=( const CXMFLOAT2& rhs )
	{
		*this = *this - rhs;
		return *this;
	}

	CXMFLOAT2& operator*=( float rhs )
	{
		*this = *this * rhs;
		return *this;
	}

	CXMFLOAT2& operator/=( float rhs )
	{
		*this = *this / rhs;
		return *this;
	}

	// unary operators
	CXMFLOAT2 operator+ ( ) const
	{
		return *this;
	}

	CXMFLOAT2 operator- ( ) const
	{
		return *this * -1.f;
	}

	bool operator==( const CXMFLOAT2& rhs ) const
	{
		return (x == rhs.x) && (y == rhs.y);
	}

	bool operator!=( const CXMFLOAT2& rhs ) const
	{
		return !(*this == rhs);
	}

	float* begin( )
	{
		return &x;
	}

	float* end( )
	{
		return &y + 1;
	}
};

//------------------------------
// 3D Vector : simple wrapper struct for support operation
struct CXMFLOAT3 : public DirectX::XMFLOAT3
{
	CXMFLOAT3( ) : XMFLOAT3{} {}
	CXMFLOAT3( float x, float y, float z ) : XMFLOAT3( x, y, z ) {}
	explicit CXMFLOAT3( const float *pArray ) : XMFLOAT3( pArray ) {}
	using XMFLOAT3::operator=;

	CXMFLOAT3( const DirectX::FXMVECTOR vec )
	{
		DirectX::XMStoreFloat3( this, vec );
	}

	// casting
	operator float*() { return &x; }
	operator const float*() { return &x; }
	operator DirectX::XMVECTOR( ) const { return DirectX::XMLoadFloat3( this ); }
	operator const DirectX::XMVECTOR( ) { return DirectX::XMLoadFloat3( this ); }
	operator const XMFLOAT3&( ) { return *this; }
	
	// binary operators
	DirectX::XMVECTOR operator+( const CXMFLOAT3& rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;
		XMVECTOR rVec = rhs;

		return lVec + rVec;
	}

	DirectX::XMVECTOR operator-( const CXMFLOAT3& rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;
		XMVECTOR rVec = rhs;

		return lVec - rVec;
	}

	DirectX::XMVECTOR operator*( float rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;

		return lVec * rhs;
	}

	friend DirectX::XMVECTOR operator*( float lhs, const CXMFLOAT3& rhs )
	{
		return rhs * lhs;
	}

	DirectX::XMVECTOR operator/( float rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;

		return lVec / rhs;
	}

	// assignment operators
	CXMFLOAT3& operator+=( const CXMFLOAT3& rhs )
	{
		*this = *this + rhs;
		return *this;
	}

	CXMFLOAT3& operator-=( const CXMFLOAT3& rhs )
	{
		*this = *this - rhs;
		return *this;
	}

	CXMFLOAT3& operator*=( float rhs )
	{
		*this = *this * rhs;
		return *this;
	}

	CXMFLOAT3& operator/=( float rhs )
	{
		*this = *this / rhs;
		return *this;
	}

	// unary operators
	CXMFLOAT3 operator+ ( ) const
	{
		return *this;
	}

	CXMFLOAT3 operator- ( ) const
	{
		return *this * -1.f;
	}

	bool operator==( const CXMFLOAT3& rhs ) const
	{
		return ( x == rhs.x ) && ( y == rhs.y ) && ( z == rhs.z );
	}

	bool operator!=( const CXMFLOAT3& rhs ) const
	{
		return !( *this == rhs );
	}

	float* begin( )
	{
		return &x;
	}

	float* end( )
	{
		return &z + 1;
	}
};

//------------------------------
// 4D Vector : simple wrapper struct for support operation
struct CXMFLOAT4 : public DirectX::XMFLOAT4
{
	CXMFLOAT4( ) : XMFLOAT4{} {}
	CXMFLOAT4( float x, float y, float z, float w ) : XMFLOAT4( x, y, z, w ) {}
	explicit CXMFLOAT4( const float *pArray ) : XMFLOAT4( pArray ) {}

	using XMFLOAT4::operator=;
	CXMFLOAT4( const DirectX::FXMVECTOR vec )
	{
		DirectX::XMStoreFloat4( this, vec );
	}

	// casting
	operator float*() { return &x; }
	operator const float*() { return &x; }
	operator DirectX::XMVECTOR( ) const { return DirectX::XMLoadFloat4( this ); }
	operator const DirectX::XMVECTOR( ) { return DirectX::XMLoadFloat4( this ); }
	operator const XMFLOAT4&() { return *this; }

	// binary operators
	DirectX::XMVECTOR operator+( const CXMFLOAT4& rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;
		XMVECTOR rVec = rhs;

		return lVec + rVec;
	}

	DirectX::XMVECTOR operator-( const CXMFLOAT4& rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;
		XMVECTOR rVec = rhs;

		return lVec - rVec;
	}

	DirectX::XMVECTOR operator*( float rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;

		return lVec * rhs;
	}

	friend DirectX::XMVECTOR operator*( float lhs, const CXMFLOAT4& rhs )
	{
		return rhs * lhs;
	}

	CXMFLOAT4 operator/( float rhs ) const
	{
		using namespace DirectX;

		XMVECTOR lVec = *this;

		return lVec / rhs;
	}

	// assignment operators
	CXMFLOAT4& operator+=( const CXMFLOAT4& rhs )
	{
		*this = *this + rhs;
		return *this;
	}

	CXMFLOAT4& operator-=( const CXMFLOAT4& rhs )
	{
		*this = *this - rhs;
		return *this;
	}

	CXMFLOAT4& operator*=( float rhs )
	{
		*this = *this * rhs;
		return *this;
	}

	CXMFLOAT4& operator/=( float rhs )
	{
		*this = *this / rhs;
		return *this;
	}

	// unary operators
	CXMFLOAT4 operator+ ( ) const
	{
		return *this;
	}

	CXMFLOAT4 operator- ( ) const
	{
		return *this * -1.f;
	}

	bool operator==( const CXMFLOAT4& rhs ) const
	{
		return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
	}

	bool operator!=( const CXMFLOAT4& rhs ) const
	{
		return !(*this == rhs);
	}

	float* begin( )
	{
		return &x;
	}

	float* end( )
	{
		return &w + 1;
	}
};

//------------------------------
// 3x3 Matrix : simple wrapper struct for support operation
struct CXMFLOAT3X3 : public DirectX::XMFLOAT3X3
{
	CXMFLOAT3X3( ) : XMFLOAT3X3{} {}
	CXMFLOAT3X3( float m00, float m01, float m02,
				float m10, float m11, float m12,
				float m20, float m21, float m22 ) : XMFLOAT3X3( m00, m01, m02, 
																m10, m11, m12,
																m20, m21, m22 ) {}
	explicit CXMFLOAT3X3( const float *pArray ) : XMFLOAT3X3( pArray ) {}

	using XMFLOAT3X3::operator=;

	CXMFLOAT3X3( const DirectX::CXMMATRIX mat )
	{
		DirectX::XMStoreFloat3x3( this, mat );
	}

	// casting
	operator DirectX::XMMATRIX( ) const { return XMLoadFloat3x3( this ); }
	operator const DirectX::XMMATRIX( ) { return XMLoadFloat3x3( this ); }
};

//------------------------------
// 4x3 Matrix : simple wrapper struct for support operation
struct CXMFLOAT4X3 : public DirectX::XMFLOAT4X3
{
	CXMFLOAT4X3( ) : XMFLOAT4X3{} {}
	CXMFLOAT4X3( float m00, float m01, float m02,
				float m10, float m11, float m12,
				float m20, float m21, float m22,
				float m30, float m31, float m32 ) : XMFLOAT4X3( m00, m01, m02, 
																m10, m11, m12, 
																m20, m21, m22, 
																m30, m31, m32 ) {}
	explicit CXMFLOAT4X3( const float *pArray ) : XMFLOAT4X3( pArray ) {}

	using XMFLOAT4X3::operator=;

	CXMFLOAT4X3( const DirectX::CXMMATRIX mat )
	{
		DirectX::XMStoreFloat4x3( this, mat );
	}

	// casting
	operator DirectX::XMMATRIX( ) const { return XMLoadFloat4x3( this ); }
	operator const DirectX::XMMATRIX( ) { return XMLoadFloat4x3( this ); }
};

//------------------------------
// 4x4 Matrix : simple wrapper struct for support operation
struct CXMFLOAT4X4 : public DirectX::XMFLOAT4X4
{
	CXMFLOAT4X4( ) : XMFLOAT4X4{} {}
	CXMFLOAT4X4( float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33 ) : XMFLOAT4X4( m00, m01, m02, m03,
																			m10, m11, m12, m13,
																			m20, m21, m22, m23,
																			m30, m31, m32, m33 ) {}
	explicit CXMFLOAT4X4( const float *pArray ) : XMFLOAT4X4( pArray ) {}

	using XMFLOAT4X4::operator=;

	CXMFLOAT4X4( const DirectX::CXMMATRIX mat )
	{
		DirectX::XMStoreFloat4x4( this, mat );
	}

	// casting
	operator DirectX::XMMATRIX( ) const { return XMLoadFloat4x4( this ); }
	operator const DirectX::XMMATRIX( ) { return XMLoadFloat4x4( this ); }
};
