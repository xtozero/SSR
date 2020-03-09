add_definitions(-DENGINE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)

set (PRECOMPILED
	"Private/stdafx.cpp"
	"Public/stdafx.h"
	"Public/targetver.h")

set (CORE_SOURCE
	"Private/Core/dllmain.cpp"
	"Private/Core/Engine.cpp"
	"Public/Core/IEngine.h")

set (CONSOLE_MESSAGE_SOURCES
	"Private/ConsoleMessage/ConCommand.cpp"
	"Private/ConsoleMessage/ConsoleMessageExecutor.cpp"
	"Private/ConsoleMessage/ConVar.cpp"
	"Public/ConsoleMessage/ConCommand.h"
	"Public/ConsoleMessage/ConsoleMessageExecutor.h"
	"Public/ConsoleMessage/ConVar.h"
	"Public/ConsoleMessage/IConsoleMessage.h")
	
set (DATA_STRUCTURE_SOURCE
	"Private/DataStructure/EnumStringMap.cpp"
	"Private/DataStructure/KeyValueReader.cpp"
	"Public/DataStructure/EnumStringMap.h"
	"Public/DataStructure/KeyValueReader.h")
	
set (PLATFORM_SOURCE
	"Private/Platform/WindowPlatformEngine.cpp"
	"Private/Platform/WindowPlatformInputConvertor.cpp"
	"Public/Platform/IPlatform.h"
	"Public/Platform/WindowPlatformEngine.h"
	"Public/Platform/WindowPlatformInputConvertor.h")
	
set (USER_INPUT_SOURCE
	"Public/UserInput/UICMap.h"
	"Public/UserInput/UserInput.h")
	
source_group("ConsoleMessage" FILES ${CONSOLE_MESSAGE_SOURCES})
source_group("DataStructure" FILES ${DATA_STRUCTURE_SOURCE})
source_group("Window" FILES ${PLATFORM_SOURCE})
source_group("UserInput" FILES &{USER_INPUT_SOURCE})

include_directories (./Public)
include_directories (../Shared/Public)
include_directories (../Logic/Public)

add_library(Engine SHARED	${PRECOMPILED} 
							${PRECOMPILED}
							${CORE_SOURCE}
							${CONSOLE_MESSAGE_SOURCES}
							${DATA_STRUCTURE_SOURCE}
							${PLATFORM_SOURCE}
							${USER_INPUT_SOURCE})
							
add_custom_command(TARGET Engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/\$\(Configuration\)/Engine.lib" "${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries/Engine.lib"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/\$\(Configuration\)/Engine.dll" "${CMAKE_CURRENT_SOURCE_DIR}/../../Program/Binaries/Engine.dll")
