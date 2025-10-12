add_subdirectory(scene)
add_subdirectory(entity)
# add_subdirectory(components)
add_subdirectory(resources)

file(GLOB_RECURSE GAME_OBJECTS_CPP "*.cpp")
add_library(game_objects STATIC ${GAME_OBJECTS_CPP})
target_include_directories(game_objects PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(game_objects PUBLIC
		core
		scene
		entity
#		components
		resources
)
target_precompile_headers(game_objects PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")