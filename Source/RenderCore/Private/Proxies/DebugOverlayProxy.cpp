#include "DebugOverlayProxy.h"

#include "SceneRenderer.h"

namespace rendercore
{
	DebugOverlayProxy::DebugOverlayProxy( const logic::DebugOverlayComponent& component )
		: m_debugLine( component.GetDebugLine() )
		, m_debugTriangle( component.GetDebugTriangle() )
	{
		m_castShadow = false;
	}

	void DebugOverlayProxy::GatherDynamicMeshDrawInfo( RenderViewInfo& viewInfo ) const
	{
		for ( const logic::DebugLine& debugLine : m_debugLine )
		{
			viewInfo.m_debugOverlayData.AddLine( debugLine.m_from, debugLine.m_to, debugLine.m_color );
		}

		for ( const logic::DebugTriangle& debugTriangle : m_debugTriangle )
		{
			viewInfo.m_debugOverlayData.AddTriangle( debugTriangle.m_vertices[0]
				, debugTriangle.m_vertices[1]
				, debugTriangle.m_vertices[2]
				, debugTriangle.m_color );
		}
	}
}
