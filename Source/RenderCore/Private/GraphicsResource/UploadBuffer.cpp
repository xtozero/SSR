#include "stdafx.h"
#include "UploadBuffer.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"

#include <cassert>

void UploadBuffer::Resize( std::size_t numElement, const void* initData )
{
	bool increasedElements = numElement > m_numElement;

	if ( increasedElements )
	{
		m_numElement = numElement;
		InitResource( initData );
	}
}

void UploadBuffer::Unlock( )
{
	GetInterface<IAga>( )->UnLock( m_buffer );
}

aga::Buffer* UploadBuffer::Resource( )
{
	return m_buffer.Get( );
}

const aga::Buffer* UploadBuffer::Resource( ) const
{
	return m_buffer.Get( );
}

UploadBuffer::UploadBuffer( std::size_t elementSize, std::size_t numElement, const void* initData ) :
	m_elementSize( elementSize ), m_numElement( numElement )
{
	InitResource( initData );
}

void UploadBuffer::InitResource( const void* initData )
{
	if ( m_elementSize > 0 && m_numElement > 0 )
	{
		BUFFER_TRAIT trait = {
		static_cast<UINT>( m_elementSize ),
		static_cast<UINT>( m_numElement ),
		RESOURCE_ACCESS_FLAG::GPU_READ |
		RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_BIND_TYPE::SHADER_RESOURCE,
		RESOURCE_MISC::BUFFER_STRUCTURED
		};

		m_buffer = aga::Buffer::Create( trait, initData );
		if ( IsInRenderThread( ) )
		{
			m_buffer->Init( );
		}
		else
		{
			EnqueueRenderTask( [buffer = m_buffer]( )
			{
				buffer->Init( );
			} );
		}
	}
}

void* UploadBuffer::LockImple( )
{
	return GetInterface<IAga>( )->Lock( m_buffer );
}
