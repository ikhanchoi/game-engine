add_library(core INTERFACE)
target_include_directories(core INTERFACE ${CMAKE_SOURCE_DIR})
target_precompile_headers(core INTERFACE "${CMAKE_SOURCE_DIR}/include/pch.h")