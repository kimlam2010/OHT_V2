# Source code CMakeLists.txt
# ISO/IEC 12207 - Implementation Layer

# Add subdirectories
add_subdirectory(hal)
add_subdirectory(app)
add_subdirectory(utils)

# Minimal API demo binary (optional)
add_executable(min_api tools/min_api_main.c)
target_link_libraries(min_api app_api hal_common pthread)
target_link_libraries(min_api websocket_server)
target_include_directories(min_api PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app/api ${CMAKE_CURRENT_SOURCE_DIR}/app)
