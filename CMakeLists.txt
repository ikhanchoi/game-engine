cmake_minimum_required(VERSION 3.27)
project(game_engine)

set(CMAKE_CXX_STANDARD 17)

# glew, glfw, glm
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# imgui
add_library(imgui STATIC
    include/imgui/imgui.cpp
	include/imgui/imgui_demo.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/backends/imgui_impl_glfw.cpp
    include/imgui/backends/imgui_impl_opengl3.cpp
    include/imgui/misc/cpp/imgui_stdlib.cpp)
target_include_directories(imgui PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include/imgui
		${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends)
target_link_libraries(imgui PRIVATE glfw)

# tiny_gltf
add_library(tiny_gltf INTERFACE)
target_include_directories(tiny_gltf INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/include/tiny_gltf)

# modules
add_library(modules STATIC
	modules/resource.cpp
	modules/component.cpp
	modules/entity.cpp
	modules/event.cpp
	modules/window.cpp
		modules/core.cpp
		modules/core.h)
target_include_directories(modules PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(modules PUBLIC GLEW::GLEW glfw imgui tiny_gltf)

# systems
add_library(systems STATIC
	systems/render.cpp
		)
target_include_directories(systems PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(systems PUBLIC GLEW::GLEW glfw imgui tiny_gltf)

# executable
add_executable(game_engine src/main.cpp)
target_link_libraries(game_engine PRIVATE modules systems)

