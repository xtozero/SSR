add_definitions(-DENGINE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (ASSET_LOADER
	Private/AssetLoader/AssetFactory.cpp
	Private/AssetLoader/AssetLoader.cpp
	Public/AssetLoader/AssetFactory.h
	Public/AssetLoader/AssetLoader.h
	Public/AssetLoader/IAsyncLoadableAsset.h
)

set (CORE_SOURCE
	"Private/Core/dllmain.cpp"
	"Private/Core/Engine.cpp"
	"Private/Core/Interfacefactories.cpp"
	"Private/Core/ModuleInit.cpp"
	"Public/Core/IEngine.h"
	"Public/Core/InterfaceFactories.h")

set (CONSOLE_MESSAGE_SOURCES
	"Private/ConsoleMessage/ConCommand.cpp"
	"Private/ConsoleMessage/ConsoleMessageExecutor.cpp"
	"Private/ConsoleMessage/ConVar.cpp"
	"Public/ConsoleMessage/ConCommand.h"
	"Public/ConsoleMessage/ConsoleMessageExecutor.h"
	"Public/ConsoleMessage/ConVar.h"
	"Public/ConsoleMessage/IConsoleMessage.h")
	
set (DATA_STRUCTURE_SOURCE
	"../Core/Public/DoubleLinkedList.h"
	"../Core/Public/SingleLInkedList.h"
	"Private/DataStructure/EnumStringMap.cpp"
	"Public/DataStructure/EnumStringMap.h")
	
set (PLATFORM_SOURCE
	"Private/Platform/WindowPlatformEngine.cpp"
	"Private/Platform/WindowPlatformInputConvertor.cpp"
	"Public/Platform/IPlatform.h"
	"Public/Platform/WindowPlatformEngine.h"
	"Public/Platform/WindowPlatformInputConvertor.h")

set (PRECOMPILED
	"Private/stdafx.cpp"
	"Public/stdafx.h"
	"Public/targetver.h")
	
set (USER_INPUT_SOURCE
	"Private/UserInput/UICMap.cpp"
	"Public/UserInput/UICMap.h"
	"Public/UserInput/UserInput.h")
	
set (MULTI_THREAD
	"../Core/Private/TaskScheduler.cpp"
	"../Core/Public/TaskScheduler.h"
	"Private/MultiThread/EngineTaskScheduler.cpp"
	"Public/MultiThread/EngineTaskScheduler.h")
	
set (FILE_SYSTEM
	"../Core/Public/FileSystem.h"
	"Private/FileSystem/EngineFileSystem.cpp"
	"Public/FileSystem/EngineFileSystem.h")
	
set (MEMORY_MANAGEMENT
	"../Core/Public/FixedBlockMemoryPool.h"
	"../Core/Public/MemoryPool.h")

source_group("AssetLoader" FILES ${ASSET_LOADER} )		
source_group("ConsoleMessage" FILES ${CONSOLE_MESSAGE_SOURCES})
source_group("DataStructure" FILES ${DATA_STRUCTURE_SOURCE})
source_group("Window" FILES ${PLATFORM_SOURCE})
source_group("UserInput" FILES ${USER_INPUT_SOURCE})
source_group("MultiThread" FILES ${MULTI_THREAD})
source_group("FileSystem" FILES ${FILE_SYSTEM})
source_group("MemoryManagement" FILES ${MEMORY_MANAGEMENT})

include_directories(./Public)
include_directories(../Core/Public)
include_directories(../Logic/Public)
include_directories(../Shared/Public)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(Engine SHARED	${ASSET_LOADER}
							${CORE_SOURCE}
							${CONSOLE_MESSAGE_SOURCES}
							${DATA_STRUCTURE_SOURCE}
							${PLATFORM_SOURCE}
							${PRECOMPILED} 
							${USER_INPUT_SOURCE}
							${MULTI_THREAD}
							${FILE_SYSTEM}
							${MEMORY_MANAGEMENT})
							
add_custom_command(TARGET Engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/Engine.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/Engine.dll")
