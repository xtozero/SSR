#pragma once

#include "SizedTypes.h"

class ColorF
{
public:
	ColorF( float r, float g, float b, float a = 1.f );
	ColorF();

	float& operator[]( size_t idx );
	float operator[]( size_t idx ) const;

	float& R();
	float R() const;

	float& G();
	float G() const;

	float& B();
	float B() const;

	float& A();
	float A() const;

	static const ColorF Black;
	static const ColorF White;

private:
	union
	{
		struct
		{
			float m_r;
			float m_g;
			float m_b;
			float m_a;
		} m_elem;

		float m_rgba[4];
	};
};

class Color
{
public:
	Color( uint8 r, uint8 g, uint8 b, uint8 a = 255 );
	Color();

	uint8& operator[]( size_t idx );
	uint8 operator[]( size_t idx ) const;

	uint8& R();
	uint8 R() const;

	uint8& G();
	uint8 G() const;

	uint8& B();
	uint8 B() const;

	uint8& A();
	uint8 A() const;

	uint32& DWORD();
	uint32 DWORD() const;

	ColorF ToColorF() const;

	friend bool operator==( const Color& lhs, const Color& rhs );
	friend bool operator!=( const Color& lhs, const Color& rhs );

	static const Color Black;
	static const Color White;

private:
	union
	{
		struct
		{
			uint8 m_r;
			uint8 m_g;
			uint8 m_b;
			uint8 m_a;
		} m_elem;

		uint32 m_rgba;
	};
};