#include "stdafx.h"
#include "IndexBuffer.h"

#include "MultiThread/EngineTaskScheduler.h"

aga::Buffer* IndexBuffer::Resource( )
{
	return m_buffer.Get( );
}

const aga::Buffer* IndexBuffer::Resource( ) const
{
	return m_buffer.Get( );
}

IndexBuffer::IndexBuffer( std::size_t numElement, const void* initData, bool isDWORD ) : m_numElement( numElement ), m_isDWORD( isDWORD )
{
	InitResource( initData );
}

void IndexBuffer::InitResource( const void* initData )
{
	BUFFER_TRAIT trait = {
		m_isDWORD ? sizeof( DWORD ) : sizeof( WORD ),
		static_cast<UINT>( m_numElement ),
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
		RESOURCE_BIND_TYPE::INDEX_BUFFER,
		0,
		RESOURCE_FORMAT::UNKNOWN
	};

	m_buffer = aga::Buffer::Create( trait, initData );
	EnqueueRenderTask( [buffer = m_buffer]( )
	{
		buffer->Init( );
	} );
}
