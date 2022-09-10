add_definitions(-DENGINE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (CONSOLE_MESSAGE_SOURCES
	Private/ConsoleMessage/ConCommand.cpp
	Private/ConsoleMessage/ConsoleMessageExecutor.cpp
	Private/ConsoleMessage/ConVar.cpp
	Public/ConsoleMessage/ConCommand.h
	Public/ConsoleMessage/ConsoleMessageExecutor.h
	Public/ConsoleMessage/ConVar.h
	Public/ConsoleMessage/IConsoleMessage.h)
	
set (CORE_SOURCE
	Private/Core/dllmain.cpp
	Private/Core/Engine.cpp
	Private/Core/ModuleInit.cpp
	Public/Core/IEngine.h)
	
set (PLATFORM_SOURCE
	Private/Platform/CommandLine.cpp
	Private/Platform/WindowPlatformEngine.cpp
	Private/Platform/WindowPlatformInputConvertor.cpp
	Public/Platform/CommandLine.h
	Public/Platform/IPlatform.h
	Public/Platform/WindowPlatformEngine.h
	Public/Platform/WindowPlatformInputConvertor.h)

set (PRECOMPILED
	Private/Pch/stdafx.cpp
	Public/Pch/stdafx.h
	Public/Pch/targetver.h)
	
set (USER_INPUT_SOURCE
	Private/UserInput/UICMap.cpp
	Public/UserInput/UICMap.h
	Public/UserInput/UserInput.h)
	
source_group("ConsoleMessage" FILES ${CONSOLE_MESSAGE_SOURCES})
source_group("UserInput" FILES ${USER_INPUT_SOURCE})
source_group("Window" FILES ${PLATFORM_SOURCE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(Engine SHARED	${CONSOLE_MESSAGE_SOURCES}
							${CORE_SOURCE}
							${PLATFORM_SOURCE}
							${PRECOMPILED} 
							${USER_INPUT_SOURCE})

target_include_directories(Engine
PUBLIC
	./Public
PRIVATE
	../Logic/Public
	./Public/AssetLoader
	./Public/ConsoleMessage
	./Public/Core
	./Public/FileSystem
	./Public/Pch
	./Public/Platform
	./Public/PrecompiledHeader
	./Public/UserInput)		
		
target_link_libraries(Engine
PRIVATE
	Core)
							
add_custom_command(TARGET Engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/Engine.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/Engine.dll")
