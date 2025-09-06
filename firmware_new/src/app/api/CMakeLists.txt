# Application API Library
# API layer with WebSocket and HTTP integration

# Create library
add_library(app_api STATIC
    api_manager.c
    api_endpoints.c
    api_config.c
    api_config_endpoints.c
    api_config_json.c
    api_telemetry.c
    api_telemetry_collectors.c
    api_telemetry_endpoints.c
    api_telemetry_json.c
    api_diagnostics.c
    api_diagnostics_endpoints.c
    api_diagnostics_json.c
    
    # Phase 5: Security & Testing
    api_auth.c
    api_auth_endpoints.c
    api_auth_middleware.c
    api_auth_json.c
    api_error_handling.c
    api_input_validation.c
    
    ../http_server.c
    ../websocket_server.c
)

# Include directories
target_include_directories(app_api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/storage
)

# Link dependencies
target_link_libraries(app_api
    hal_common
    hal_communication
    hal_safety
    hal_peripherals
    hal_gpio
    app_core
    app_managers
    app_modules
    pthread
    ssl
    crypto
)
