
add_subdirectory(render)
# add_subdirectory( {systems} )

add_library(runtime STATIC render/render_system.cpp)
target_include_directories(runtime PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(runtime PUBLIC
		core mesh_render
)
target_precompile_headers(runtime PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")