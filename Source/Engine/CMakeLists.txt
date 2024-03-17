add_definitions(-DENGINE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (CONFIG_SOURCE
	Private/Config/DefaultAppConfig.cpp
	Public/Config/DefaultAppConfig.h)

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
	
set (CPU_PROFILER_HEADER
	Public/CpuProfiler/CpuProfiler.h)

set (CPU_PROFILER_SOURCE
	Private/CpuProfiler/CpuProfiler.cpp)

set (PLATFORM_SOURCE
	Private/Platform/CommandLine.cpp
	Private/Platform/WindowPlatformEngine.cpp
	Private/Platform/WindowPlatformInputConvertor.cpp
	Public/Platform/CommandLine.h
	Public/Platform/IPlatform.h
	Public/Platform/WindowPlatformEngine.h
	Public/Platform/WindowPlatformInputConvertor.h)
	
set (USER_INPUT_HEADER
	Public/UserInput/UICMap.h
	Public/UserInput/UserInput.h)

set (USER_INPUT_SOURCE
	Private/UserInput/UICMap.cpp)

source_group("Config" FILES ${CONFIG_SOURCE})	
source_group("ConsoleMessage" FILES ${CONSOLE_MESSAGE_SOURCES})
source_group("CpuProfiler" FILES ${CPU_PROFILER_HEADER} ${CPU_PROFILER_SOURCE})
source_group("UserInput" FILES ${USER_INPUT_HEADER} ${USER_INPUT_SOURCE})
source_group("Window" FILES ${PLATFORM_SOURCE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(Engine SHARED	${CONFIG_SOURCE}
							${CONSOLE_MESSAGE_SOURCES}
							${CORE_SOURCE}
							${CPU_PROFILER_HEADER}
							${CPU_PROFILER_SOURCE}
							${PLATFORM_SOURCE}
							${USER_INPUT_HEADER}
							${USER_INPUT_SOURCE})

target_include_directories(Engine
PUBLIC
	./Public
	./Public/CpuProfiler
PRIVATE
	../Editor/Public
	../Logic/Public
	./Public/AssetLoader
	./Public/ConsoleMessage
	./Public/Core
	./Public/CpuProfiler
	./Public/FileSystem
	./Public/Platform
	./Public/UserInput)
		
target_link_libraries(Engine
PRIVATE
	Core)

target_precompile_headers(Engine
PUBLIC
	${CPU_PROFILER_HEADER}
	${USER_INPUT_HEADER})
							
add_custom_command(TARGET Engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/Engine.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/Engine.dll")
