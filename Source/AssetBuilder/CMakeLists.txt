set (ENTRY_POINT
	Private/main.cpp
)

set (BUILDER
	Public/AssetBuilder.h
	Public/EngineDefaultBuilder.h
	Public/TextureBuilder.h
	Public/JsonBuilder.h
	Public/AssetBuilderConfig.h
	Public/ShaderBuilder.h
	Public/WavefrontObjBuilder.h
	Public/WavefrontObjParser.hpp
	Private/AssetBuilder.cpp
	Private/EngineDefaultBuilder.cpp
	Private/TextureBuilder.cpp
	Private/JsonBuilder.cpp
	Private/ShaderBuilder.cpp
	Private/WavefrontObjBuilder.cpp
)

set (SHADER_TOOL
	Public/ShaderFileMerger.h
	Public/ShaderTool.h
	Private/ShaderFileMerger.cpp
	Private/ShaderTool.cpp
)

set (UTIL
	Public/DirectXTexTool.h
	Private/DirectXTexTool.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(AssetBuilder		${BUILDER}
                                ${ENTRY_POINT}
								${SHADER_TOOL}
								${UTIL})

target_link_directories(AssetBuilder
PRIVATE
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_include_directories(AssetBuilder
PRIVATE
	./Public)

target_link_libraries(AssetBuilder
PRIVATE
	Core
	DirectXTex
	Engine
	Logic
	RenderCore
	d3dcompiler
	dxcompiler
	meshoptimizer)

set_target_properties(AssetBuilder PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/AssetBuilder")
