#pragma once

#include "GraphicsApiResource.h"
#include "Math/CXMFloat.h"

#include <vector>

class IRenderer;

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
	RenderView& AddRenderView( )
	{
		m_viewGroup.emplace_back( );
		return m_viewGroup.back( );
	}

	std::vector<RenderView> m_viewGroup;
};
