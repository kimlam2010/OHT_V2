# Application - Safety Orchestrator
# Orchestrates safety from multiple sources (core safety + safety module + safety manager)

add_library(app_application_safety_orchestrator STATIC
    safety_manager.c
)

target_include_directories(app_application_safety_orchestrator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/state_management
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../domain/module_management
    ${CMAKE_CURRENT_SOURCE_DIR}/../../domain/safety_module
)

target_link_libraries(app_application_safety_orchestrator
    hal_common
    hal_safety
    hal_peripherals
    app_core_state_management
    app_core_safety
)

