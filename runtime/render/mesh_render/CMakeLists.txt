add_library(mesh_render STATIC mesh_render_system.cpp)
target_link_libraries(mesh_render PUBLIC GLEW::GLEW)
target_include_directories(mesh_render PUBLIC ${CMAKE_SOURCE_DIR})
target_precompile_headers(mesh_render PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")