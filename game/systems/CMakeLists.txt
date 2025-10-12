# add_subdirectory(render)
# add_subdirectory(input)

file(GLOB_RECURSE GAME_SYSTEMS_CPP "*.cpp")
add_library(game_systems STATIC ${GAME_SYSTEMS_CPP})
target_link_libraries(game_systems PUBLIC
		core
#		render
#		input
)
target_precompile_headers(game_systems PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")