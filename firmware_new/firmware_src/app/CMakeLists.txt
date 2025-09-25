# Application Layer CMakeLists.txt
# ISO/IEC 12207 - Software Design

# Add subdirectories
add_subdirectory(core)
add_subdirectory(managers)
add_subdirectory(modules)
add_subdirectory(api)
add_subdirectory(validation)
add_subdirectory(storage)

# WebSocket server library (Original - DEPRECATED)
add_library(websocket_server websocket_server.c)
target_include_directories(websocket_server PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../hal/communication
)
target_link_libraries(websocket_server hal_common hal_communication app_core app_managers app_modules app_api pthread ssl crypto)

# WebSocket server library with libwebsockets (Issue #153 Fix)
add_library(websocket_server_lws websocket_server_lws.c)
target_include_directories(websocket_server_lws PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../hal/communication
)
target_link_libraries(websocket_server_lws 
    hal_common 
    hal_communication 
    app_core 
    app_managers 
    app_modules 
    app_api 
    pthread 
    ssl 
    crypto
    websockets
)

# WebSocket Integration library (Issue #153 Fix)
add_library(websocket_integration websocket_integration.c)
target_include_directories(websocket_integration PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../hal/communication
)
target_link_libraries(websocket_integration 
    hal_common 
    hal_communication 
    app_core 
    app_managers 
    app_modules 
    app_api 
    websocket_server
    websocket_server_lws
    pthread 
    ssl 
    crypto
    websockets
)

# Simple HTTP server for port 8081 (Issue #113 Fix) - REMOVED (duplicate functionality)

# HTTP server library (Issue #113, #98 Fix)
add_library(http_server http_server.c)
target_include_directories(http_server PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../hal/common
)
target_link_libraries(http_server hal_common pthread)

# TODO: Security authentication library (Issue #123 - Future implementation)
# add_library(security_auth security_auth.c)
# target_include_directories(security_auth PUBLIC 
#     ${CMAKE_CURRENT_SOURCE_DIR}
#     ${CMAKE_CURRENT_SOURCE_DIR}/api
# )
# target_link_libraries(security_auth hal_common app_core app_api pthread ssl crypto jwt)

# TODO: Module control APIs library (Issue #100 - Future implementation)  
# add_library(module_control_apis module_control_apis.c)
# target_include_directories(module_control_apis PUBLIC 
#     ${CMAKE_CURRENT_SOURCE_DIR}
#     ${CMAKE_CURRENT_SOURCE_DIR}/api
#     ${CMAKE_CURRENT_SOURCE_DIR}/managers
#     ${CMAKE_CURRENT_SOURCE_DIR}/modules
# )
# target_link_libraries(module_control_apis hal_common app_core app_api app_managers app_modules security_auth pthread)


