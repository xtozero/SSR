add_definitions(-DENGINE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (ASSET_LOADER
	Private/AssetLoader/AssetFactory.cpp
	Private/AssetLoader/AssetLoader.cpp
	Public/AssetLoader/AssetFactory.h
	Public/AssetLoader/AssetLoader.h
	Public/AssetLoader/IAsyncLoadableAsset.h)

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
	
set (FILE_SYSTEM
	Private/FileSystem/EngineFileSystem.cpp
	Public/FileSystem/EngineFileSystem.h)
	
set (MULTITHREAD
	Private/MultiThread/EngineTaskScheduler.cpp
	Public/MultiThread/EngineTaskScheduler.h)
	
set (PLATFORM_SOURCE
	Private/Platform/WindowPlatformEngine.cpp
	Private/Platform/WindowPlatformInputConvertor.cpp
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

source_group("AssetLoader" FILES ${ASSET_LOADER} )		
source_group("ConsoleMessage" FILES ${CONSOLE_MESSAGE_SOURCES})
source_group("FileSystem" FILES ${FILE_SYSTEM})
source_group("MultiThread" FILES ${MULTITHREAD})
source_group("UserInput" FILES ${USER_INPUT_SOURCE})
source_group("Window" FILES ${PLATFORM_SOURCE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(Engine SHARED	${ASSET_LOADER}
							${CONSOLE_MESSAGE_SOURCES}
							${CORE_SOURCE}
							${FILE_SYSTEM}
							${MULTITHREAD}
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
