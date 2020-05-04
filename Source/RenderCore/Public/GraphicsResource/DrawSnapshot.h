#pragma once

#include "GraphicsApiResource.h"
#include "ShaderBindings.h"
#include "VertexInputStream.h"

class DrawSnapshot
{
public:
	VertexInputStream m_vertexStream;
	RE_HANDLE m_indexBuffer;
	ShaderBindings m_shaderBindings;

	// Pipeline State
	GraphisPipelineState m_pipelineState;

	// For Draw method
	UINT m_indexCount;
	UINT m_startIndexLocation;
	UINT m_baseVertexLocation;
};