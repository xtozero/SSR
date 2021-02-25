#include "stdafx.h"
#include "UploadBuffer.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"

#include <cassert>

UploadBuffer UploadBuffer::Create( std::size_t elementSize, std::size_t numElement, const void* initData )
{
	UploadBuffer ub;
	
	if ( elementSize > 0 && numElement > 0 )
	{
		BUFFER_TRAIT trait = {
		static_cast<UINT>( elementSize ),
		static_cast<UINT>( numElement ),
		RESOURCE_ACCESS_FLAG::GPU_READ |
		RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_BIND_TYPE::SHADER_RESOURCE,
		RESOURCE_MISC::BUFFER_STRUCTURED
		};

		ub.m_buffer = GetInterface<IAga>( )->CreateBuffer( trait, initData );
		if ( IsInRenderThread( ) )
		{
			ub.m_buffer->Init( );
		}
		else
		{
			EnqueueRenderTask( [buffer = ub.m_buffer]( )
			{
				buffer->Init( );
			} );
		}
	}

	ub.m_elementSize = elementSize;
	ub.m_numElement = numElement;
	return ub;
}

void UploadBuffer::Resize( std::size_t numElement )
{
	if ( numElement > m_numElement )
	{
		UploadBuffer newBuffer = UploadBuffer::Create( m_elementSize, numElement, nullptr );
		( *this ) = std::move( newBuffer );
	}
}

void UploadBuffer::Unlock( )
{
	GetInterface<IAga>( )->UnLock( m_buffer );
}

void* UploadBuffer::LockImple( )
{
	return GetInterface<IAga>( )->Lock( m_buffer );
}
