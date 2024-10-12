#pragma once

#include "Components/DebugOverlayComponent.h"
#include "GameObject.h"

namespace logic
{
	class DebugOverlay : public CGameObject
	{
		GENERATE_CLASS_TYPE_INFO( DebugOverlay )

	public:
		DebugOverlay()
		{
			m_debugOverlayComponent = CreateComponent<DebugOverlayComponent>( *this, "DebugOverlayComponent" );
			SetRootComponent( m_debugOverlayComponent );
		}

		void AddDebugLine( const Point& from, const Point& to, const ColorF& color, float life )
		{
			m_debugOverlayComponent->AddDebugLine( from, to, color, life );
		}

		void AddDebugTriangle( const Point& p0, const Point& p1, const Point& p2, const ColorF& color, float life )
		{
			m_debugOverlayComponent->AddDebugTriangle( p0, p1, p2, color, life );
		}

		void AddDebugCube( const Point& min, const Point& max, const ColorF& color, float life )
		{
			m_debugOverlayComponent->AddDebugCube( min, max, color, life );
		}

		void AddDebugCube( const Vector& halfSize, const Matrix& transform, const ColorF& color, float life )
		{
			m_debugOverlayComponent->AddDebugCube( halfSize, transform, color, life );
		}

		void AddDebugSolidCube( const Point& min, const Point& max, const ColorF& color, float life )
		{
			m_debugOverlayComponent->AddDebugSolidCube( min, max, color, life );
		}

		void AddDebugSphere( const Point& center, float radius, const ColorF& color, float life )
		{
			m_debugOverlayComponent->AddDebugSphere( center, radius, color, life );
		}

	private:
		DebugOverlayComponent* m_debugOverlayComponent = nullptr;
	};
}