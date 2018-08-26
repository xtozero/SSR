#pragma once

#include "Math/CXMFloat.h"
#include "Model/Surface.h"

namespace SHARED_CONSTANT_BUFFER
{
	enum
	{
		VS_GEOMETRY = 0,
		VS_VIEW_PROJECTION,
		PS_SURFACE,
		PS_UTIL,
		Count,
	};
}

namespace VS_CONSTANT_BUFFER
{
	enum
	{
		GEOMETRY = 0,
		VIEW_PROJECTION,
		SHADOW,
	};
}

namespace GS_CONSTANT_BUFFER
{
	enum
	{
		SHADOW = 0,
	};
}

namespace PS_CONSTANT_BUFFER
{
	enum
	{
		LIGHT = 0,
		SURFACE,
		UTIL,
		SHADOW,
	};
}

struct GeometryTransform
{
	CXMFLOAT4X4 m_world;
	CXMFLOAT4X4 m_invWorld;
};

struct ViewProjectionTrasform
{
	CXMFLOAT4X4 m_view;
	CXMFLOAT4X4 m_projection;
};

struct PassConstant
{
	float m_receiversNear;
	float m_receiversFar;
	float m_elapsedTime;
	float m_totalTime;
	CXMFLOAT2 m_renderTargetSize;
	CXMFLOAT2 m_invRenderTargetSize;
};