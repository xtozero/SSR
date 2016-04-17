#pragma once

#include <array>
#include <memory>

class IBuffer;

enum MAT_CONSTANT_BUFFER
{
	SURFACE = 0,
	TOTAL_COUNT
};

typedef std::array<std::shared_ptr<IBuffer>, MAT_CONSTANT_BUFFER::TOTAL_COUNT> MatConstantBuffers;