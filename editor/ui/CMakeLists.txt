add_library(ui STATIC
		ui_system.cpp
		panel/hierarchy/hierarchy_panel.cpp
		panel/inspector/inspector_panel.cpp)
target_include_directories(ui PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(ui PUBLIC
		imgui)
target_precompile_headers(ui PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")