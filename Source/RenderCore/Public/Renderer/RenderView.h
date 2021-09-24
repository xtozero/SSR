#pragma once

#include "common.h"
#include "DrawSnapshot.h"
#include "GraphicsApiResource.h"
#include "Math/CXMFloat.h"
#include "SizedTypes.h"

#include <vector>

namespace rendercore
{
	class Viewport;
}

class IScene;

struct ForwardLightingResource;

struct RenderView
{
	// for view
	CXMFLOAT3 m_viewOrigin = {};
	CXMFLOAT3X3 m_viewAxis = {};

	// for projection
	float m_nearPlaneDistance = 0.f;
	float m_farPlaneDistance = 0.f;
	float m_aspect = 0.f;
	float m_fov = 0.f;

	std::vector<VisibleDrawSnapshot> m_snapshots;

	ForwardLightingResource* m_forwardLighting = nullptr;
};

class RenderViewGroup
{
public:
	RENDERCORE_DLL RenderViewGroup( IScene& scene, rendercore::Viewport& viewport ) : m_scene( scene ), m_viewport( viewport )
	{}

	RENDERCORE_DLL RenderView& AddRenderView( );

	size_t Size( ) const
	{
		return m_viewGroup.size( );
	}

	RenderView* begin( )
	{
		return m_viewGroup.size( ) > 0 ? &m_viewGroup[0] : nullptr;
	}

	RenderView* end( )
	{
		RenderView* endIter = nullptr;
		if ( m_viewGroup.size( ) > 0 )
		{
			endIter = &m_viewGroup.back( );
			++endIter;
		}

		return endIter;
	}

	IScene& Scene( ) { return m_scene; }
	const IScene& Scene( ) const { return m_scene; }
	rendercore::Viewport& Viewport( ) { return m_viewport; }
	const rendercore::Viewport& Viewport( ) const { return m_viewport; }

	const RenderView& operator[]( size_t index ) const
	{
		return m_viewGroup[index];
	}

	RenderView& operator[]( size_t index )
	{
		return m_viewGroup[index];
	}

private:
	std::vector<RenderView> m_viewGroup;
	IScene& m_scene;
	rendercore::Viewport& m_viewport;
};
