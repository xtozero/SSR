#include "stdafx.h"
#include "Scene/DebugOverlayManager.h"

#include "Core/GameLogic.h"
#include "Render/Resource.h"

using namespace DirectX;

void CDebugOverlayManager::OnDeviceRestore( CGameLogic& gameLogic )
{
	m_dynamicVB.OnDeviceRestore( gameLogic );
	m_debugMaterial = gameLogic.GetRenderer( ).SearchMaterial( _T( "mat_debugOverlay" ) );
}

bool CDebugOverlayManager::Init( CGameLogic& gameLogic )
{
	IRenderer& renderer = gameLogic.GetRenderer( );
	IResourceManager& resourceMgr = renderer.GetResourceManager( );

	constexpr unsigned int defaultDynamicBufferSize = 64 * 1024;
	m_dynamicVB.Initialize( resourceMgr, RESOURCE_TYPE::VERTEX_BUFFER, defaultDynamicBufferSize );

	m_debugMaterial = renderer.SearchMaterial( _T( "mat_debugOverlay" ) );
	if ( m_debugMaterial == INVALID_MATERIAL )
	{
		return false;
	}
	
	return true;
}

void CDebugOverlayManager::DrawPrimitive( IRenderer& renderer, float deltaTime )
{
	int popCount = 0;

	for ( auto iter = m_debugLine.begin( ), end = m_debugLine.end(); iter != end; )
	{
		if ( iter->m_life <= 0.f )
		{
			std::iter_swap( iter, end - 1 );
			--end;
			++popCount;
		}
		else
		{
			iter->m_life -= deltaTime;
			++iter;
		}
	}

	while ( popCount-- > 0 )
	{
		m_debugLine.pop_back( );
	}

	popCount = 0;
	for ( auto iter = m_debugTriangle.begin( ), end = m_debugTriangle.end(); iter != end; )
	{
		if ( iter->m_life <= 0.f )
		{
			std::iter_swap( iter, end - 1 );
			--end;
			++popCount;
		}
		else
		{
			iter->m_life -= deltaTime;
			++iter;
		}
	}

	while ( popCount-- > 0 )
	{
		m_debugTriangle.pop_back( );
	}

	struct DebugPrimitiveVertex
	{
		CXMFLOAT3 m_pos;
		unsigned int m_color;
	};

	DebugPrimitiveVertex* vertices = m_dynamicVB.Map<DebugPrimitiveVertex>( renderer, sizeof( DebugPrimitiveVertex ) * ( m_debugLine.size( ) * 2 + m_debugTriangle.size( ) * 3 ) );
	if ( vertices == nullptr )
	{
		__debugbreak( );
	}

	int idx = 0;
	for ( const auto& line : m_debugLine )
	{
		vertices[idx].m_pos = line.m_from;
		vertices[idx++].m_color = line.m_color;

		vertices[idx].m_pos = line.m_to;
		vertices[idx++].m_color = line.m_color;
	}

	for ( const auto& triangle : m_debugTriangle )
	{
		vertices[idx].m_pos = triangle.m_vertices[0];
		vertices[idx++].m_color = triangle.m_color;

		vertices[idx].m_pos = triangle.m_vertices[1];
		vertices[idx++].m_color = triangle.m_color;

		vertices[idx].m_pos = triangle.m_vertices[2];
		vertices[idx++].m_color = triangle.m_color;
	}

	m_dynamicVB.Unmap( renderer );

	RE_HANDLE handle = m_dynamicVB.GetHandle( );
	UINT stride = sizeof( DebugPrimitiveVertex );
	UINT offset = 0;
	renderer.BindVertexBuffer( &handle, 0, 1, &stride, &offset );
	renderer.BindMaterial( m_debugMaterial );

	size_t debugLineSize = m_debugLine.size( ) * 2;
	renderer.Draw( RESOURCE_PRIMITIVE::LINELIST, debugLineSize );
	renderer.Draw( RESOURCE_PRIMITIVE::TRIANGLELIST, m_debugTriangle.size() * 3, debugLineSize );
}

