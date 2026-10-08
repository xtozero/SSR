cmake_minimum_required(VERSION 3.21)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project (ScreenSpaceReflection LANGUAGES CXX)

# Usage:
# Enable editor features:
#   cmake .. -DENABLE_EDITOR=ON
# Disable editor features:
#   cmake .. -DENABLE_EDITOR=OFF
option(ENABLE_EDITOR "Enable editor features (independent of build type)" OFF)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

find_package(Vulkan QUIET)

add_subdirectory(Source/Agl)
add_subdirectory(Source/AssetBuilder)
add_subdirectory(Source/Core)
add_subdirectory(Source/Editor)
add_subdirectory(Source/Engine)
add_subdirectory(Source/RenderCore)
add_subdirectory(Source/Imgui)
add_subdirectory(Source/Logic)
add_subdirectory(Source/ScreenSpaceReflection)

option(BUILD_TOOLS "I don't need command-line tools" OFF)
add_subdirectory(ThirdParty/DirectXTex)

add_subdirectory(ThirdParty/MeshOptimizer)

add_subdirectory(ThirdParty/DirectXHeaders)

if(Vulkan_FOUND)
# Only spirv-cross-core is used by Agl; all optional SPIRV-Cross components are disabled.
option(SPIRV_CROSS_CLI "I don't need the SPIRV-Cross command-line tools" OFF)
option(SPIRV_CROSS_ENABLE_GLSL "I don't need GLSL backend support" OFF)
option(SPIRV_CROSS_ENABLE_HLSL "I don't need HLSL backend support" OFF)
option(SPIRV_CROSS_ENABLE_MSL "I don't need MSL backend support" OFF)
option(SPIRV_CROSS_ENABLE_CPP "I don't need C++ backend support" OFF)
option(SPIRV_CROSS_ENABLE_REFLECT "I don't need JSON reflection support" OFF)
option(SPIRV_CROSS_ENABLE_C_API "I don't need the C API wrapper" OFF)
option(SPIRV_CROSS_ENABLE_UTIL "I don't need the SPIRV-Cross utility module" OFF)

add_subdirectory(ThirdParty/SPIRV-Cross)
set_target_properties(spirv-cross-core PROPERTIES FOLDER ThirdParty)
endif()

set_target_properties(ddsview PROPERTIES FOLDER ThirdParty)
set_target_properties(DirectX-Guids PROPERTIES FOLDER ThirdParty)
set_target_properties(DirectX-Headers PROPERTIES FOLDER ThirdParty)
set_target_properties(DirectXTex PROPERTIES FOLDER ThirdParty)
set_target_properties(meshoptimizer PROPERTIES FOLDER ThirdParty)