add_definitions(-DAGL_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (COMMON_SOURCE
	Private/Common/GraphicsApiResource.cpp
	Public/Common/GraphicsApiResource.h
	Public/Common/IAgl.h
	Public/Common/ICommandList.h
	Public/Common/IRenderResourceManager.h
	Public/Common/RefHandle.h)

set (CONFIG_SOURCE
	Private/Config/DefaultAglConfig.cpp
	Public/Config/DefaultAglConfig.h)

set (CORE_SOURCE
	Private/Core/Agl.cpp
	Private/Core/dllmain.cpp
	Private/Core/ModuleInit.cpp)
	
set (D3D11_RENDER_RESOURCE_SOURCE
	Private/D3D11/D3D11BaseTexture.cpp
	Private/D3D11/D3D11BlendState.cpp
	Private/D3D11/D3D11Buffer.cpp
	Private/D3D11/D3D11CommandList.cpp
	Private/D3D11/D3D11DepthStencilState.cpp
	Private/D3D11/D3D11PipelineState.cpp
	Private/D3D11/D3D11RasterizerState.cpp
	Private/D3D11/D3D11ResourceManager.cpp
	Private/D3D11/D3D11ResourceViews.cpp
	Private/D3D11/D3D11SamplerState.cpp
	Private/D3D11/D3D11Shaders.cpp
	Private/D3D11/D3D11StateCache.cpp
	Private/D3D11/D3D11VetexLayout.cpp
	Private/D3D11/D3D11Viewport.cpp
	Public/D3D11/D3D11BaseTexture.h
	Public/D3D11/D3D11BlendState.h
	Public/D3D11/D3D11Buffer.h
	Public/D3D11/D3D11CommandList.h
	Public/D3D11/D3D11DepthStencilState.h
	Public/D3D11/D3D11PipelineState.h
	Public/D3D11/D3D11RasterizerState.h
	Public/D3D11/D3D11ResourceManager.h
	Public/D3D11/D3D11ResourceViews.h
	Public/D3D11/D3D11SamplerState.h
	Public/D3D11/D3D11Shaders.h
	Public/D3D11/D3D11StateCache.h
	Public/D3D11/D3D11VetexLayout.h
	Public/D3D11/D3D11Viewport.h)

set (D3D11_SOURCE
	Private/D3D11/D3D11Api.cpp
	Public/D3D11/D3D11Api.h)
	
set (D3D11_UTILITY
	Public/D3D11/D3D11FlagConvertor.h)

set (D3D12_RENDER_RESOURCE_SOURCE
	Private/D3D12/D3D12BaseTexture.cpp
	Private/D3D12/D3D12BlendState.cpp
	Private/D3D12/D3D12Buffer.cpp
	Private/D3D12/D3D12CommandList.cpp
	Private/D3D12/D3D12DepthStencilState.cpp
	Private/D3D12/D3D12DescriptorHeapAllocator.cpp
	Private/D3D12/D3D12GlobalDescriptorHeap.cpp
	Private/D3D12/D3D12HeapAllocator.cpp
	Private/D3D12/D3D12PipelineState.cpp
	Private/D3D12/D3D12RasterizerState.cpp
	Private/D3D12/D3D12ResourceAllocator.cpp
	Private/D3D12/D3D12ResourceManager.cpp
	Private/D3D12/D3D12ResourceUploader.cpp
	Private/D3D12/D3D12ResourceViews.cpp
	Private/D3D12/D3D12RootSignature.cpp
	Private/D3D12/D3D12SamplerState.cpp
	Private/D3D12/D3D12Shaders.cpp
	Private/D3D12/D3D12VertexLayout.cpp
	Private/D3D12/D3D12Viewport.cpp
	Public/D3D12/D3D12BaseTexture.h
	Public/D3D12/D3D12BlendState.h
	Public/D3D12/D3D12Buffer.h
	Public/D3D12/D3D12CommandList.h
	Public/D3D12/D3D12DepthStencilState.h
	Public/D3D12/D3D12DescriptorHeapAllocator.h
	Public/D3D12/D3D12GlobalDescriptorHeap.h
	Public/D3D12/D3D12HeapAllocator.h
	Public/D3D12/D3D12PipelineState.h
	Public/D3D12/D3D12RasterizerState.h
	Public/D3D12/D3D12ResourceAllocator.h
	Public/D3D12/D3D12ResourceManager.h
	Public/D3D12/D3D12ResourceUploader.h
	Public/D3D12/D3D12ResourceViews.h
	Public/D3D12/D3D12RootSignature.h
	Public/D3D12/D3D12SamplerState.h
	Public/D3D12/D3D12Shaders.h
	Public/D3D12/D3D12VertexLayout.h
	Public/D3D12/D3D12Viewport.h)

set (D3D12_SOURCE
	Private/D3D12/D3D12Api.cpp
	Public/D3D12/D3D12Api.h)

set (D3D12_UTILITY
	Public/D3D12/D3D12FlagConvertor.h)

set (DXC_SOURCE
	../../ThirdParty/DXC/inc/d3d12shader.h
	../../ThirdParty/DXC/inc/dxcapi.h
	../../ThirdParty/DXC/inc/dxcerrors.h
	../../ThirdParty/DXC/inc/dxcisense.h)

set (DXGI_SOURCE
	Public/DXGI/DxgiFlagConvertor.h)

set (PRECOMPILED
	Private/Pch/stdafx.cpp
	Public/Pch/stdafx.h
	Public/Pch/targetver.h)

set (RENDER_RESOURCE_SOURCE
	Private/RenderResource/Buffer.cpp
	Private/RenderResource/GlobalConstantBuffers.cpp
	Private/RenderResource/PipelineState.cpp
	Private/RenderResource/ResourceViews.cpp
	Private/RenderResource/ShaderParameterMap.cpp
	Private/RenderResource/ShaderResource.cpp
	Private/RenderResource/Texture.cpp
	Private/RenderResource/Viewport.cpp
	Public/RenderResource/Buffer.h
	Public/RenderResource/GlobalConstantBuffers.h
	Public/RenderResource/PipelineState.h
	Public/RenderResource/ResourceViews.h
	Public/RenderResource/ShaderBindings.h
	Public/RenderResource/ShaderParameterInfo.h
	Public/RenderResource/ShaderParameterMap.h
	Public/RenderResource/ShaderResource.h
	Public/RenderResource/Texture.h
	Public/RenderResource/Viewport.h)

source_group("Config" FILES ${CONFIG_SOURCE})
source_group("Direct11\\RenderResource" FILES ${D3D11_RENDER_RESOURCE_SOURCE})
source_group("Direct11" FILES ${D3D11_SOURCE})	
source_group("Direct11\\Utility" FILES ${D3D11_UTILITY})
source_group("Direct12\\RenderResource" FILES ${D3D12_RENDER_RESOURCE_SOURCE})
source_group("Direct12" FILES ${D3D12_SOURCE})
source_group("Direct12\\Utility" FILES ${D3D12_UTILITY})
source_group("DXC" FILES ${DXC_SOURCE})
source_group("DXGI" FILES ${DXGI_SOURCE})
source_group("RenderCommon" FILES ${COMMON_SOURCE})
source_group("RenderResource" FILES ${RENDER_RESOURCE_SOURCE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_library(Agl SHARED 	${CONFIG_SOURCE}
						${COMMON_SOURCE}
						${CORE_SOURCE}
						${D3D11_RENDER_RESOURCE_SOURCE}
						${D3D11_SOURCE}
						${D3D11_UTILITY}
						${D3D12_RENDER_RESOURCE_SOURCE}
						${D3D12_SOURCE}
						${D3D12_UTILITY}
						${DXC_SOURCE}
						${DXGI_SOURCE}
						${PRECOMPILED}
						${RENDER_RESOURCE_SOURCE})

target_include_directories(Agl
PUBLIC
	./Public/RenderResource
	./Public/Common
PRIVATE
	./../ThirdParty/inc
	./Public
	./Public/D3D11
	./Public/D3D12
	./Public/DXGI
	./Public/Pch)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
target_link_directories(Agl
PRIVATE
	./../ThirdParty/DXC/lib/x64
	$ENV{DXSDK_DIR}lib/x64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
target_link_directories(Agl
PRIVATE
	./../ThirdParty/DXC/lib/x86
	$ENV{DXSDK_DIR}lib/x86)
endif()

set (DIRECT3D_LIBRARY
	"d3d11.lib"
	"d3d12.lib"
	"d3dcompiler.lib"
	"dxcompiler.lib"
	"dxguid.lib"
	"dxgi.lib")
								
target_link_libraries(Agl
PRIVATE 
	Core
	${DIRECT3D_LIBRARY})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
add_custom_command(TARGET Agl POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/Agl.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/Agl.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/ThirdParty/DXC/bin/x64/dxcompiler.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/dxcompiler.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/ThirdParty/DXC/bin/x64/dxil.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/dxil.dll")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
add_custom_command(TARGET Agl POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/Agl.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/Agl.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/ThirdParty/DXC/bin/x86/dxcompiler.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/dxcompiler.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/ThirdParty/DXC/bin/x86/dxil.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/dxil.dll")
endif()
