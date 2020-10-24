#include "stdafx.h"
#include "ConstantBuffer.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"

RefHandle<ConstantBuffer> ConstantBuffer::Create( std::size_t size )
{
	BUFFER_TRAIT trait = {
		static_cast<UINT>( size ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_BIND_TYPE::CONSTANT_BUFFER
	};

	return GetInterface<IAga>( )->CreateConstantBuffer( trait );
}
