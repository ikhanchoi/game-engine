add_subdirectory(scene)
add_subdirectory(entity)
add_subdirectory(component)
add_subdirectory(resource)

add_library(world STATIC world.cpp)
target_include_directories(world PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(world PUBLIC
		core
		scene
		entity
		component
		resource
)
target_precompile_headers(world PUBLIC
		"${CMAKE_SOURCE_DIR}/include/pch.h")