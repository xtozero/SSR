cmake_minimum_required (VERSION 3.8.2)

project ("Screen Space Reflection")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /std:c++17")

add_subdirectory (Source/Engine)
add_subdirectory (Source/RenderCore)
add_subdirectory (Source/Logic)
add_subdirectory ("Source/Sceen Space Reflect")

add_dependencies(RenderCore Engine)
add_dependencies(Logic Engine)