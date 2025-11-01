add_library(component INTERFACE)
target_include_directories(component INTERFACE ${CMAKE_SOURCE_DIR})
target_precompile_headers(component INTERFACE "${CMAKE_SOURCE_DIR}/include/pch.h")