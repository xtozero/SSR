#include "ImguiSharedContext.h"

#include <cstdlib>

namespace imgui
{
	static void* MallocWrapper( size_t size, [[maybe_unused]] void* user_data )
	{
		return std::malloc( size );
	}

	static void FreeWrapper( void* ptr, [[maybe_unused]] void* user_data )
	{
		free( ptr );
	}

	SharedContext GetSharedContext()
	{
		if ( ImGui::GetCurrentContext() == nullptr )
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			ImGui::StyleColorsDark();

			ImGui::SetAllocatorFunctions( &MallocWrapper, &FreeWrapper );
		}

		SharedContext sharedContext{
			.m_context = ImGui::GetCurrentContext(),
			.m_allocFunc = &MallocWrapper,
			.m_freeFunc = &FreeWrapper
		};

		return sharedContext;
	}

	void DestorySharedContext( const SharedContext& sharedContext )
	{
		if ( sharedContext.m_context != nullptr )
		{
			ImGui::DestroyContext( sharedContext.m_context );
		}
	}
}