add_subdirectory(ui)

add_library(editor STATIC editor.cpp)
target_include_directories(editor PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(editor PUBLIC
		core runtime
		GLEW::GLEW glfw imgui
		ui
)
target_precompile_headers(editor PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")