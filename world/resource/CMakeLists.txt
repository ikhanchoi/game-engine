add_library(resource INTERFACE)
target_link_libraries(resource INTERFACE GLEW::GLEW tiny_gltf)
target_include_directories(resource INTERFACE ${CMAKE_SOURCE_DIR})
target_precompile_headers(resource INTERFACE "${CMAKE_SOURCE_DIR}/include/pch.h")