set (ENTRY_POINT
	Private/main.cpp
)

set (MANUFACTURER
	Public/AssetManufacturer.h
	Public/EngineDefaultManufacturer.h
	Public/TextureManufacturer.h
	Public/JsonManufacturer.h
	Public/ManufactureConfig.h
	Public/ShaderManufacturer.h
	Public/WavefrontObjManufacturer.h
	Public/WavefrontObjParser.hpp
	Private/AssetManufacturer.cpp
	Private/EngineDefaultManufacturer.cpp
	Private/TextureManufacturer.cpp
	Private/JsonManufacturer.cpp
	Private/ShaderManufacturer.cpp
	Private/WavefrontObjManufacturer.cpp
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

add_executable(AssetManufacture		${ENTRY_POINT}
									${MANUFACTURER}
									${SHADER_TOOL}
									${UTIL})

target_link_directories(AssetManufacture
PRIVATE
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_include_directories(AssetManufacture
PRIVATE
	./Public)

target_link_libraries(AssetManufacture
PRIVATE
	Core
	DirectXTex
	Engine
	Logic
	RenderCore
	d3dcompiler
	dxcompiler
	meshoptimizer)

set_target_properties( AssetManufacture PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/AssetManufacture" )
