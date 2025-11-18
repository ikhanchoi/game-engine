add_library(resource STATIC resource_manager.cpp)
target_include_directories(resource PUBLIC ${CMAKE_SOURCE_DIR})
target_precompile_headers(resource PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")