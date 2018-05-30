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

namespace PS_CONSTANT_BUFFER
{
	enum
	{
		LIGHT = 0,
		SURFACE,
		UTIL,
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
	CXMFLOAT4X4 m_lightViewProjection;
};

struct PassConstant
{
	float m_zNear;
	float m_zFar;
	float m_elapsedTime;
	float m_totalTime;
	CXMFLOAT2 m_renderTargetSize;
	CXMFLOAT2 m_invRenderTargetSize;
};