add_definitions(-DEDITOR_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (CORE
	./Private/Core/DllMain.cpp
	./Private/Core/ModuleInit.cpp
	./Public/Core/IEditor.h)

set (IMGUI_EDITOR
	./Private/ImguiEditor/ImguiEditor.cpp
	./Public/ImguiEditor/ImguiEditor.h)

set (PRECOMPILED
	./Private/Pch/stdafx.cpp
	./Public/Pch/stdafx.h
	./Public/Pch/targetver.h)

source_group("Core" FILES ${CORE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(Editor SHARED	${CORE}
							${IMGUI_EDITOR}
							${PRECOMPILED})

target_include_directories(Editor
PUBLIC
	../Logic/Public
	./Public
	./Public/Pch
PRIVATE
	./Public/Core
	./Public/ImguiEditor
)

target_link_libraries(Editor
PRIVATE
	Core
	Engine
	Imgui)
							
add_custom_command(TARGET Editor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/Editor.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/Editor.dll")