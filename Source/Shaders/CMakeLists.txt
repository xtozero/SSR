add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_library(BuildShader STATIC dummy.cpp)

if (WIN32)
	add_custom_command(TARGET BuildShader PRE_LINK
	COMMAND cmd /c call ${CMAKE_SOURCE_DIR}/Source/Shaders/make_shader.bat)
endif()