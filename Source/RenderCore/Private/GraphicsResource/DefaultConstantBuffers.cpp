#include "stdafx.h"
#include "DefaultConstantBuffers.h"

#include "CommandList.h"
#include "Shader.h"
#include "TaskScheduler.h"

void DefaultConstantBuffers::BootUp()
{
	for ( uint32 i = 0; i < MAX_SHADER_TYPE<uint32>; ++i )
	{
		ConstantBufferContext& context = m_contexts[i];

		context.m_buffer = ConstantBuffer( BUFFER_SIZE );
		context.m_dataStorage = new unsigned char[BUFFER_SIZE];
		context.m_invalidRangeEnd = 0;
	}

	auto SetDefaultConstant = [contexts = m_contexts]()
	{
		auto commandList = rendercore::GetImmediateCommandList();

		for ( uint32 i = 0; i < MAX_SHADER_TYPE<uint32>; ++i )
		{
			auto shaderType = static_cast<SHADER_TYPE>( i );
			commandList.BindConstantBuffer( shaderType, 0, contexts[i].m_buffer.Resource() );
		}
	};

	EnqueueRenderTask( [SetDefaultConstant]()
		{
			SetDefaultConstant();
		} );
}

void DefaultConstantBuffers::Shutdown()
{
	for ( uint32 i = 0; i < MAX_SHADER_TYPE<uint32>; ++i )
	{
		ConstantBufferContext& context = m_contexts[i];
		context.m_dataStorage = {};
		delete[] context.m_dataStorage;
		context.m_dataStorage = nullptr;
		context.m_invalidRangeEnd = 0;
	}
}

void DefaultConstantBuffers::SetShaderValue( SHADER_TYPE shader, uint32 offset, uint32 numBytes, const void* value )
{
	assert( ( SHADER_TYPE::NONE < shader&& shader < SHADER_TYPE::Count ) && "Invalid shader type" );
	SetShaderValueInternal( static_cast<uint32>( shader ), offset, numBytes, value );
}

void DefaultConstantBuffers::Commit( SHADER_TYPE shader )
{
	assert( ( SHADER_TYPE::NONE < shader&& shader < SHADER_TYPE::Count ) && "Invalid shader type" );
	CommitInternal( static_cast<uint32>( shader ) );
}

void DefaultConstantBuffers::CommitAll()
{
	for ( uint32 i = 0; i < MAX_SHADER_TYPE<uint32>; ++i )
	{
		CommitInternal( i );
	}
}

void DefaultConstantBuffers::SetShaderValueInternal( uint32 ctxIndex, uint32 offset, uint32 numBytes, const void* value )
{
	ConstantBufferContext& context = m_contexts[ctxIndex];
	std::memcpy( context.m_dataStorage + offset, value, numBytes );
	context.m_invalidRangeEnd = std::max( context.m_invalidRangeEnd, offset + numBytes );
}

void DefaultConstantBuffers::CommitInternal( uint32 ctxIndex )
{
	ConstantBufferContext& context = m_contexts[ctxIndex];
	if ( context.m_invalidRangeEnd > 0 )
	{
		context.m_buffer.Update( context.m_dataStorage, context.m_invalidRangeEnd );
		context.m_invalidRangeEnd = 0;
	}
}
