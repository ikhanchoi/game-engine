add_library(component STATIC component_manager.cpp)
target_include_directories(component PUBLIC ${CMAKE_SOURCE_DIR})
target_precompile_headers(component PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")