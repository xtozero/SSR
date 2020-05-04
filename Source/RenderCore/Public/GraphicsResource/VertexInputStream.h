#pragma once

#include "GraphicsApiResource.h"

struct VertexInput
{
	RE_HANDLE m_vertexBuffer;
};

struct VertexInputStream
{
	static constexpr int MAX_VERTEX_SLOT = 32;

	VertexInput m_vertexInputs[MAX_VERTEX_SLOT];
};

