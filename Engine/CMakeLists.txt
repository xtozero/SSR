add_definitions(-DENGINE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)

set (PRECOMPILED
	"Source/stdafx.cpp"
	"Source/stdafx.h"
	"Source/targetver.h")

set (CORE_SOURCE
	"Source/Private/Core/dllmain.cpp"
	"Source/Private/Core/Engine.cpp"
	"Source/Public/Core/IEngine.h")

set (CONSOLE_MESSAGE_SOURCES
	"Source/Private/ConsoleMessage/ConCommand.cpp"
	"Source/Private/ConsoleMessage/ConsoleMessageExecutor.cpp"
	"Source/Private/ConsoleMessage/ConVar.cpp"
	"Source/Public/ConsoleMessage/ConCommand.h"
	"Source/Public/ConsoleMessage/ConsoleMessageExecutor.h"
	"Source/Public/ConsoleMessage/ConVar.h"
	"Source/Public/ConsoleMessage/IConsoleMessage.h")
	
set (DATA_STRUCTURE_SOURCE
	"Source/Private/DataStructure/EnumStringMap.cpp"
	"Source/Private/DataStructure/KeyValueReader.cpp"
	"Source/Public/DataStructure/EnumStringMap.h"
	"Source/Public/DataStructure/KeyValueReader.h")
	
set (PLATFORM_SOURCE
	"Source/Private/Platform/WindowPlatformEngine.cpp"
	"Source/Private/Platform/WindowPlatformInputConvertor.cpp"
	"Source/Public/Platform/IPlatform.h"
	"Source/Public/Platform/WindowPlatformEngine.h"
	"Source/Public/Platform/WindowPlatformInputConvertor.h")
	
set (USER_INPUT_SOURCE
	"Source/Public/UserInput/UICMap.h"
	"Source/Public/UserInput/UserInput.h")
	
source_group("ConsoleMessage" FILES ${CONSOLE_MESSAGE_SOURCES})
source_group("DataStructure" FILES ${DATA_STRUCTURE_SOURCE})
source_group("Window" FILES ${PLATFORM_SOURCE})
source_group("UserInput" FILES &{USER_INPUT_SOURCE})

include_directories	(Source)
include_directories (Source/Public)
include_directories (../Shared/Public)
include_directories (../Logic/Source/Public)

add_library(Engine SHARED	${PRECOMPILED} 
							${PRECOMPILED}
							${CORE_SOURCE}
							${CONSOLE_MESSAGE_SOURCES}
							${DATA_STRUCTURE_SOURCE}
							${PLATFORM_SOURCE}
							${USER_INPUT_SOURCE})
							
add_custom_command(TARGET Engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/\$\(Configuration\)/Engine.lib" "${CMAKE_CURRENT_SOURCE_DIR}/../lib/Engine.lib"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/\$\(Configuration\)/Engine.dll" "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Engine.dll")
