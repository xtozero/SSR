#include "Core/DebugDrawHelper.h"

#include "GameObject/DebugOverlay.h"
#include "World/World.h"

#include <cassert>

namespace logic
{
	DebugOverlay* GetDebugOverlay( World& world )
	{
		return static_cast<DebugOverlay*>( world.GetDebugOverlay() );
	}

	void DrawDebugLine( World& world, const Point& from, const Point& to, const ColorF& color, float life )
	{
		auto debugOverlay = GetDebugOverlay( world );
		assert( debugOverlay );

		debugOverlay->AddDebugLine( from, to, color, life );
	}

	void DrawDebugTriangle( World& world, const Point& p0, const Point& p1, const Point& p2, const ColorF& color, float life )
	{
		auto debugOverlay = GetDebugOverlay( world );
		assert( debugOverlay );

		debugOverlay->AddDebugTriangle( p0, p1, p2, color, life );
	}

	void DrawDebugCube( World& world, const Point& min, const Point& max, const ColorF& color, float life )
	{
		auto debugOverlay = GetDebugOverlay( world );
		assert( debugOverlay );

		debugOverlay->AddDebugCube( min, max, color, life );
	}

	void DrawDebugSolidCube( World& world, const Point& min, const Point& max, const ColorF& color, float life )
	{
		auto debugOverlay = GetDebugOverlay( world );
		assert( debugOverlay );

		debugOverlay->AddDebugSolidCube( min, max, color, life );
	}

	void DrawDebugSphere( World& world, const Point& center, float radius, const ColorF& color, float life )
	{
		auto debugOverlay = GetDebugOverlay( world );
		assert( debugOverlay );

		debugOverlay->AddDebugSphere( center, radius, color, life );
	}
}