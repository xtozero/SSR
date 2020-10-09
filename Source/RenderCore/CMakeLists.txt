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
	
set (MESH_SOURCE
	"Private/Mesh/StaticMeshResource.cpp"
	"Public/Mesh/StaticMeshResource.h")

set (RENDERER_SOURCE
	"Private/Renderer/ForwardRenderer.cpp"
	"Public/Renderer/IRenderCore.h")

set (RENDER_UTIL
	"Private/Renderer/RenderOutputManager.cpp"
	"Public/Renderer/RenderOutputManager.h")
	
set (GRAPHICS_RESOURCE_SOURCE
	"Private/GraphicsResource/IndexBuffer.cpp"
	"Private/GraphicsResource/RenderOption.cpp"
	"Private/GraphicsResource/RenderOptionManager.cpp"
	"Public/GraphicsResource/DrawSnapshot.h"
	"Public/GraphicsResource/IndexBuffer.h"
	"Public/GraphicsResource/RenderOption.h"
	"Public/GraphicsResource/RenderOptionManager.h"
	"Public/GraphicsResource/ShaderBindings.h"
	"Public/GraphicsResource/VertexBuffer.h"
	"Public/GraphicsResource/VertexInputStream.h")
	
set (PROXIES_SOURCE
	"Private/Proxies/PrimitiveProxy.cpp"
	"Private/Proxies/StaticMeshPrimitiveProxy.cpp"
	"Public/Proxies/PrimitiveProxy.h"
	"Public/Proxies/StaticMeshPrimitiveProxy.h")

set (RENDER_SCENE_SOURCE
	"Private/Scene/PrimitiveSceneInfo.cpp"
	"Private/Scene/Scene.cpp"
	"Public/Scene/IScene.h"
	"Public/Scene/PrimitiveSceneInfo.h"
	"Public/Scene/Scene.h")
	
source_group("GraphicsResource" FILES ${GRAPHICS_RESOURCE_SOURCE})
source_group("Mesh" FILES ${MESH_SOURCE})
source_group("Proxies" FILES ${PROXIES_SOURCE})
source_group("Renderer" FILES ${RENDERER_SOURCE})
source_group("RenderUtil" FILES ${RENDER_UTIL})
source_group("Scene" FILES ${RENDER_SCENE_SOURCE})

include_directories(./Public)
include_directories(./Public/Core)
include_directories(./Public/Material)
include_directories(./Public/Renderer)
include_directories(../Aga/Public/Common)
include_directories(../Core/Public)
include_directories(../Shared/Public)
include_directories(../Engine/Public)
include_directories(../Logic/Public)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_library(RenderCore SHARED 	${PRECOMPILED}
								${CORE_SOURCE}
								${MESH_SOURCE}
								${RENDERER_SOURCE}
								${RENDER_SCENE_SOURCE}
								${RENDER_UTIL}
								${GRAPHICS_RESOURCE_SOURCE}
								${PROXIES_SOURCE}
								${SCENE_SOURCE})

add_custom_command(TARGET RenderCore POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/RenderCore.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/RenderCore.dll")