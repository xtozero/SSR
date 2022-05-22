#include "stdafx.h"
#include "VertexLayout.h"

#include "TaskScheduler.h"

Archive& operator<<( Archive& ar, VertexLayoutDesc& desc )
{
	ar << desc.m_size;
	for ( uint32 i = 0; i < desc.Size(); ++i )
	{
		ar << desc.m_layoutData[i].m_isInstanceData;
		ar << desc.m_layoutData[i].m_index;
		ar << desc.m_layoutData[i].m_format;
		ar << desc.m_layoutData[i].m_slot;
		ar << desc.m_layoutData[i].m_instanceDataStep;
		ar << desc.m_layoutData[i].m_name;
	}

	return ar;
}

aga::VertexLayout* VertexLayout::Resource()
{
	return m_layout.Get();
}

const aga::VertexLayout* VertexLayout::Resource() const
{
	return m_layout.Get();
}

VertexLayout::VertexLayout( const VertexShader& vs, const VertexLayoutDesc& desc )
{
	InitResource( vs, desc );
}

void VertexLayout::InitResource( const VertexShader& vs, const VertexLayoutDesc& desc )
{
	assert( IsInRenderThread() );
	m_layout = aga::VertexLayout::Create( vs.Resource(), desc.Data(), desc.Size() );
}
