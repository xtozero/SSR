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
	Private/Renderer/ForwardRenderer.cpp
	Private/Renderer/RenderView.cpp
	Private/Renderer/SceneRenderer.cpp
	Public/Renderer/ConstantSlotDefine.h
	Public/Renderer/ForwardRenderer.h
	Public/Renderer/IRenderCore.h
	Public/Renderer/RenderView.h
	Public/Renderer/SceneRenderer.h)

set (RENDER_UTILITY
	Private/RenderUtility/AgaDelegator.cpp
	Public/RenderUtility/AgaDelegator.h
	Public/RenderUtility/ShaderParameterUtil.h)
	
set (GRAPHICS_RESOURCE_SOURCE
	Private/GraphicsResource/ByteBuffer.cpp
	Private/GraphicsResource/ConstantBuffer.cpp
	Private/GraphicsResource/DefaultConstantBuffers.cpp
	Private/GraphicsResource/DepthStencil.cpp
	Private/GraphicsResource/GlobalShaders.cpp
	Private/GraphicsResource/GraphicsPipelineState.cpp
	Private/GraphicsResource/IndexBuffer.cpp
	Private/GraphicsResource/RenderOption.cpp
	Private/GraphicsResource/Shader.cpp
	Private/GraphicsResource/UploadBuffer.cpp
	Private/GraphicsResource/VertexBuffer.cpp
	Private/GraphicsResource/VertexLayout.cpp
	Private/GraphicsResource/Viewport.cpp
	Public/GraphicsResource/ByteBuffer.h
	Public/GraphicsResource/ConstantBuffer.h
	Public/GraphicsResource/DefaultConstantBuffers.h
	Public/GraphicsResource/DepthStencil.h
	Public/GraphicsResource/DrawSnapshot.h
	Public/GraphicsResource/GlobalShaders.h
	Public/GraphicsResource/GraphicsPipelineState.h
	Public/GraphicsResource/IndexBuffer.h
	Public/GraphicsResource/RenderOption.h
	Public/GraphicsResource/Shader.h
	Public/GraphicsResource/ShaderBindings.h
	Public/GraphicsResource/TypedBuffer.h
	Public/GraphicsResource/UploadBuffer.h
	Public/GraphicsResource/VertexBuffer.h
	Public/GraphicsResource/VertexLayout.h
	Public/GraphicsResource/VertexInputStream.h
	Public/GraphicsResource/Viewport.h)
	
set (PROXIES_SOURCE
	"Private/Proxies/PrimitiveProxy.cpp"
	"Private/Proxies/StaticMeshPrimitiveProxy.cpp"
	"Public/Proxies/PrimitiveProxy.h"
	"Public/Proxies/StaticMeshPrimitiveProxy.h")

set (RENDER_SCENE_SOURCE
	"Private/Scene/PrimitiveSceneInfo.cpp"
	"Private/Scene/Scene.cpp"
	"Private/Scene/SceneConstantBuffers.cpp"
	"Public/Scene/IScene.h"
	"Public/Scene/PrimitiveSceneInfo.h"
	"Public/Scene/Scene.h"
	"Public/Scene/SceneConstantBuffers.h")
	
source_group("GraphicsResource" FILES ${GRAPHICS_RESOURCE_SOURCE})
source_group("Mesh" FILES ${MESH_SOURCE})
source_group("Proxies" FILES ${PROXIES_SOURCE})
source_group("Renderer" FILES ${RENDERER_SOURCE})
source_group("Scene" FILES ${RENDER_SCENE_SOURCE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_library(RenderCore SHARED 	${PRECOMPILED}
								${CORE_SOURCE}
								${MESH_SOURCE}
								${RENDERER_SOURCE}
								${RENDER_SCENE_SOURCE}
								${RENDER_UTILITY}
								${GRAPHICS_RESOURCE_SOURCE}
								${PROXIES_SOURCE}
								${SCENE_SOURCE})

target_link_libraries(RenderCore Aga)

target_include_directories(RenderCore
PUBLIC
	./Public
	./Public/Core
	./Public/GraphicsResource
	./Public/Material
	./Public/Renderer
	../Aga/Public/Common
	../Core/Public
	../Shared/Public
	../Engine/Public
	../Logic/Public
PRIVATE
	./Public/RenderUtility
	)

add_custom_command(TARGET RenderCore POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/RenderCore.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/RenderCore.dll")