add_executable(game_engine main.cpp)
target_include_directories(game_engine PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(game_engine PUBLIC
		core editor
)
target_precompile_headers(game_engine PUBLIC "${CMAKE_SOURCE_DIR}/include/pch.h")