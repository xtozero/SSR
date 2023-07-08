#pragma once

#include "imgui.h"

namespace imgui
{
	struct SharedContext
	{
		ImGuiContext* m_context;
		ImGuiMemAllocFunc m_allocFunc;
		ImGuiMemFreeFunc m_freeFunc;
	};

	SharedContext GetSharedContext();
	void DestorySharedContext( const SharedContext& sharedContext );
}