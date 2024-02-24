set (ENTRY_POINT
	Private/main.cpp
)

set (MANUFACTURER
	Public/AssetManufacturer.h
	Public/TextureManufacturer.h
	Public/JsonManufacturer.h
	Public/ManufactureConfig.h
	Public/ShaderManufacturer.h
	Public/WavefrontObjManufacturer.h
	Public/WavefrontObjParser.hpp
	Private/AssetManufacturer.cpp
	Private/TextureManufacturer.cpp
	Private/JsonManufacturer.cpp
	Private/ShaderManufacturer.cpp
	Private/WavefrontObjManufacturer.cpp
)

set (SHADER
	Public/ShaderFileMerger.h
	Public/ShaderTool.h
	Private/ShaderFileMerger.cpp
	Private/ShaderTool.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(AssetManufacture		${ENTRY_POINT}
									${MANUFACTURER}
									${SHADER})

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
	d3dcompiler)

set_target_properties( AssetManufacture PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/AssetManufacture" )