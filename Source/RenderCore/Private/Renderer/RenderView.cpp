#include "stdafx.h"
#include "Renderer/RenderView.h"

RenderView& RenderViewGroup::AddRenderView( )
{
	m_viewGroup.emplace_back( );
	return m_viewGroup.back( );
}

