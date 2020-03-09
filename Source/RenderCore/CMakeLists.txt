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
	"Private/Core/RenderCore.cpp"
	"Public/Core/RenderCoreDllFunc.h")

set (D3D11_RENDER_STATE_SOURCE
	"Private/D3D11/D3D11RenderStateFactory.cpp"
	"Private/D3D11/D3D11RenderStateManager.cpp"
	"Public/D3D11/D3D11RenderState.h"
	"Public/D3D11/D3D11RenderStateFactory.h"
	"Public/D3D11/D3D11RenderStateManager.h")
	
set (D3D11_RENDER_RESOURCE_SOURCE
	"Private/D3D11/D3D11Resource.cpp"
	"Private/D3D11/D3D11ResourceManager.cpp"
	"Public/D3D11/D3D11Resource.h"
	"Public/D3D11/D3D11ResourceManager.h")

set (D3D11_SOURCE
	"Private/D3D11/Direct3D11.cpp")
	
set (MATERIAL_SOURCE
	"Private/Render/Material.cpp"
	"Private/Render/MaterialManager.cpp"
	"Public/Render/Material.h"
	"Public/Render/MaterialManager.h")
	
set (RENDER_COMMON_SOURCE
	"Public/Render/IRenderer.h"
	"Public/Render/IRenderResource.h"
	"Public/Render/IRenderResourceManager.h"
	"Public/Render/Resource.h")

set (RENDER_UTIL
	"Private/Render/RenderOutputManager.cpp"
	"Public/Render/RenderOutputManager.h")
	
source_group("Direct11\\RenderResource" FILES ${D3D11_RENDER_RESOURCE_SOURCE})
source_group("Direct11\\RenderState" FILES ${D3D11_RENDER_STATE_SOURCE})
source_group("Direct11" FILES ${D3D11_SOURCE})
source_group("Material" FILES ${MATERIAL_SOURCE})
source_group("RenderCommon" FILES ${RENDER_COMMON_SOURCE})
source_group("RenderUtil" FILES ${RENDER_UTIL})

include_directories(./Public)
include_directories(../Shared/Public)
include_directories(../Engine/Public)
include_directories($ENV{DXSDK_DIR}include)

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
link_directories($ENV{DXSDK_DIR}lib/x86)
else()
link_directories($ENV{DXSDK_DIR}lib/x64)
endif()

link_directories(${CMAKE_SOURCE_DIR}/Libraries)

add_library(RenderCore SHARED 	${PRECOMPILED}
								${CORE_SOURCE}
								${D3D11_RENDER_RESOURCE_SOURCE}
								${D3D11_RENDER_STATE_SOURCE}
								${D3D11_SOURCE}
								${MATERIAL_SOURCE}
								${RENDER_COMMON_SOURCE}
								${RENDER_UTIL})

set (DIRECT3D_LIBRARY
	"d3d11.lib"
	"d3dx11.lib"
	"d3dx9.lib"
	"d3dx10.lib"
	"dxguid.lib"
	"dxgi.lib")
								
target_link_libraries(RenderCore PRIVATE ${DIRECT3D_LIBRARY})

add_custom_command(TARGET RenderCore POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/\$\(Configuration\)/RenderCore.lib" "${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries/RenderCore.lib"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/\$\(Configuration\)/RenderCore.dll" "${CMAKE_CURRENT_SOURCE_DIR}/../../Program/Binaries/RenderCore.dll")

if (WIN32)
	add_custom_command(TARGET RenderCore POST_BUILD
	COMMAND cmd /c call ${CMAKE_SOURCE_DIR}/Source/Shaders/make_shader.bat ${CMAKE_SOURCE_DIR}/Program/Shaders)
endif()