#include "PanelSharedContext.h"

using ::logic::GameObject;

namespace editor
{
	void PanelSharedContext::SelectObject( GameObject* object )
	{
		m_selectedObject.emplace( object );
	}

	void PanelSharedContext::UnselectObject()
	{
		m_selectedObject.clear();
	}

	bool PanelSharedContext::IsSelectedObject( GameObject* object )
	{
		return m_selectedObject.contains( object );
	}

	const std::set<logic::GameObject*>& PanelSharedContext::GetSelectedObjects() const
	{
		return m_selectedObject;
	}

	void PanelSharedContext::OpenProfiler( bool open )
	{
		m_openProfiler = open;
	}

	bool PanelSharedContext::ShouldDrawProfiler() const
	{
		return m_openProfiler;
	}

	void PanelSharedContext::OpenPipelineStatistics( bool open )
	{
		m_openPipelineStatistics = open;
	}

	bool PanelSharedContext::ShouldDrawPipelineStatistics() const
	{
		return m_openPipelineStatistics;
	}
}
