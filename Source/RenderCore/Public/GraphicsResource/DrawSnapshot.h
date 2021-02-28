#pragma once

#include "GraphicsApiResource.h"
#include "GraphicsPipelineState.h"
#include "IndexBuffer.h"
#include "ShaderBindings.h"
#include "VertexInputStream.h"

class DrawSnapshot
{
public:
	VertexInputStream m_vertexStream;
	IndexBuffer m_indexBuffer;
	ShaderBindings m_shaderBindings;

	// Pipeline State
	GraphicsPipelineState m_pipelineState;

	// For Draw method
	UINT m_indexCount;
	UINT m_startIndexLocation;
	UINT m_baseVertexLocation;
};