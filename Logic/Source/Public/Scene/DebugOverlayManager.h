#pragma once

#include "INotifyGraphicsDevice.h"
#include "Math/CXMFloat.h"
#include "Scene/DynamicBuffer.h"

#include <vector>

class CGameLogic;
class IRenderer;

inline constexpr unsigned int Convert2ARGB( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
	return a << 24 | r << 16 | g << 8 | b;
}

constexpr unsigned int g_colorChartreuse = Convert2ARGB( 127, 255, 0, 255 );

class BaseDebugPrimitive
{
public:
	unsigned int m_color;
	float m_life;

	BaseDebugPrimitive( unsigned int color, float life ) : m_color( color ), m_life( life ) {}
};

class DebugLine : public BaseDebugPrimitive
{
public:
	CXMFLOAT3 m_from;
	CXMFLOAT3 m_to;

	DebugLine( const CXMFLOAT3& from, const CXMFLOAT3& to, unsigned int color, float life ) :
		m_from( from ), m_to( to ), BaseDebugPrimitive( color, life ) {}
};

class DebugTriangle : public BaseDebugPrimitive
{
public:
	CXMFLOAT3 m_vertices[3];

	DebugTriangle( const CXMFLOAT3& p0, const CXMFLOAT3& p1, const CXMFLOAT3& p2, unsigned int color, float life ) :
		m_vertices{ p0, p1, p2 }, BaseDebugPrimitive( color, life ) {}
};

class CDebugOverlayManager : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;
	bool Init( CGameLogic& gameLogic );

	void DrawPrimitive( IRenderer& renderer, float deltaTime );

	void AddDebugLine( const CXMFLOAT3& from, const CXMFLOAT3& to, unsigned int color, float life );
	void AddDebugTriangle( const CXMFLOAT3& p0, const CXMFLOAT3& p1, const CXMFLOAT3& p2, unsigned int color, float life );
	void AddDebugCube( const CXMFLOAT3& min, const CXMFLOAT3& max, unsigned int color, float life );
	void AddDebugSolid( const CXMFLOAT3& min, const CXMFLOAT3& max, unsigned int color, float life );
	void AddDebugSphere( const CXMFLOAT3& center, float radius, unsigned int color, float life );

private:
	std::vector<DebugLine> m_debugLine;
	std::vector<DebugTriangle> m_debugTriangle;

	CDynamicBuffer m_dynamicVB;
	Material m_debugMaterial = INVALID_MATERIAL;
};