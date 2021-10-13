add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
add_definitions(-DWIN32=1)
endif()

set (EXCUTABLE_SOUCE
	./Private/Pch/stdafx.cpp
	./Private/ScreenSpaceReflection.cpp
	./Private/Window.cpp
	./Public/Pch/stdafx.h
	./Public/Pch/targetver.h
	./Public/Resource.h
	./Public/ScreenSpaceReflection.h
	./Public/Window.h)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	
add_executable(ScreenSpaceReflection WIN32 ${EXCUTABLE_SOUCE})

target_include_directories(ScreenSpaceReflection
PRIVATE
	./Public
	./Public/Pch)

target_link_libraries(ScreenSpaceReflection
PRIVATE
	Core
	Engine)

set_target_properties( "ScreenSpaceReflection" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Program" )