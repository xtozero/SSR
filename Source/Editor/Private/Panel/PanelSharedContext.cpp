#include "PanelSharedContext.h"

using ::logic::CGameObject;

namespace editor
{
	void PanelSharedContext::SelectObject( CGameObject* object )
	{
		m_selectedObject.emplace( object );
	}

	void PanelSharedContext::UnselectObject()
	{
		m_selectedObject.clear();
	}

	bool PanelSharedContext::IsSelectedObject( CGameObject* object )
	{
		return m_selectedObject.find( object ) != std::end( m_selectedObject );
	}

	const std::set<logic::CGameObject*>& PanelSharedContext::GetSelectedObjects() const
	{
		return m_selectedObject;
	}
}
