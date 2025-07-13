#pragma once

#include <set>

namespace logic
{
	class GameObject;
}

namespace editor
{
	class PanelSharedContext
	{
	public:
		void SelectObject( logic::GameObject* object );
		void UnselectObject();
		bool IsSelectedObject( logic::GameObject* object );
		const std::set<logic::GameObject*>& GetSelectedObjects() const;

		void OpenProfiler( bool open );
		bool ShouldDrawProfiler() const;

		void OpenPipelineStatistics( bool open );
		bool ShouldDrawPipelineStatistics() const;

		PanelSharedContext() = default;
		PanelSharedContext( const PanelSharedContext& ) = delete;
		PanelSharedContext& operator=( const PanelSharedContext& ) = delete;
		PanelSharedContext( PanelSharedContext&& ) = delete;
		PanelSharedContext& operator=( PanelSharedContext&& ) = delete;

	private:
		std::set<logic::GameObject*> m_selectedObject;

		bool m_openProfiler = false;
		bool m_openPipelineStatistics = false;
	};
}