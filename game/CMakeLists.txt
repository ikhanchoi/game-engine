add_subdirectory(objects)
# add_subdirectory(systems)

add_library(game STATIC world.cpp)
target_include_directories(game PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(game PUBLIC
		core
		game_objects
#		game_systems
)
target_precompile_headers(game PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")