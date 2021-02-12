#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "Math/CXMFloat.h"

#include <vector>

namespace rendercore
{
	class Viewport;
}

class IScene;

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
};

class RenderViewGroup
{
public:
	RENDERCORE_DLL RenderViewGroup( IScene& scene, rendercore::Viewport& viewport ) : m_scene( scene ), m_viewport( viewport )
	{}

	RENDERCORE_DLL RenderView& AddRenderView( );
	const RenderView& operator[]( std::size_t index ) const
	{
		return m_viewGroup[index];
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

	IScene& Scene() { return m_scene; }
	const IScene& Scene( ) const { return m_scene; }
	rendercore::Viewport& Viewport( ) { return m_viewport; }
	const rendercore::Viewport& Viewport( ) const { return m_viewport; }

private:
	std::vector<RenderView> m_viewGroup;
	IScene& m_scene;
	rendercore::Viewport& m_viewport;
};
