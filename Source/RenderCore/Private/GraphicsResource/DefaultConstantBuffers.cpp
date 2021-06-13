#include "stdafx.h"
#include "DefaultConstantBuffers.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"
#include "RenderUtility/ShaderParameterUtil.h"

void DefaultConstantBuffers::BootUp( )
{
	for ( int i = 0; i < TotalConstants; ++i )
	{
		ConstantBufferContext& context = m_contexts[i];

		context.m_buffer = ConstantBuffer( BUFFER_SIZE );
		context.m_dataStorage = new unsigned char[BUFFER_SIZE];
		context.m_invalidRangeEnd = 0;
	}

	auto SetDefaultConstant = [contexts = m_contexts]( )
	{
		aga::ShaderParameter param = {  };
		param.m_type = aga::ShaderParameterType::ConstantBuffer;
		param.m_bindPoint = 0;
		param.m_offset = 0;

		VertexShader vs;
		BindShaderParameter( vs, param, contexts[VS].m_buffer.Resource( ) );
		PixelShader ps;
		BindShaderParameter( ps, param, contexts[PS].m_buffer.Resource( ) );
		ComputeShader cs;
		BindShaderParameter( cs, param, contexts[CS].m_buffer.Resource( ) );
	};

	if ( IsInRenderThread( ) )
	{
		SetDefaultConstant( );
	}
	else
	{
		EnqueueRenderTask( [SetDefaultConstant]( )
		{
			SetDefaultConstant( );
		} );
	}
}

void DefaultConstantBuffers::Shutdown( )
{
	for ( int i = 0; i < TotalConstants; ++i )
	{
		ConstantBufferContext& context = m_contexts[i];
		context.m_dataStorage = {};
		delete[] context.m_dataStorage;
		context.m_dataStorage = nullptr;
		context.m_invalidRangeEnd = 0;
	}
}

void DefaultConstantBuffers::CommitAll( )
{
	for ( int i = 0; i < TotalConstants; ++i )
	{
		Commit( i );
	}
}

void DefaultConstantBuffers::SetShaderValue( int ctxIndex, UINT offset, UINT numBytes, const void* value )
{
	ConstantBufferContext& context = m_contexts[ctxIndex];
	std::memcpy( context.m_dataStorage + offset, value, numBytes );
	context.m_invalidRangeEnd = offset + numBytes;
}

void DefaultConstantBuffers::Commit( int ctxIndex )
{
	ConstantBufferContext& context = m_contexts[ctxIndex];
	if ( context.m_invalidRangeEnd > 0 )
	{
		context.m_buffer.Update( context.m_dataStorage, context.m_invalidRangeEnd );
		context.m_invalidRangeEnd = 0;
	}
}
