add_library(scene STATIC
		scene_manager.cpp)
target_include_directories(scene PUBLIC ${CMAKE_SOURCE_DIR})
target_precompile_headers(scene PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")