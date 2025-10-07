# Domain - Safety Module
# Safety module handler for Module 0x03

add_library(app_domain_safety_module STATIC
    safety_module_handler.c
)

target_include_directories(app_domain_safety_module PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/state_management
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/register
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../infrastructure/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../module_management
)

target_link_libraries(app_domain_safety_module
    hal_common
    hal_communication
    hal_register
    hal_safety
    app_core_safety
    app_infrastructure_communication
)

