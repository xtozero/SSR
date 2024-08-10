#if 0
#include "Scene/DebugOverlayManager.h"

#include "Core/GameLogic.h"
//#include "Render/Resource.h"

#include <cassert>

namespace logic
{
	bool CDebugOverlayManager::Init( CGameLogic& gameLogic )
	{
		//IRenderer& renderer = gameLogic.GetRenderer( );
		//IResourceManager& resourceMgr = renderer.GetResourceManager( );

		//constexpr uint32 defaultDynamicBufferSize = 64 * 1024;
		//m_dynamicVB.Initialize( resourceMgr, RESOURCE_BIND_TYPE::VERTEX_BUFFER, defaultDynamicBufferSize );

		//m_debugMaterial = renderer.SearchMaterial( "mat_debugOverlay" );
		//if ( m_debugMaterial == INVALID_MATERIAL )
		//{
		//	return false;
		//}

		return true;
	}

	void CDebugOverlayManager::DrawPrimitive( IRenderer& renderer, float deltaTime )
	{
		//int32 popCount = 0;

		//for ( auto iter = m_debugLine.begin( ), end = m_debugLine.end(); iter != end; )
		//{
		//	if ( iter->m_life <= 0.f )
		//	{
		//		std::iter_swap( iter, end - 1 );
		//		--end;
		//		++popCount;
		//	}
		//	else
		//	{
		//		iter->m_life -= deltaTime;
		//		++iter;
		//	}
		//}

		//while ( popCount-- > 0 )
		//{
		//	m_debugLine.pop_back( );
		//}

		//popCount = 0;
		//for ( auto iter = m_debugTriangle.begin( ), end = m_debugTriangle.end(); iter != end; )
		//{
		//	if ( iter->m_life <= 0.f )
		//	{
		//		std::iter_swap( iter, end - 1 );
		//		--end;
		//		++popCount;
		//	}
		//	else
		//	{
		//		iter->m_life -= deltaTime;
		//		++iter;
		//	}
		//}

		//while ( popCount-- > 0 )
		//{
		//	m_debugTriangle.pop_back( );
		//}

		//struct DebugPrimitiveVertex
		//{
		//	CXMFLOAT3 m_pos;
		//	uint32 m_color;
		//};

		//assert( ( m_debugLine.size( ) * 2 + m_debugTriangle.size( ) * 3 ) <= UINT_MAX );
		//DebugPrimitiveVertex* vertices = m_dynamicVB.Map<DebugPrimitiveVertex>( renderer, sizeof( DebugPrimitiveVertex ) * static_cast<uint32>( m_debugLine.size( ) * 2 + m_debugTriangle.size( ) * 3 ) );
		//if ( vertices == nullptr )
		//{
		//	__debugbreak( );
		//}

		//size_t idx = 0;
		//for ( const auto& line : m_debugLine )
		//{
		//	vertices[idx].m_pos = line.m_from;
		//	vertices[idx++].m_color = line.m_color;

		//	vertices[idx].m_pos = line.m_to;
		//	vertices[idx++].m_color = line.m_color;
		//}

		//for ( const auto& triangle : m_debugTriangle )
		//{
		//	vertices[idx].m_pos = triangle.m_vertices[0];
		//	vertices[idx++].m_color = triangle.m_color;

		//	vertices[idx].m_pos = triangle.m_vertices[1];
		//	vertices[idx++].m_color = triangle.m_color;

		//	vertices[idx].m_pos = triangle.m_vertices[2];
		//	vertices[idx++].m_color = triangle.m_color;
		//}

		//m_dynamicVB.Unmap( renderer );

		//RE_HANDLE handle = m_dynamicVB.GetHandle( );
		//uint32 stride = sizeof( DebugPrimitiveVertex );
		//uint32 offset = 0;
		//renderer.BindVertexBuffer( &handle, 0, 1, &stride, &offset );
		//renderer.BindMaterial( m_debugMaterial );

		//assert( ( m_debugLine.size( ) * 2 ) <= UINT_MAX );
		//uint32 debugLineSize = static_cast<uint32>( m_debugLine.size( ) * 2 );
		//renderer.Draw( RESOURCE_PRIMITIVE::LINELIST, debugLineSize );
		//assert( ( m_debugTriangle.size( ) * 3 ) <= UINT_MAX );
		//renderer.Draw( RESOURCE_PRIMITIVE::TRIANGLELIST, static_cast<uint32>( m_debugTriangle.size() * 3 ), debugLineSize );
	}

	void CDebugOverlayManager::AddDebugLine( const Point& from, const Point& to, uint32 color, float life )
	{
		m_debugLine.emplace_back( from, to, color, life );
	}

	void CDebugOverlayManager::AddDebugTriangle( const Point& p0, const Point& p1, const Point& p2, uint32 color, float life )
	{
		m_debugTriangle.emplace_back( p0, p1, p2, color, life );
	}

