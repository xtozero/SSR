add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
add_definitions(-DWIN32=1)
endif()

set (EXCUTABLE_SOUCE
	"./Source/Private/Sceen Space Reflect.cpp"
	"./Source/Private/stdafx.cpp"
	"./Source/Private/Window.cpp"
	"./Source/Public/Resource.h"
	"./Source/Public/Sceen Space Reflect.h"
	"./Source/Public/stdafx.h"
	"./Source/Public/targetver.h"
	"./Source/Public/Window.h")

include_directories(./Source)
include_directories(./Source/Public)
include_directories(../Shared/Public)
include_directories(../Engine/Source/Public)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ )
	
add_executable( "ScreenSpaceReflect" WIN32 ${EXCUTABLE_SOUCE} )

set_target_properties( "ScreenSpaceReflect" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )