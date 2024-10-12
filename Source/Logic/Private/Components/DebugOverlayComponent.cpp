#include "Components/DebugOverlayComponent.h"

#include "Proxies/DebugOverlayProxy.h"

#include <algorithm>

namespace logic
{
	DebugOverlayComponent::DebugOverlayComponent( CGameObject* pOwner, const char* name )
		: Super( pOwner, name )
	{
		m_think.m_canEverTick = true;
	}

	void DebugOverlayComponent::ThinkComponent( float elapsedTime )
	{
		int32 numRemoveLine = 0;
		for ( auto iter = std::begin( m_debugLine ), end = std::end( m_debugLine ); iter != end; )
		{
			if ( iter->m_life <= 0.f )
			{
				std::iter_swap( iter, end - 1 );
				--end;
				++numRemoveLine;
			}
			else
			{
				iter->m_life -= elapsedTime;
				++iter;
			}
		}

		if ( numRemoveLine > 0 )
		{
			auto iter = std::end( m_debugLine );
			std::advance( iter, -numRemoveLine );

			m_debugLine.erase( iter, std::end( m_debugLine ) );
		}

		int numRemoveTriangle = 0;
		for ( auto iter = std::begin( m_debugTriangle ), end = std::end( m_debugTriangle ); iter != end; )
		{
			if ( iter->m_life <= 0.f )
			{
				std::iter_swap( iter, end - 1 );
				--end;
				++numRemoveTriangle;
			}
			else
			{
				iter->m_life -= elapsedTime;
				++iter;
			}
		}

		if ( numRemoveTriangle > 0 )
		{
			auto iter = std::end( m_debugTriangle );
			std::advance( iter, -numRemoveTriangle );

			m_debugTriangle.erase( iter, std::end( m_debugTriangle ) );
		}

		if ( ( numRemoveLine > 0 ) || ( numRemoveTriangle > 0 ) )
		{
			MarkRenderStateDirty();
		}
	}

	rendercore::PrimitiveProxy* DebugOverlayComponent::CreateProxy() const
	{
		if ( m_debugLine.empty() && m_debugTriangle.empty() )
		{
			return nullptr;
		}

		return new rendercore::DebugOverlayProxy( *this );
	}

	BodySetup* DebugOverlayComponent::GetBodySetup()
	{
		return nullptr;
	}

	void DebugOverlayComponent::AddDebugLine( const Point& from, const Point& to, const ColorF& color, float life )
	{
		m_debugLine.emplace_back( from, to, color, life );
		MarkRenderStateDirty();
	}

	void DebugOverlayComponent::AddDebugTriangle( const Point& p0, const Point& p1, const Point& p2, const ColorF& color, float life )
	{
		m_debugTriangle.emplace_back( p0, p1, p2, color, life );
		MarkRenderStateDirty();
	}

	void DebugOverlayComponent::AddDebugCube( const Point& min, const Point& max, const ColorF& color, float life )
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

	void DebugOverlayComponent::AddDebugCube( const Vector& halfSize, const Matrix& transform, const ColorF& color, float life )
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

	void DebugOverlayComponent::AddDebugSolidCube( const Point& min, const Point& max, const ColorF& color, float life )
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

	void DebugOverlayComponent::AddDebugSphere( const Point& center, float radius, const ColorF& color, float life )
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

	bool DebugOverlayComponent::ShouldCreatePhysicsState() const
	{
		return false;
	}
}