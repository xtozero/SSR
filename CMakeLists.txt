cmake_minimum_required (VERSION 3.8.2)

project ("Screen Space Reflection")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /std:c++17")

add_subdirectory (Engine)
add_subdirectory (RenderCore)
add_subdirectory (Logic)
add_subdirectory ("Sceen Space Reflect")

add_dependencies(RenderCore Engine)
add_dependencies(Logic Engine)
add_dependencies("ScreenSpaceReflect" Engine RenderCore Logic)