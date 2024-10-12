#pragma once

class ColorF;
class World;

struct Vector;

namespace logic
{
	void DrawDebugLine( World& world, const Point& from, const Point& to, const ColorF& color, float life );
	void DrawDebugTriangle( World& world, const Point& p0, const Point& p1, const Point& p2, const ColorF& color, float life );
	void DrawDebugCube( World& world, const Point& min, const Point& max, const ColorF& color, float life );
	void DrawDebugSolidCube( World& world, const Point& min, const Point& max, const ColorF& color, float life );
	void DrawDebugSphere( World& world, const Point& center, float radius, const ColorF& color, float life );
}