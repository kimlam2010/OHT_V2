# Domain - Motion Control
# Travel motor module handler for Module 0x04

add_library(app_domain_motion STATIC
    travel_motor_module_handler.c
)

target_include_directories(app_domain_motion PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/state_management
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/control
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/register
    ${CMAKE_CURRENT_SOURCE_DIR}/../../infrastructure/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../application/safety_orchestrator
    ${CMAKE_CURRENT_SOURCE_DIR}/../module_management
)

target_link_libraries(app_domain_motion
    hal_common
    hal_communication
    hal_register
    app_core_state_management
    app_core_safety
    app_infrastructure_communication
)

