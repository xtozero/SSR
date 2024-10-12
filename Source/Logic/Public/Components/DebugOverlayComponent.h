#pragma once

#include "PrimitiveComponent.h"

#include <vector>

namespace logic
{
	struct BaseDebugPrimitive
	{
	public:
		BaseDebugPrimitive( const ColorF& color, float life )
			: m_color( color )
			, m_life( life )
		{}

		ColorF m_color;
		float m_life;
	};

	struct DebugLine : public BaseDebugPrimitive
	{
	public:
		DebugLine( const Point& from, const Point& to, const ColorF& color, float life )
			: BaseDebugPrimitive( color, life )
			, m_from( from )
			, m_to( to )
		{}

		Point m_from;
		Point m_to;
	};

	struct DebugTriangle : public BaseDebugPrimitive
	{
	public:
		DebugTriangle( const Point& p0, const Point& p1, const Point& p2, const ColorF& color, float life )
			: BaseDebugPrimitive( color, life )
			, m_vertices{ p0, p1, p2 }
		{}

		Point m_vertices[3];
	};

	class DebugOverlayComponent : public PrimitiveComponent
	{
		GENERATE_CLASS_TYPE_INFO( DebugOverlayComponent )

	public:
		DebugOverlayComponent( CGameObject* pOwner, const char* name );

		virtual void ThinkComponent( float elapsedTime );

		virtual rendercore::PrimitiveProxy* CreateProxy() const override;
		virtual BodySetup* GetBodySetup() override;

		const std::vector<DebugLine>& GetDebugLine() const
		{
			return m_debugLine;
		}

		const std::vector<DebugTriangle>& GetDebugTriangle() const
		{
			return m_debugTriangle;
		}

		void AddDebugLine( const Point& from, const Point& to, const ColorF& color, float life );
		void AddDebugTriangle( const Point& p0, const Point& p1, const Point& p2, const ColorF& color, float life );
		void AddDebugCube( const Point& min, const Point& max, const ColorF& color, float life );
		void AddDebugCube( const Vector& halfSize, const Matrix& transform, const ColorF& color, float life );
		void AddDebugSolidCube( const Point& min, const Point& max, const ColorF& color, float life );
		void AddDebugSphere( const Point& center, float radius, const ColorF& color, float life );

	protected:
		virtual bool ShouldCreatePhysicsState() const;

	private:
		std::vector<DebugLine> m_debugLine;
		std::vector<DebugTriangle> m_debugTriangle;
	};
}