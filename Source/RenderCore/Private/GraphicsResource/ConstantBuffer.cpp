#include "stdafx.h"
#include "ConstantBuffer.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"

ConstantBuffer ConstantBuffer::Create( std::size_t size )
{
	ConstantBuffer cb;

	BUFFER_TRAIT trait = {
		static_cast<UINT>( size ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_BIND_TYPE::CONSTANT_BUFFER
	};

	cb.m_buffer = GetInterface<IAga>( )->CreateBuffer( trait );
	if ( IsInRenderThread( ) )
	{
		cb.m_buffer->Init( );
	}
	else
	{
		EnqueueRenderTask( [buffer = cb.m_buffer]( )
		{
			buffer->Init( );
		} );
	}

	return cb;
}

void ConstantBuffer::Update( const void* data, std::size_t size )
{
	assert( IsInRenderThread( ) );

	assert( data != nullptr );
	void* dst = GetInterface<IAga>( )->Lock( m_buffer );
	if ( dst )
	{
		std::memcpy( dst, data, size );
	}
	GetInterface<IAga>( )->UnLock( m_buffer );
}

void ConstantBuffer::Bind( SHADER_TYPE shaderType, UINT slot )
{
	aga::Buffer* buffer[] = { m_buffer };
	GetInterface<IAga>( )->BindConstantBuffer( shaderType, slot, 1, buffer );
}
