cmake_minimum_required(VERSION 3.21)

project (ScreenSpaceReflection LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

add_subdirectory(Source/Agl)
add_subdirectory(Source/AssetManufacture)
add_subdirectory(Source/Core)
add_subdirectory(Source/Engine)
add_subdirectory(Source/RenderCore)
add_subdirectory(Source/Logic)
add_subdirectory(Source/ScreenSpaceReflection)

option(BUILD_TOOLS "I don't need command-line tools" OFF)
add_subdirectory(ThirdParty/DirectXTex)
