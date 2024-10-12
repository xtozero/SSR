#pragma once

#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "SizedTypes.h"

#include <vector>

namespace rendercore
{
	class GlobalDynamicVertexBuffer;
	class RenderingShaderResource;

	struct DebugOverlayVertex
	{
		Vector m_position = Vector::ZeroVector;
		ColorF m_color = ColorF::Black;

		DebugOverlayVertex() = default;
		DebugOverlayVertex( const Vector& position, const ColorF& color );
	};

	static_assert( std::is_standard_layout_v<DebugOverlayVertex> );

	class DebugOverlayData
	{
	public:
		void Draw( GlobalDynamicVertexBuffer& dynamicVertexBuffer, RenderingShaderResource& resources );

		void AddLine( const Point& from, const Point& to, const ColorF& color );
		void AddTriangle( const Point& p0, const Point& p1, const Point& p2, const ColorF& color );

	private:
		std::vector<DebugOverlayVertex> m_debugLine;
		std::vector<DebugOverlayVertex> m_debugTriangle;
	};
}
