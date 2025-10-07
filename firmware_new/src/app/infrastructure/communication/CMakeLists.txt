# Infrastructure - Communication Layer
# Handles RS485 and Modbus RTU communication

add_library(app_infrastructure_communication STATIC
    communication_manager.c
)

target_include_directories(app_infrastructure_communication PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/state_management
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/control
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/register
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/peripherals
    # TEMPORARY: Domain dependency for registry functions
    # TODO: Refactor to remove circular dependency (Issue #TBD)
    ${CMAKE_CURRENT_SOURCE_DIR}/../../domain/module_management
    ${CMAKE_CURRENT_SOURCE_DIR}/../../validation
)

target_link_libraries(app_infrastructure_communication
    hal_common
    hal_communication
    hal_register
)

