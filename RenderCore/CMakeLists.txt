add_definitions(-DRENDERCORE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)

set (PRECOMPILED
	"Source/RenderOutputManager.cpp"
	"Source/RenderOutputManager.h"
	"Source/stdafx.cpp"
	"Source/stdafx.h"
	"Source/targetver.h")

set (CORE_SOURCE
	"Source/Private/Core/dllmain.cpp"
	"Source/Private/Core/RenderCore.cpp"
	"Source/Public/Core/RenderCoreDllFunc.h")

set (D3D11_RENDER_STATE_SOURCE
	"Source/Public/D3D11/D3D11RenderState.h"
	"Source/Public/D3D11/D3D11RenderStateFactory.h"
	"Source/Public/D3D11/D3D11RenderStateManager.h"
	"Source/Private/D3D11/D3D11RenderStateFactory.cpp"
	"Source/Private/D3D11/D3D11RenderStateManager.cpp")
	
set (D3D11_RENDER_RESOURCE_SOURCE
	"Source/Public/D3D11/D3D11Resource.h"
	"Source/Public/D3D11/D3D11ResourceManager.h"
	"Source/Private/D3D11/D3D11Resource.cpp"
	"Source/Private/D3D11/D3D11ResourceManager.cpp")

set (D3D11_SOURCE
	"Source/Private/D3D11/Direct3D11.cpp")
	
set (RENDER_COMMON_SOURCE
	"Source/Public/Render/IRenderer.h"
	"Source/Public/Render/IRenderResource.h"
	"Source/Public/Render/IRenderResourceManager.h"
	"Source/Public/Render/Resource.h")
	
set (MATERIAL_SOURCE
	"Source/Public/Render/Material.h"
	"Source/Public/Render/MaterialManager.h"
	"Source/Private/Render/Material.cpp"
	"Source/Private/Render/MaterialManager.cpp")
	
source_group("Direct11\\RenderResource" FILES ${D3D11_RENDER_RESOURCE_SOURCE})
source_group("Direct11\\RenderState" FILES ${D3D11_RENDER_STATE_SOURCE})
source_group("Direct11" FILES ${D3D11_SOURCE})
source_group("Material" FILES ${MATERIAL_SOURCE})
source_group("RenderCommon" FILES ${RENDER_COMMON_SOURCE})

include_directories(./Source)
include_directories(./Source/Public)
include_directories(../Shared/Public)
include_directories(../Engine/Source/Public)
include_directories($ENV{DXSDK_DIR}include)

if (NOT "${CMAKE_GENERATOR}" MATCHES "(WIN64|IA64)")
link_directories($ENV{DXSDK_DIR}lib/x86)
else()
link_directories($ENV{DXSDK_DIR}lib/x64)
endif()

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

add_library(RenderCore SHARED 	${PRECOMPILED}
								${CORE_SOURCE}
								${D3D11_RENDER_RESOURCE_SOURCE}
								${D3D11_RENDER_STATE_SOURCE}
								${D3D11_SOURCE}
								${MATERIAL_SOURCE}
								${RENDER_COMMON_SOURCE})

set (DIRECT3D_LIBRARY
	"d3d11.lib"
	"d3dx11.lib"
	"d3dx9.lib"
	"d3dx10.lib"
	"dxguid.lib"
	"dxgi.lib")
								
target_link_libraries(RenderCore PRIVATE ${DIRECT3D_LIBRARY})
								
add_custom_command(TARGET RenderCore POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/\$\(Configuration\)/RenderCore.lib" "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/\$\(Configuration\)/RenderCore.dll" "${CMAKE_CURRENT_SOURCE_DIR}/../bin")