add_library(resources INTERFACE)
target_link_libraries(resources INTERFACE GLEW::GLEW tiny_gltf)
target_include_directories(resources INTERFACE ${CMAKE_SOURCE_DIR})
target_precompile_headers(resources INTERFACE "${CMAKE_SOURCE_DIR}/include/pch.h")