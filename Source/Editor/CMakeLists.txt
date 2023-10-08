add_definitions(-DEDITOR_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (CORE
	./Private/Core/DllMain.cpp
	./Private/Core/ModuleInit.cpp
	./Public/Core/IEditor.h
	./Public/Core/IPanel.h)

set (IMGUI_EDITOR
	./Private/ImguiEditor/ImguiEditor.cpp
	./Public/ImguiEditor/ImguiEditor.h)

set (PANEL
	./Private/Panel/ContentBrowserPanel.cpp
	./Private/Panel/MainMenuPanel.cpp
	./Private/Panel/ScenePanel.cpp
	./Public/Panel/ContentBrowserPanel.h
	./Public/Panel/MainMenuPanel.h
	./Public/Panel/PanelFactory.h
	./Public/Panel/ScenePanel.h)

set (PRECOMPILED
	./Private/Pch/stdafx.cpp
	./Public/Pch/stdafx.h
	./Public/Pch/targetver.h)

source_group("Core" FILES ${CORE})
source_group("ImguiEditor" FILES ${IMGUI_EDITOR})
source_group("Panel" FILES ${PANEL})
source_group("Precomputed" FILES ${PRECOMPILED})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(Editor SHARED	${CORE}
							${IMGUI_EDITOR}
							${PANEL}
							${PRECOMPILED})

target_include_directories(Editor
PUBLIC
	../Logic/Public
	./Public
	./Public/Pch
PRIVATE
	./Public/Core
	./Public/ImguiEditor
	./Public/Panel
)

target_link_libraries(Editor
PRIVATE
	Agl
	Core
	Engine
	Imgui
	Logic
	RenderCore)
							
add_custom_command(TARGET Editor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/Editor.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/Editor.dll")