#include "stdafx.h"
#include "IndexBuffer.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"

RefHandle<IndexBuffer> IndexBuffer::Create( std::size_t numElement, const void* initData, bool isDWORD )
{
	BUFFER_TRAIT trait = {
		isDWORD ? 4u : 2u,
		static_cast<UINT>( numElement ),
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
		RESOURCE_BIND_TYPE::INDEX_BUFFER
	};

	return GetInterface<IAga>( )->CreateIndexBuffer( trait, initData );
}
