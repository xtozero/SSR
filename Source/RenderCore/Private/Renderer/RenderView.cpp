#include "Renderer/RenderView.h"

#include "TaskScheduler.h"

namespace rendercore
{
	RenderViewState* RenderViewStateReference::Get() const
	{
		return m_viewState;
	}

	void RenderViewStateReference::Allocate()
	{
		m_viewState = new RenderViewState();
	}

	RenderViewStateReference::~RenderViewStateReference()
	{
		if ( m_viewState )
		{
			EnqueueRenderTask( [viewState = m_viewState]()
				{
					delete viewState;
				} );
			m_viewState = nullptr;
		}
	}

	RenderView& RenderViewGroup::AddRenderView()
	{
		m_views.emplace_back();
		return m_views.back();
	}
}
