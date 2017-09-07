#pragma once

#include <array>
#include <memory>

class IBuffer;

enum MAT_CONSTANT_BUFFER
{
	SURFACE = 0,
	TOTAL_COUNT
};

using MatConstantBuffers = std::array<IBuffer*, MAT_CONSTANT_BUFFER::TOTAL_COUNT>;