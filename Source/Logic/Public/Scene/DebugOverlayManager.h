#pragma once

#include "INotifyGraphicsDevice.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "SizedTypes.h"

#include <vector>

class CGameLogic;
class IRenderer;

inline constexpr uint32 Convert2ARGB( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
	return a << 24 | r << 16 | g << 8 | b;
}

constexpr uint32 g_colorChartreuse = Convert2ARGB( 127, 255, 0, 255 );
constexpr uint32 g_colorRed = Convert2ARGB( 255, 0, 0, 255 );

class BaseDebugPrimitive
{
public:
	uint32 m_color;
	float m_life;

	BaseDebugPrimitive( uint32 color, float life ) : m_color( color ), m_life( life ) {}
};

class DebugLine : public BaseDebugPrimitive
{
public:
	Point m_from;
	Point m_to;

	DebugLine( const Point& from, const Point& to, uint32 color, float life ) :
		m_from( from ), m_to( to ), BaseDebugPrimitive( color, life ) {}
};

class DebugTriangle : public BaseDebugPrimitive
{
public:
	Point m_vertices[3];

	DebugTriangle( const Point& p0, const Point& p1, const Point& p2, uint32 color, float life ) :
		m_vertices{ p0, p1, p2 }, BaseDebugPrimitive( color, life ) {}
};

class CDebugOverlayManager : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;
	bool Init( CGameLogic& gameLogic );

	void DrawPrimitive( IRenderer& renderer, float deltaTime );

	void AddDebugLine( const Point& from, const Point& to, uint32 color, float life );
	void AddDebugTriangle( const Point& p0, const Point& p1, const Point& p2, uint32 color, float life );
	void AddDebugCube( const Point& min, const Point& max, uint32 color, float life );
	void AddDebugCube( const Vector& halfSize, const Matrix& transform, uint32 color, float life );
	void AddDebugSolidCube( const Point& min, const Point& max, uint32 color, float life );
	void AddDebugSphere( const Point& center, float radius, uint32 color, float life );

private:
	std::vector<DebugLine> m_debugLine;
	std::vector<DebugTriangle> m_debugTriangle;

	//CDynamicBuffer m_dynamicVB;
	//Material m_debugMaterial = INVALID_MATERIAL;
};