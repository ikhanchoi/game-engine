add_library(scene INTERFACE)
target_include_directories(scene INTERFACE ${CMAKE_SOURCE_DIR})
target_precompile_headers(scene INTERFACE "${CMAKE_SOURCE_DIR}/include/pch.h")