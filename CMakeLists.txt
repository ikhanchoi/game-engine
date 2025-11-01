cmake_minimum_required(VERSION 3.27)
project(game_engine)
set(CMAKE_CXX_STANDARD 20)

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

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
target_link_libraries(imgui PUBLIC glfw)

add_library(tiny_gltf INTERFACE)
target_include_directories(tiny_gltf INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/include/tiny_gltf)

add_subdirectory(main)
add_subdirectory(core)
add_subdirectory(runtime)
add_subdirectory(editor)
add_subdirectory(world)
# add_subdirectory(tests)

