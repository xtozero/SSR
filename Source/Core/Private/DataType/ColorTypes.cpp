#include "ColorTypes.h"

#include <cassert>

ColorF::ColorF( float r, float g, float b, float a )
	: m_elem{ r, g, b, a }
{}

ColorF::ColorF()
	: m_elem{ 0.f, 0.f, 0.f, 0.f }
{
}

float& ColorF::operator[]( size_t idx )
{
	return m_rgba[idx];
}

float ColorF::operator[]( size_t idx ) const
{
	return m_rgba[idx];
}

float& ColorF::R()
{
	return m_elem.m_r;
}

float ColorF::R() const
{
	return m_elem.m_r;
}

float& ColorF::G()
{
	return m_elem.m_g;
}

float ColorF::G() const
{
	return m_elem.m_g;
}

float& ColorF::B()
{
	return m_elem.m_b;
}

float ColorF::B() const
{
	return m_elem.m_b;
}

float& ColorF::A()
{
	return m_elem.m_a;
}

float ColorF::A() const
{
	return m_elem.m_a;
}

const ColorF ColorF::Black( 0.f, 0.f, 0.f, 1.f );
const ColorF ColorF::White( 1.f, 1.f, 1.f, 1.f );

uint8& Color::operator[]( size_t idx )
{
	switch ( idx )
	{
	case 0:
		return m_elem.m_r;
	case 1:
		return m_elem.m_g;
	case 2:
		return m_elem.m_b;
	case 3:
		return m_elem.m_a;
	default:
		break;
	}

	assert( false && "Invalid color index" );
	return m_elem.m_r;
}

uint8 Color::operator[]( size_t idx ) const
{
	switch ( idx )
	{
	case 0:
		return m_elem.m_r;
	case 1:
		return m_elem.m_g;
	case 2:
		return m_elem.m_b;
	case 3:
		return m_elem.m_a;
	default:
		break;
	}

	assert( false && "Invalid color index" );
	return m_elem.m_r;
}

uint8& Color::R()
{
	return m_elem.m_r;
}

uint8 Color::R() const
{
	return m_elem.m_r;
}

uint8& Color::G()
{
	return m_elem.m_g;
}

uint8 Color::G() const
{
	return m_elem.m_g;
}

uint8& Color::B()
{
	return m_elem.m_b;
}

uint8 Color::B() const
{
	return m_elem.m_b;
}

uint8& Color::A()
{
	return m_elem.m_a;
}

uint8 Color::A() const
{
	return m_elem.m_a;
}

uint32& Color::DWORD()
{
	return m_rgba;
}

uint32 Color::DWORD() const
{
	return m_rgba;
}

ColorF Color::ToColorF() const
{
	return ColorF( m_elem.m_r / 255.f, m_elem.m_g / 255.f, m_elem.m_b / 255.f, m_elem.m_a / 255.f );
}

bool operator==( const Color& lhs, const Color& rhs )
{
	return lhs.m_rgba == rhs.m_rgba;
}

bool operator!=( const Color& lhs, const Color& rhs )
{
	return !( lhs == rhs );
}

const Color Color::Black( 0, 0, 0, 255 );
const Color Color::White( 255, 255, 255, 255 );