void CDebugOverlayManager::AddDebugLine( const CXMFLOAT3& from, const CXMFLOAT3& to, unsigned int color, float life )
{
	m_debugLine.emplace_back( from, to, color, life );
}

void CDebugOverlayManager::AddDebugTriangle( const CXMFLOAT3& p0, const CXMFLOAT3& p1, const CXMFLOAT3& p2, unsigned int color, float life )
{
	m_debugTriangle.emplace_back( p0, p1, p2, color, life );
}

void CDebugOverlayManager::AddDebugCube( const CXMFLOAT3& min, const CXMFLOAT3& max, unsigned int color, float life )
{
	CXMFLOAT3 from = min;
	CXMFLOAT3 to = min;
	to.x = max.x;
	AddDebugLine( from, to, color, life );
	from = CXMFLOAT3( to.x, to.y, max.z );
	AddDebugLine( from, to, color, life );
	to = CXMFLOAT3( min.x, from.y, from.z );
	AddDebugLine( from, to, color, life );
	from = CXMFLOAT3( to.x, to.y, min.z );
	AddDebugLine( from, to, color, life );

	from = CXMFLOAT3( min.x, max.y, min.z );
	to = CXMFLOAT3( max.x, from.y, from.z );
	AddDebugLine( from, to, color, life );
	from = CXMFLOAT3( to.x, to.y, max.z );
	AddDebugLine( from, to, color, life );
	to = CXMFLOAT3( min.x, from.y, from.z );
	AddDebugLine( from, to, color, life );
	from = CXMFLOAT3( to.x, to.y, min.z );
	AddDebugLine( from, to, color, life );

	from = min;
	to = CXMFLOAT3( from.x, max.y, from.z );
	AddDebugLine( from, to, color, life );
	from.x = max.x;
	to.x = max.x;
	AddDebugLine( from, to, color, life );
	from.z = max.z;
	to.z = max.z;
	AddDebugLine( from, to, color, life );
	from.x = min.x;
	to.x = min.x;
	AddDebugLine( from, to, color, life );
}

void CDebugOverlayManager::AddDebugCube( const CXMFLOAT3& halfSize, const CXMFLOAT4X4& transform, unsigned int color, float life )
{
	CXMFLOAT3 vertex[8] = {
		CXMFLOAT3( -halfSize.x, -halfSize.y, -halfSize.z ),
		CXMFLOAT3( halfSize.x, -halfSize.y, -halfSize.z ),
		CXMFLOAT3( halfSize.x, halfSize.y, -halfSize.z ),
		CXMFLOAT3( -halfSize.x, halfSize.y, -halfSize.z ),
		CXMFLOAT3( -halfSize.x, -halfSize.y, halfSize.z ),
		CXMFLOAT3( halfSize.x, -halfSize.y, halfSize.z ),
		CXMFLOAT3( halfSize.x, halfSize.y, halfSize.z ),
		CXMFLOAT3( -halfSize.x, halfSize.y, halfSize.z )
	};

	for ( int i = 0; i < 8; ++i )
	{
		vertex[i] = XMVector3TransformCoord( vertex[i], transform );
	}

	AddDebugLine( vertex[0], vertex[1], color, life );
	AddDebugLine( vertex[1], vertex[2], color, life );
	AddDebugLine( vertex[2], vertex[3], color, life );
	AddDebugLine( vertex[3], vertex[0], color, life );

	AddDebugLine( vertex[4], vertex[5], color, life );
	AddDebugLine( vertex[5], vertex[6], color, life );
	AddDebugLine( vertex[6], vertex[7], color, life );
	AddDebugLine( vertex[7], vertex[4], color, life );

	AddDebugLine( vertex[0], vertex[4], color, life );
	AddDebugLine( vertex[1], vertex[5], color, life );
	AddDebugLine( vertex[2], vertex[6], color, life );
	AddDebugLine( vertex[3], vertex[7], color, life );
}

