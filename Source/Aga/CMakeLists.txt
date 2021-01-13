add_definitions(-DAGA_EXPORTS=1)
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
	"Private/D3D11/D3D11BlendState.cpp"
	"Private/D3D11/D3D11Buffer.cpp"
	"Private/D3D11/D3D11DepthStencil.cpp"
	"Private/D3D11/D3D11DepthStencilState.cpp"
	"Private/D3D11/D3D11RandomAccessResource.cpp"
	"Private/D3D11/D3D11RasterizerState.cpp"
	"Private/D3D11/D3D11RenderTarget.cpp"
	"Private/D3D11/D3D11ResourceManager.cpp"
	"Private/D3D11/D3D11SamplerState.cpp"
	"Private/D3D11/D3D11ShaderResource.cpp"
	"Private/D3D11/D3D11Shaders.cpp"
	"Private/D3D11/D3D11Textures.cpp"
	"Private/D3D11/D3D11VetexLayout.cpp"
	"Public/D3D11/D3D11BlendState.h"
	"Public/D3D11/D3D11Buffer.h"
	"Public/D3D11/D3D11DepthStencil.h"
	"Public/D3D11/D3D11DepthStencilState.h"
	"Public/D3D11/D3D11RandomAccessResource.h"
	"Public/D3D11/D3D11RasterizerState.h"
	"Public/D3D11/D3D11RenderTarget.h"
	"Public/D3D11/D3D11ResourceInterface.h"
	"Public/D3D11/D3D11ResourceManager.h"
	"Public/D3D11/D3D11SamplerState.h"
	"Public/D3D11/D3D11ShaderResource.h"
	"Public/D3D11/D3D11Shaders.h"
	"Public/D3D11/D3D11Textures.h"
	"Public/D3D11/D3D11VetexLayout.h")

set (D3D11_SOURCE
	"Private/D3D11/D3D11Api.cpp"
	"Public/D3D11/D3D11Api.h")
	
set (D3D11_UTILITY
	"Public/D3D11/D3D11FlagConvertor.h")
	
set (COMMON_SOURCE
	"Private/Common/GraphicsApiResource.cpp"
	"Public/Common/GraphicsApiResource.h"
	"Public/Common/IAga.h"
	"Public/Common/IRenderResourceManager.h"
	"Public/Common/RefHandle.h"
	"Public/Common/ShaderPrameterInfo.h"
	"Public/Common/ShaderPrameterMap.h")

set (RENDER_RESOURCE_SOURCE
	"Private/RenderResource/ConstantBuffer.cpp"
	"Private/RenderResource/IndexBuffer.cpp"
	"Private/RenderResource/ShaderResource.cpp"
	"Private/RenderResource/VertexBuffer.cpp"
	"Public/RenderResource/ConstantBuffer.h"
	"Public/RenderResource/IndexBuffer.h"
	"Public/RenderResource/ShaderResource.h"
	"Public/RenderResource/VertexBuffer.h"
	"Public/RenderResource/VertexInputLayout.h")

source_group("Direct11" FILES ${D3D11_SOURCE})	
source_group("Direct11\\RenderResource" FILES ${D3D11_RENDER_RESOURCE_SOURCE})
source_group("Direct11\\Utility" FILES ${D3D11_UTILITY})
source_group("RenderCommon" FILES ${COMMON_SOURCE})
source_group("RenderResource" FILES ${RENDER_RESOURCE_SOURCE})

include_directories(./Public)
include_directories(./Public/Common)
include_directories(./Public/D3D11)
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

add_library(Aga SHARED 	${COMMON_SOURCE}
						${CORE_SOURCE}
						${D3D11_RENDER_RESOURCE_SOURCE}
						${D3D11_SOURCE}
						${D3D11_UTILITY}
						${PRECOMPILED}
						${RENDER_RESOURCE_SOURCE})

target_include_directories( Aga
	PUBLIC
		./Public/RenderResource
)

set (DIRECT3D_LIBRARY
	"d3d11.lib"
	"d3dcompiler.lib"
	"d3dx11.lib"
	"d3dx9.lib"
	"d3dx10.lib"
	"dxguid.lib"
	"dxgi.lib")
								
target_link_libraries(Aga PRIVATE ${DIRECT3D_LIBRARY})

add_custom_command(TARGET Aga POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/Aga.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/Aga.dll")

if (WIN32)
	add_custom_command(TARGET Aga POST_BUILD
	COMMAND cmd /c call ${CMAKE_SOURCE_DIR}/Source/Shaders/make_shader.bat ${CMAKE_SOURCE_DIR}/Program/Shaders)
endif()