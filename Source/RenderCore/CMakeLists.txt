add_definitions(-DRENDERCORE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (CONFIG_SOURCE
	Private/Config/DefaultRenderCoreConfig.cpp
	Public/Config/DefaultRenderCoreConfig.h)

set (CORE_SOURCE
	Private/Core/dllmain.cpp
	Private/Core/ModuleInit.cpp
	Private/Core/RenderCore.cpp)

set (GPU_PROFILER_SOURCE
	Private/GpuProfiler/GPUProfiler.cpp
	Public/GpuProfiler/GpuProfiler.h)

set (GRAPHICS_RESOURCE_HEADER
	Public/GraphicsResource/ByteBuffer.h
	Public/GraphicsResource/Canvas.h
	Public/GraphicsResource/CommandList.h
	Public/GraphicsResource/ComputePipelineState.h
	Public/GraphicsResource/ConstantBuffer.h
	Public/GraphicsResource/DDSTexture.h
	Public/GraphicsResource/DrawSnapshot.h
	Public/GraphicsResource/GlobalShaders.h
	Public/GraphicsResource/GraphicsPipelineState.h
	Public/GraphicsResource/IndexBuffer.h
	Public/GraphicsResource/PipelineStateCache.h
	Public/GraphicsResource/RenderOption.h
	Public/GraphicsResource/Shader.h
	Public/GraphicsResource/ShaderArguments.h
	Public/GraphicsResource/ShaderCache.h
	Public/GraphicsResource/StaticShaderSwitch.h
	Public/GraphicsResource/StaticState.h
	Public/GraphicsResource/TypedBuffer.h
	Public/GraphicsResource/UberShader.h
	Public/GraphicsResource/UploadBuffer.h
	Public/GraphicsResource/VertexBuffer.h
	Public/GraphicsResource/VertexLayout.h
	Public/GraphicsResource/VertexBufferBundle.h
	Public/GraphicsResource/Viewport.h)

set (GRAPHICS_RESOURCE_SOURCE
	Private/GraphicsResource/ByteBuffer.cpp
	Private/GraphicsResource/Canvas.cpp
	Private/GraphicsResource/CommandList.cpp
	Private/GraphicsResource/ComputePipelineState.cpp
	Private/GraphicsResource/ConstantBuffer.cpp
	Private/GraphicsResource/DDSTexture.cpp
	Private/GraphicsResource/DrawSnapshot.cpp
	Private/GraphicsResource/GlobalShaders.cpp
	Private/GraphicsResource/GraphicsPipelineState.cpp
	Private/GraphicsResource/IndexBuffer.cpp
	Private/GraphicsResource/PipelineStateCache.cpp
	Private/GraphicsResource/RenderOption.cpp
	Private/GraphicsResource/Shader.cpp
	Private/GraphicsResource/ShaderArguments.cpp
	Private/GraphicsResource/ShaderCache.cpp
	Private/GraphicsResource/StaticShaderSwitch.cpp
	Private/GraphicsResource/UberShader.cpp
	Private/GraphicsResource/UploadBuffer.cpp
	Private/GraphicsResource/VertexBuffer.cpp
	Private/GraphicsResource/VertexLayout.cpp
	Private/GraphicsResource/Viewport.cpp)

set (MATERIAL_SOURCE
	Private/Material/Material.cpp
	Private/Material/MaterialResource.cpp
	Public/Material/Material.h
	Public/Material/MaterialResource.h)

set (MESH_SOURCE
	Private/Mesh/BaseMesh.cpp
	Private/Mesh/StaticMesh.cpp
	Private/Mesh/StaticMeshResource.cpp
	Private/Mesh/VertexCollection.cpp
	Public/Mesh/BaseMesh.h
	Public/Mesh/MeshDescription.h
	Public/Mesh/MeshDrawInfo.h
	Public/Mesh/StaticMesh.h
	Public/Mesh/StaticMeshResource.h
	Public/Mesh/VertexCollection.h)

set (PASS_PROCESSOR
	Private/PassProcessor/DefaultPassProcessor.cpp
	Private/PassProcessor/DepthWritePassProcessor.cpp
	Private/PassProcessor/HitProxyPassProcessor.cpp
	Private/PassProcessor/PassProcessor.cpp
	Private/PassProcessor/ShadowDepthPassProcessor.cpp
	Private/PassProcessor/ShadowDrawPassProcessor.cpp
	Private/PassProcessor/VolumetricCloudPassProcessor.cpp
	Private/PassProcessor/VolumetricFogPassProcessor.cpp
	Public/PassProcessor/DefaultPassProcessor.h
	Public/PassProcessor/DepthWritePassProcessor.h
	Public/PassProcessor/HitProxyPassProcessor.h
	Public/PassProcessor/PassProcessor.h
	Public/PassProcessor/ShadowDepthPassProcessor.h
	Public/PassProcessor/ShadowDrawPassProcessor.h
	Public/PassProcessor/VolumetricCloudPassProcessor.h
	Public/PassProcessor/VolumetricFogPassProcessor.h)
	
set (PROXIES_SOURCE
	Private/Proxies/HitProxy.cpp
	Private/Proxies/LightProxy.cpp
	Private/Proxies/PrimitiveProxy.cpp
	Private/Proxies/SkyAtmosphereProxy.cpp
	Private/Proxies/StaticMeshPrimitiveProxy.cpp
	Private/Proxies/TexturedSkyProxy.cpp
	Private/Proxies/VolumetricCloudProxy.cpp
	Private/Proxies/VolumetricFogProxy.cpp
	Public/Proxies/HitProxy.h
	Public/Proxies/LightProxy.h
	Public/Proxies/PrimitiveProxy.h
	Public/Proxies/SkyAtmosphereProxy.h
	Public/Proxies/StaticMeshPrimitiveProxy.h
	Public/Proxies/TexturedSkyProxy.h
	Public/Proxies/VolumetricCloudProxy.h
	Public/Proxies/VolumetricFogProxy.h)

set (RENDER_SCENE_SOURCE
	Private/Scene/LightSceneInfo.cpp
	Private/Scene/PrimitiveSceneInfo.cpp
	Private/Scene/Scene.cpp
	Private/Scene/SceneConstantBuffers.cpp
	Private/Scene/SceneVelocityData.cpp
	Private/Scene/ShadowInfo.cpp
	Private/Scene/VolumetricCloudSceneInfo.cpp
	Private/Scene/VolumetricFogSceneInfo.cpp
	Public/Scene/IScene.h
	Public/Scene/LightSceneInfo.h
	Public/Scene/PrimitiveSceneInfo.h
	Public/Scene/Scene.h
	Public/Scene/SceneConstantBuffers.h
	Public/Scene/SceneVelocityData.h
	Public/Scene/ShadowInfo.h
	Public/Scene/VolumetricCloudSceneInfo.h
	Public/Scene/VolumetricFogSceneInfo.h)

set (RENDER_UTILITY_HEADER
	../Shaders/Public/Atmosphere/AtmosphereConstant.h
	Public/RenderUtility/AbstractGraphicsInterface.h
	Public/RenderUtility/CommonRenderResource.h
	Public/RenderUtility/RenderTargetPool.h
	Public/RenderUtility/ShaderParameterUtils.h
	Public/RenderUtility/ShadowSetup.h
	Public/RenderUtility/ResourceBarrierUtils.h)

set (RENDER_UTILITY_SOURCE
	Private/RenderUtility/AbstractGraphicsInterface.cpp
	Private/RenderUtility/CommonRenderResource.cpp
	Private/RenderUtility/RenderTargetPool.cpp
	Private/RenderUtility/ShadowSetup.cpp
	Private/RenderUtility/ResourceBarrierUtils.cpp)
	
set (RENDERER_SOURCE
	Private/Renderer/ExponentialShadowMapRendering.cpp
	Private/Renderer/ForwardLighting.cpp
	Private/Renderer/ForwardRenderer.cpp
	Private/Renderer/ImageBasedLightingRendering.cpp
	Private/Renderer/ImguiRenderer.cpp
	Private/Renderer/LightPropagationVolume.cpp
	Private/Renderer/OcclusionRendering.cpp
	Private/Renderer/ReflectiveShadowMapRendering.cpp
	Private/Renderer/RenderView.cpp
	Private/Renderer/SceneRenderer.cpp
	Private/Renderer/SkyAtmosphereRendering.cpp
	Private/Renderer/TemporalAntiAliasingRendering.cpp
	Public/Renderer/ExponentialShadowMapRendering.h
	Public/Renderer/ForwardLighting.h
	Public/Renderer/ForwardRenderer.h
	Public/Renderer/ImageBasedLightingRendering.h
	Public/Renderer/IRenderCore.h
	Public/Renderer/LightPropagationVolume.h
	Public/Renderer/OcclusionRendering.h
	Public/Renderer/ReflectiveShadowMapRendering.h
	Public/Renderer/RenderView.h
	Public/Renderer/SceneRenderer.h
	Public/Renderer/SkyAtmosphereRendering.h
	Public/Renderer/TemporalAntiAliasingRendering.h
	Public/Renderer/UserInterfaceRenderer.h)

source_group("Config" FILES ${CONFIG_SOURCE})
source_group("GpuProfiler" FILES ${GPU_PROFILER_SOURCE})
source_group("GraphicsResource" FILES ${GRAPHICS_RESOURCE_HEADER} ${GRAPHICS_RESOURCE_SOURCE})
source_group("Material" FILES ${MATERIAL_SOURCE})
source_group("Mesh" FILES ${MESH_SOURCE})
source_group("PassProcessor" FILES ${PASS_PROCESSOR})
source_group("Proxies" FILES ${PROXIES_SOURCE})
source_group("Renderer" FILES ${RENDERER_SOURCE})
source_group("RenderUtility" FILES ${RENDER_UTILITY_HEADER} ${RENDER_UTILITY_SOURCE})
source_group("Scene" FILES ${RENDER_SCENE_SOURCE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_library(RenderCore SHARED 	${CONFIG_SOURCE}
								${CORE_SOURCE}
								${GPU_PROFILER_SOURCE}
								${GRAPHICS_RESOURCE_HEADER}
								${GRAPHICS_RESOURCE_SOURCE}
								${MATERIAL_SOURCE}
								${MESH_SOURCE}
								${PASS_PROCESSOR}
								${PROXIES_SOURCE}
								${RENDER_SCENE_SOURCE}
								${RENDER_UTILITY_HEADER}
								${RENDER_UTILITY_SOURCE}
								${RENDERER_SOURCE})

target_include_directories(RenderCore
PUBLIC
	./Public
	./Public/Core
	./Public/GraphicsResource
	./Public/Material
	./Public/PassProcessor
	./Public/Renderer
	./Public/RenderUtility
PRIVATE
	../../ThirdParty/PIX/Include
	../Editor/Public
	../Logic/Public
	../Shaders/Public/Atmosphere
	./Public/GpuProfiler
	./Public/Mesh
	./Public/Proxies)

if(WIN32)
target_link_directories(RenderCore
PRIVATE
	../../ThirdParty/PIX/lib/x64
)
endif()

target_link_libraries(RenderCore 
PUBLIC
	Agl
PRIVATE
	Core
	Engine
	Imgui)

target_precompile_headers(RenderCore
PUBLIC
	${GRAPHICS_RESOURCE_HEADER}
PRIVATE
	${RENDER_UTILITY_HEADER})

add_custom_command(TARGET RenderCore POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/RenderCore.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/RenderCore.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/ThirdParty/PIX/bin/x64/WinPixEventRuntime.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/WinPixEventRuntime.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/ThirdParty/PIX/bin/x64/WinPixEventRuntime.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/WinPixEventRuntime.dll")