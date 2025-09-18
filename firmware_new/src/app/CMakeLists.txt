# Application Layer CMakeLists.txt
# ISO/IEC 12207 - Software Design

# Add subdirectories
add_subdirectory(core)
add_subdirectory(managers)
add_subdirectory(modules)
add_subdirectory(api)

# WebSocket server library
add_library(websocket_server websocket_server.c)
target_include_directories(websocket_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(websocket_server hal_common pthread ssl crypto)


