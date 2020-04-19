add_definitions(-DRENDERCORE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (PRECOMPILED
	"Private/stdafx.cpp"
	"Public/stdafx.h"
	"Public/targetver.h")

set (CORE_SOURCE
	"Private/Core/Aga.cpp"
	"Private/Core/dllmain.cpp"
	"Private/Core/ModuleInit.cpp")
	
set (D3D11_RENDER_RESOURCE_SOURCE
	"Private/D3D11/D3D11Resource.cpp"
	"Private/D3D11/D3D11ResourceManager.cpp"
	"Public/D3D11/D3D11Resource.h"
	"Public/D3D11/D3D11ResourceManager.h")

set (D3D11_SOURCE
	"Private/D3D11/Direct3D11.cpp"
	"Public/D3D11/Direct3D11.h")
	
set (RENDER_COMMON_SOURCE
	"Public/Render/IRenderer.h"
	"Public/Render/IRenderResource.h"
	"Public/Render/IRenderResourceManager.h"
	"Public/Render/Resource.h")
	
source_group("Direct11\\RenderResource" FILES ${D3D11_RENDER_RESOURCE_SOURCE})
source_group("Direct11" FILES ${D3D11_SOURCE})
source_group("RenderCommon" FILES ${RENDER_COMMON_SOURCE})

include_directories(./Public)
include_directories(../Shared/Public)
include_directories(../Engine/Public)
include_directories($ENV{DXSDK_DIR}include)

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
link_directories($ENV{DXSDK_DIR}lib/x86)
else()
link_directories($ENV{DXSDK_DIR}lib/x64)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_library(Aga SHARED 	${PRECOMPILED}
						${CORE_SOURCE}
						${D3D11_RENDER_RESOURCE_SOURCE}
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
								
target_link_libraries(Aga PRIVATE ${DIRECT3D_LIBRARY})

add_custom_command(TARGET Aga POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/Aga.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/Aga.dll")
