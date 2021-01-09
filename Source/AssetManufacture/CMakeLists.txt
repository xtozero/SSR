set (ARCHIVE
	../Core/Public/Archive.h
	../Core/Public/ArchiveUtility.h
)

set (ENTRY_POINT
	Private/main.cpp
)

set (MANUFACTURER
	Public/AssetManufacturer.h
	Public/JsonManufacturer.h
	Public/ShaderManufacturer.h
	Public/WavefrontObjManufacturer.h
	Public/WavefrontObjParser.hpp
	Private/AssetManufacturer.cpp
	Private/JsonManufacturer.cpp
	Private/ShaderManufacturer.cpp
	Private/WavefrontObjManufacturer.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(AssetManufacture		${ARCHIVE}
									${ENTRY_POINT}
									${MANUFACTURER})

target_include_directories(AssetManufacture
PRIVATE
	../Core/Public
	../Logic/Public
	../Shared/Public
	../Aga/Public/Common
	../Engine/Public
	Public)

target_link_libraries(AssetManufacture
PRIVATE 
	Logic
	RenderCore
	d3dcompiler)