	void CDebugOverlayManager::AddDebugCube( const Point& min, const Point& max, uint32 color, float life )
	{
		Point from = min;
		Point to = min;
		to.x = max.x;
		AddDebugLine( from, to, color, life );
		from = Point( to.x, to.y, max.z );
		AddDebugLine( from, to, color, life );
		to = Point( min.x, from.y, from.z );
		AddDebugLine( from, to, color, life );
		from = Point( to.x, to.y, min.z );
		AddDebugLine( from, to, color, life );

		from = Point( min.x, max.y, min.z );
		to = Point( max.x, from.y, from.z );
		AddDebugLine( from, to, color, life );
		from = Point( to.x, to.y, max.z );
		AddDebugLine( from, to, color, life );
		to = Point( min.x, from.y, from.z );
		AddDebugLine( from, to, color, life );
		from = Point( to.x, to.y, min.z );
		AddDebugLine( from, to, color, life );

		from = min;
		to = Point( from.x, max.y, from.z );
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

	void CDebugOverlayManager::AddDebugCube( const Vector& halfSize, const Matrix& transform, uint32 color, float life )
	{
		Point vertex[8] = {
			Point( -halfSize.x, -halfSize.y, -halfSize.z ),
			Point( halfSize.x, -halfSize.y, -halfSize.z ),
			Point( halfSize.x, halfSize.y, -halfSize.z ),
			Point( -halfSize.x, halfSize.y, -halfSize.z ),
			Point( -halfSize.x, -halfSize.y, halfSize.z ),
			Point( halfSize.x, -halfSize.y, halfSize.z ),
			Point( halfSize.x, halfSize.y, halfSize.z ),
			Point( -halfSize.x, halfSize.y, halfSize.z )
		};

		for ( uint32 i = 0; i < 8; ++i )
		{
			vertex[i] = transform.TransformPosition( vertex[i] );
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

	void CDebugOverlayManager::AddDebugSolidCube( const Point& min, const Point& max, uint32 color, float life )
	{
		Point a = min;
		Point b( max.x, min.y, min.z );
		Point c( max.x, max.y, min.z );

		AddDebugTriangle( a, c, b, color, life );
		b.x = min.x;
		b.y = max.y;
		AddDebugTriangle( a, b, c, color, life );

		a.z = max.z;
		b = Point( max.x, min.y, max.z );
		c = Point( max.x, max.y, max.z );
		AddDebugTriangle( a, b, c, color, life );
		b.x = min.x;
		b.y = max.y;
		AddDebugTriangle( a, c, b, color, life );

		a = min;
		b = Point( max.x, min.y, min.z );
		c = Point( max.x, min.y, max.z );
		AddDebugTriangle( a, b, c, color, life );
		b.x = min.x;
		b.z = max.z;
		AddDebugTriangle( a, c, b, color, life );

		a.y = max.y;
		b = Point( max.x, max.y, min.z );
		c = max;
		AddDebugTriangle( a, c, b, color, life );
		b.x = min.x;
		b.z = max.z;
		AddDebugTriangle( a, b, c, color, life );

		a = min;
		b = Point( min.x, max.y, min.z );
		c = Point( min.x, max.y, max.z );
		AddDebugTriangle( a, c, b, color, life );
		b.y = min.y;
		b.z = max.z;
		AddDebugTriangle( a, b, c, color, life );

		a.x = max.x;
		b = Point( max.x, max.y, min.z );
		c = max;
		AddDebugTriangle( a, b, c, color, life );
		b.y = min.y;
		b.z = max.z;
		AddDebugTriangle( a, c, b, color, life );
	}

	void CDebugOverlayManager::AddDebugSphere( const Point& center, float radius, uint32 color, float life )
	{
		constexpr int32 COLS = 36;
		constexpr int32 ROWS = COLS >> 1;
		constexpr float STEP = ( DirectX::XM_PI / 180.0f ) * 360.0f / COLS;
		int32 p2 = COLS >> 1;
		int32 r2 = ROWS >> 1;
		float prev_ci = 1;
		float prev_si = 0;
		for ( int32 y = -r2; y < r2; ++y )
		{
			float cy = cos( y * STEP );
			float cy1 = cos( ( y + 1 ) * STEP );
			float sy = sin( y * STEP );
			float sy1 = sin( ( y + 1 ) * STEP );

			for ( int32 i = -p2; i < p2; ++i )
			{
				float ci = cos( i * STEP );
				float si = sin( i * STEP );
				AddDebugLine( Point( center.x + radius * ci * cy, center.y + radius * sy, center.z + radius * si * cy ),
					Point( center.x + radius * ci * cy1, center.y + radius * sy1, center.z + radius * si * cy1 ),
					color, life );
				AddDebugLine( Point( center.x + radius * ci * cy, center.y + radius * sy, center.z + radius * si * cy ),
					Point( center.x + radius * prev_ci * cy, center.y + radius * sy, center.z + radius * prev_si * cy ),
					color, life );
				AddDebugLine( Point( center.x + radius * prev_ci * cy1, center.y + radius * sy1, center.z + radius * prev_si * cy1 ),
					Point( center.x + radius * ci * cy1, center.y + radius * sy1, center.z + radius * si * cy1 ),
					color, life );
				prev_ci = ci;
				prev_si = si;
			}
		}
	}
}
#endif
