#include "Renderer/RenderView.h"

namespace rendercore
{
	RenderView& RenderViewGroup::AddRenderView()
	{
		m_views.emplace_back();
		return m_views.back();
	}
}