void CDebugOverlayManager::AddDebugSolidCube( const CXMFLOAT3& min, const CXMFLOAT3& max, unsigned int color, float life )
{
	CXMFLOAT3 a = min;
	CXMFLOAT3 b( max.x, min.y, min.z );
	CXMFLOAT3 c( max.x, max.y, min.z );

	AddDebugTriangle( a, c, b, color, life );
	b.x = min.x;
	b.y = max.y;
	AddDebugTriangle( a, b, c, color, life );

	a.z = max.z;
	b = CXMFLOAT3( max.x, min.y, max.z );
	c = CXMFLOAT3( max.x, max.y, max.z );
	AddDebugTriangle( a, b, c, color, life );
	b.x = min.x;
	b.y = max.y;
	AddDebugTriangle( a, c, b, color, life );

	a = min;
	b = CXMFLOAT3( max.x, min.y, min.z );
	c = CXMFLOAT3( max.x, min.y, max.z );
	AddDebugTriangle( a, b, c, color, life );
	b.x = min.x;
	b.z = max.z;
	AddDebugTriangle( a, c, b, color, life );

	a.y = max.y;
	b = CXMFLOAT3( max.x, max.y, min.z );
	c = max;
	AddDebugTriangle( a, c, b, color, life );
	b.x = min.x;
	b.z = max.z;
	AddDebugTriangle( a, b, c, color, life );

	a = min;
	b = CXMFLOAT3( min.x, max.y, min.z );
	c = CXMFLOAT3( min.x, max.y, max.z );
	AddDebugTriangle( a, c, b, color, life );
	b.y = min.y;
	b.z = max.z;
	AddDebugTriangle( a, b, c, color, life );

	a.x = max.x;
	b = CXMFLOAT3( max.x, max.y, min.z );
	c = max;
	AddDebugTriangle( a, b, c, color, life );
	b.y = min.y;
	b.z = max.z;
	AddDebugTriangle( a, c, b, color, life );
}

void CDebugOverlayManager::AddDebugSphere( const CXMFLOAT3 & center, float radius, unsigned int color, float life )
{
	static const int COLS = 36;
	static const int ROWS = COLS >> 1;
	static const float STEP = ( DirectX::XM_PI / 180.0f ) * 360.0f / COLS;
	int p2 = COLS >> 1;
	int r2 = ROWS >> 1;
	float prev_ci = 1;
	float prev_si = 0;
	for ( int y = -r2; y < r2; ++y )
	{
		float cy = cos( y * STEP );
		float cy1 = cos( ( y + 1 ) * STEP );
		float sy = sin( y * STEP );
		float sy1 = sin( ( y + 1 ) * STEP );

		for ( int i = -p2; i < p2; ++i )
		{
			float ci = cos( i * STEP );
			float si = sin( i * STEP );
			AddDebugLine( CXMFLOAT3( center.x + radius * ci * cy, center.y + radius * sy, center.z + radius * si * cy ),
				CXMFLOAT3( center.x + radius * ci * cy1, center.y + radius * sy1, center.z + radius * si * cy1 ),
				color, life );
			AddDebugLine( CXMFLOAT3( center.x + radius * ci * cy, center.y + radius * sy, center.z + radius * si * cy ),
				CXMFLOAT3( center.x + radius * prev_ci * cy, center.y + radius * sy, center.z + radius * prev_si * cy ),
				color, life );
			AddDebugLine( CXMFLOAT3( center.x + radius * prev_ci * cy1, center.y + radius * sy1, center.z + radius * prev_si * cy1 ),
				CXMFLOAT3( center.x + radius * ci * cy1, center.y + radius * sy1, center.z + radius * si * cy1 ),
				color, life );
			prev_ci = ci;
			prev_si = si;
		}
	}
}
