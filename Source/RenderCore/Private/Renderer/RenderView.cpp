#include "Renderer/RenderView.h"

namespace rendercore
{
	RenderView& RenderViewGroup::AddRenderView()
	{
		m_viewGroup.emplace_back();
		return m_viewGroup.back();
	}
}
