add_library(entity STATIC
		entity_manager.cpp
)
target_include_directories(entity PUBLIC ${CMAKE_SOURCE_DIR})
target_precompile_headers(entity PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")