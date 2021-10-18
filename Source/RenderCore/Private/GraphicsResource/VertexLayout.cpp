#include "stdafx.h"
#include "VertexLayout.h"

#include "TaskScheduler.h"

void VertexLayoutDesc::Serialize( Archive& ar )
{
	ar << m_size;
	for ( uint32 i = 0; i < Size( ); ++i )
	{
		ar << m_layoutData[i].m_isInstanceData;
		ar << m_layoutData[i].m_index;
		ar << m_layoutData[i].m_format;
		ar << m_layoutData[i].m_slot;
		ar << m_layoutData[i].m_instanceDataStep;
		ar << m_layoutData[i].m_name;
	}
}

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
