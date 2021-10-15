#include "stdafx.h"
#include "VertexLayout.h"

#include "TaskScheduler.h"

aga::VertexLayout* VertexLayout::Resource( )
{
	return m_layout.Get( );
}

const aga::VertexLayout* VertexLayout::Resource( ) const
{
	return m_layout.Get( );
}

VertexLayout::VertexLayout( const VertexShader& vs, const VertexLayoutDesc& desc )
{
	InitResource( vs, desc );
}

void VertexLayout::InitResource( const VertexShader& vs, const VertexLayoutDesc& desc )
{
	assert( IsInRenderThread( ) );
	m_layout = aga::VertexLayout::Create( vs.Resource( ), desc.Data( ), desc.Size( ) );
}
