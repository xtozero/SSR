#pragma once

#include "Model/Surface.h"

namespace COMMON_CONSTANT_BUFFER
{
	enum
	{
		PS_SURFACE = 0,
		Count,
	};
}

namespace VS_CONSTANT_BUFFER
{
	enum
	{
		WORLD = 0,
		VIEW_PROJECTION,
		LIGHT_VIEW_PROJECTION,
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