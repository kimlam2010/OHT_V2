# Infrastructure - Telemetry Layer
# Handles telemetry data collection and serialization

add_library(app_infrastructure_telemetry STATIC
    telemetry_manager.c
)

target_include_directories(app_infrastructure_telemetry PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/state_management
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/control
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../domain/module_management
)

target_link_libraries(app_infrastructure_telemetry
    hal_common
    app_core_state_management
    app_core_safety
    app_core_control
)

