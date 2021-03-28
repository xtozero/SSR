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

	ShaderStates m_shaders;
	ShaderBindings m_shaderBindings;

	// Pipeline State
	GraphicsPipelineState m_pipelineState;

	// For Draw method
	UINT m_indexCount;
	UINT m_startIndexLocation;
	UINT m_baseVertexLocation;

	DrawSnapshot( ) = default;
	DrawSnapshot( const DrawSnapshot& other )
	{
		( *this ) = other;
	}
	DrawSnapshot& operator=( const DrawSnapshot& other ) noexcept
	{
		if ( this != &other )
		{
			m_vertexStream = other.m_vertexStream;
			m_indexBuffer = other.m_indexBuffer;
			m_shaders = other.m_shaders;
			m_shaderBindings = other.m_shaderBindings;
			m_pipelineState = other.m_pipelineState;
			m_indexCount = other.m_indexCount;
			m_startIndexLocation = other.m_startIndexLocation;
			m_baseVertexLocation = other.m_baseVertexLocation;
		}

		return *this;
	}
	DrawSnapshot( DrawSnapshot&& other ) noexcept
	{
		( *this ) = std::move( other );
	}
	DrawSnapshot& operator=( DrawSnapshot&& other ) noexcept
	{
		if ( this != &other )
		{
			m_vertexStream = std::move( other.m_vertexStream );
			m_indexBuffer = std::move( other.m_indexBuffer );
			m_shaders = std::move( other.m_shaders );
			m_shaderBindings = std::move( other.m_shaderBindings );
			m_pipelineState = std::move( other.m_pipelineState );
			m_indexCount = other.m_indexCount;
			m_startIndexLocation = other.m_startIndexLocation;
			m_baseVertexLocation = other.m_baseVertexLocation;
		}

		return *this;
	}
};