#include "stdafx.h"
#include "PipelineState.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"

namespace aga
{
	RefHandle<DepthStencilState> DepthStencilState::Create( const DEPTH_STENCIL_STATE_TRAIT& trait )
	{
		return GetInterface<IAga>( )->CreateDepthStencilState( trait );
	}
}
