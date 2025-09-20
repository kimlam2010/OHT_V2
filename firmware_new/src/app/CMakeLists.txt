# Application Layer CMakeLists.txt
# ISO/IEC 12207 - Software Design

# Add subdirectories
add_subdirectory(core)
add_subdirectory(managers)
add_subdirectory(modules)
add_subdirectory(api)

# WebSocket server library
add_library(websocket_server websocket_server.c)
target_include_directories(websocket_server PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../hal/communication
)
target_link_libraries(websocket_server hal_common hal_communication app_core app_managers app_modules pthread ssl crypto)

# Simple HTTP server for port 8081 (Issue #113 Fix)
add_library(simple_http_8081 simple_http_8081.c)
target_include_directories(simple_http_8081 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(simple_http_8081 pthread)


