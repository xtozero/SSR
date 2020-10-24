#include "stdafx.h"
#include "VertexBuffer.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"

RefHandle<VertexBuffer> VertexBuffer::Create( std::size_t elementSize, std::size_t numElement, const void* initData )
{
	BUFFER_TRAIT trait = {
		static_cast<UINT>( elementSize ),
		static_cast<UINT>( numElement ),
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
		RESOURCE_BIND_TYPE::VERTEX_BUFFER
	};

	return GetInterface<IAga>( )->CreateVertexBuffer( trait, initData );
}
