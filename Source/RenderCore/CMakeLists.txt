add_definitions(-DRENDERCORE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (PRECOMPILED
	"Private/stdafx.cpp"
	"Public/stdafx.h"
	"Public/targetver.h")

set (CORE_SOURCE
	"Private/Core/dllmain.cpp"
	"Private/Core/ModuleInit.cpp"
	"Private/Core/RenderCore.cpp")
	
set (RENDERER_SOURCE
	"Private/Renderer/ForwardRenderer.cpp"
	"Public/Renderer/IRenderCore.h")
	
set (RENDER_SCENE_SOURCE
	"Private/Renderer/Scene.cpp"
	"Public/Renderer/IScene.h")

set (RENDER_UTIL
	"Private/Renderer/RenderOutputManager.cpp"
	"Public/Renderer/RenderOutputManager.h")
	
set (GRAPHICS_RESOURCE_SOURCE
#	"Private/GraphicsResource/"
	"Public/GraphicsResource/DrawSnapshot.h"
	"Public/GraphicsResource/ShaderBindings.h"
	"Public/GraphicsResource/VertexInputStream.h")
	
source_group("Renderer" FILES ${RENDERER_SOURCE})
source_group("RenderUtil" FILES ${RENDER_UTIL})

include_directories(./Public)
include_directories(./Public/Core)
include_directories(./Public/Material)
include_directories(./Public/Renderer)
include_directories(../Aga/Public/Common)
include_directories(../Shared/Public)
include_directories(../Engine/Public)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_library(RenderCore SHARED 	${PRECOMPILED}
								${CORE_SOURCE}
								${RENDERER_SOURCE}
								${RENDER_SCENE_SOURCE}
								${RENDER_UTIL}
								${GRAPHICS_RESOURCE_SOURCE})

add_custom_command(TARGET RenderCore POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/RenderCore.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/RenderCore.dll")