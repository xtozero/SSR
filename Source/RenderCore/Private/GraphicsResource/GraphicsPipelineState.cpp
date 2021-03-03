#include "stdafx.h"
#include "GraphicsPipelineState.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"

DepthStencilState DepthStencilState::Create( const DEPTH_STENCIL_STATE_TRAIT& trait )
{
	aga::DepthStencilState* state( GetInterface<IAga>( )->CreateDepthStencilState( trait ) );
	if ( IsInRenderThread( ) )
	{
		state->Init( );
	}
	else
	{
		EnqueueRenderTask( [state]( )
		{
			state->Init( );
		} );
	}

	return DepthStencilState( state );
}

RasterizerState RasterizerState::Create( const RASTERIZER_STATE_TRAIT& trait )
{
	aga::RasterizerState* state( GetInterface<IAga>( )->CreateRasterizerState( trait ) );
	if ( IsInRenderThread( ) )
	{
		state->Init( );
	}
	else
	{
		EnqueueRenderTask( [state]( )
		{
			state->Init( );
		} );
	}

	return RasterizerState( state );
}