add_definitions(-DIMGUI_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (IMGUI
	../../ThirdParty/ImGui/imgui.cpp
	../../ThirdParty/ImGui/imgui_draw.cpp
	../../ThirdParty/ImGui/imgui_tables.cpp
	../../ThirdParty/ImGui/imgui_widgets.cpp
	../../ThirdParty/ImGui/imconfig.h
	../../ThirdParty/ImGui/imgui.h
	../../ThirdParty/ImGui/imgui_internal.h)

set (IMGUI_HELPER
	./Public/ImguiSharedContext.h
	./Private/ImguiSharedContext.cpp)

set (IMGUI_WINDOWS_BACKEND
	../../ThirdParty/ImGui/backends/imgui_impl_win32.cpp
	../../ThirdParty/ImGui/backends/imgui_impl_win32.h)

source_group("Imgui" FILES ${IMGUI})
source_group("ImguiHelper" FILES ${IMGUI_HELPER})
source_group("ImguiWindowsBackend" FILES ${IMGUI_WINDOWS_BACKEND})

add_library(Imgui STATIC	${IMGUI}
							${IMGUI_HELPER}
							${IMGUI_WINDOWS_BACKEND})

target_include_directories(Imgui
PUBLIC
	./Public
	../../ThirdParty/ImGui
	../../ThirdParty/ImGui/backends
)
							
