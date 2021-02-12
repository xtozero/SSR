#include "stdafx.h"
#include "VertexBuffer.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"

VertexBuffer VertexBuffer::Create( std::size_t elementSize, std::size_t numElement, const void* initData )
{
	VertexBuffer vb;

	BUFFER_TRAIT trait = {
		static_cast<UINT>( elementSize ),
		static_cast<UINT>( numElement ),
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
		RESOURCE_BIND_TYPE::VERTEX_BUFFER
	};

	vb.m_buffer = GetInterface<IAga>( )->CreateBuffer( trait, initData );

	return vb;
}
