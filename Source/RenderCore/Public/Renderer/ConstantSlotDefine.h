#pragma once

#include "Math/CXMFloat.h"

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
		DEFAULT = 0,
		GEOMETRY,
		VIEW_PROJECTION,
		SHADOW,
	};
}

namespace GS_CONSTANT_BUFFER
{
	enum
	{
		DEFAULT = 0,
		SHADOW,
	};
}

namespace PS_CONSTANT_BUFFER
{
	enum
	{
		DEFAULT = 0,
		LIGHT,
		SURFACE,
		UTIL,
		SHADOW,
		ATMOSPHERIC,
	};
}

namespace CS_CONSTANT_BUFFER
{
	enum
	{
		DEFAULT = 0,
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
	CXMFLOAT4X4 m_invView;
	CXMFLOAT4X4 m_invProjection;
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