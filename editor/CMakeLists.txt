# add_subdirectory(objects)
# add_subdirectory(systems)

add_library(editor STATIC editor.cpp ${CMAKE_CURRENT_SOURCE_DIR}/systems/ui/ui.cpp) # TODO: delete ui.cpp later
target_include_directories(editor PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(editor PUBLIC
		core game
		GLEW::GLEW glfw imgui
#		editor_objects
#		editor_systems
)
target_precompile_headers(editor PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")