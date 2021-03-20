#include "stdafx.h"
#include "VertexLayout.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"

VertexLayout VertexLayout::Create( const VertexShader& vs, const VertexLayoutDesc& desc )
{
	assert( IsInRenderThread( ) );
	std::size_t size = desc.Size( );
	aga::VertexLayout* layout( GetInterface<IAga>( )->CreateVertexLayout( vs, desc.Data( ), size ) );

	return VertexLayout( layout );
}
