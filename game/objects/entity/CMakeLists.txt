add_library(entity INTERFACE)
target_include_directories(entity INTERFACE ${CMAKE_SOURCE_DIR})
target_precompile_headers(entity INTERFACE "${CMAKE_SOURCE_DIR}/include/pch.h")