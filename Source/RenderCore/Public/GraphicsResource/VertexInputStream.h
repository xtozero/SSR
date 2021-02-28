#pragma once

#include "GraphicsApiResource.h"
#include "VertexBuffer.h"

struct VertexInput
{
	VertexBuffer m_vertexBuffer;
};

class VertexInputStream
{
public:
	void Bind( VertexBuffer vertexBuffer, int slot )
	{
		m_vertexInputs[slot].m_vertexBuffer = vertexBuffer;
	}

	static constexpr int MAX_VERTEX_SLOT = 32;
	VertexInput m_vertexInputs[MAX_VERTEX_SLOT];
};

