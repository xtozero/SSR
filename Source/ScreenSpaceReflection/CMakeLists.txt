add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
add_definitions(-DWIN32=1)
endif()

set (EXCUTABLE_SOUCE
	"./Private/ScreenSpaceReflection.cpp"
	"./Private/stdafx.cpp"
	"./Private/Window.cpp"
	"./Public/Resource.h"
	"./Public/ScreenSpaceReflection.h"
	"./Public/stdafx.h"
	"./Public/targetver.h"
	"./Public/Window.h")

include_directories(./Public)
include_directories(../Shared/Public)
include_directories(../Engine/Public)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	
add_executable( "ScreenSpaceReflection" WIN32 ${EXCUTABLE_SOUCE} )

set_target_properties( "ScreenSpaceReflection" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Program" )