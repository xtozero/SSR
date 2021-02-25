#include "stdafx.h"
#include "IndexBuffer.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"

IndexBuffer IndexBuffer::Create( std::size_t numElement, const void* initData, bool isDWORD )
{
	IndexBuffer ib;

	BUFFER_TRAIT trait = {
		isDWORD ? 4u : 2u,
		static_cast<UINT>( numElement ),
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
		RESOURCE_BIND_TYPE::INDEX_BUFFER
	};

	ib.m_buffer = GetInterface<IAga>( )->CreateBuffer( trait, initData );
	if ( IsInRenderThread( ) )
	{
		ib.m_buffer->Init( );
	}
	else
	{
		EnqueueRenderTask( [buffer = ib.m_buffer]( )
		{
			buffer->Init( );
		} );
	}

	return ib;
}
