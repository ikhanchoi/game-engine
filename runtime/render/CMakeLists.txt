add_subdirectory(mesh_render)
add_library(render STATIC render_system.cpp)
target_include_directories(render PUBLIC ${CMAKE_SOURCE_DIR})
target_precompile_headers(render PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")