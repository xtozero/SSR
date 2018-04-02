#pragma once

#include "Math/CXMFloat.h"
#include "Model/Surface.h"

namespace SHARED_CONSTANT_BUFFER
{
	enum
	{
		VS_GEOMETRY = 0,
		VS_SHADOW = 1,
		PS_SURFACE,
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
		GBUFFER,
	};
}

struct GeometryTransform
{
	CXMFLOAT4X4 m_world;
	CXMFLOAT4X4 m_invWorld;
};

struct ShadowTransform
{
	CXMFLOAT4X4 m_lightView;
	CXMFLOAT4X4 m_lightProjection;
};

struct ViewProjectionTrasform
{
	CXMFLOAT4X4 m_view;
	CXMFLOAT4X4 m_projection;
};

struct PASS_CONSTANT
{
	float m_zNear;
	float m_zFar;
	float m_elapsedTime;
	float m_totalTime;
	CXMFLOAT2 m_renderTargetSize;
	CXMFLOAT2 m_invRenderTargetSize;
};