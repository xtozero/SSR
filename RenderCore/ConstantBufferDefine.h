#pragma once

enum class VS_CONSTANT_BUFFER
{
	WORLD = 0,
	VIEW_PROJECTION,
	LIGHT_VIEW_PROJECTION,
};

enum class PS_CONSTANT_BUFFER
{
	LIGHT = 0,
	SURFACE,
};