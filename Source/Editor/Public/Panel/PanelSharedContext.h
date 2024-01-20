#pragma once

#include <set>

namespace logic
{
	class CGameObject;
}

namespace editor
{
	class PanelSharedContext
	{
	public:
		void SelectObject( logic::CGameObject* object );
		void UnselectObject();
		bool IsSelectedObject( logic::CGameObject* object );
		const std::set<logic::CGameObject*>& GetSelectedObjects() const;

		void OpenProfiler( bool open );
		bool ShouldDrawProfiler() const;

		PanelSharedContext() = default;
		PanelSharedContext( const PanelSharedContext& ) = delete;
		PanelSharedContext& operator=( const PanelSharedContext& ) = delete;
		PanelSharedContext( PanelSharedContext&& ) = delete;
		PanelSharedContext& operator=( PanelSharedContext&& ) = delete;

	private:
		std::set<logic::CGameObject*> m_selectedObject;

		bool m_openProfiler = false;
	};
}