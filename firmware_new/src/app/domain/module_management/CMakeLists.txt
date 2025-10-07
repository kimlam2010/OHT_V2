# Domain - Module Management
# Module discovery, registration, polling, and registry

add_library(app_domain_module_management STATIC
    module_manager.c
    module_polling_manager.c
    module_registry.c
)

target_include_directories(app_domain_module_management PUBLIC
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../infrastructure/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../validation
    # Domain cross-references
    ${CMAKE_CURRENT_SOURCE_DIR}/../power
    ${CMAKE_CURRENT_SOURCE_DIR}/../motion
    ${CMAKE_CURRENT_SOURCE_DIR}/../safety_module
    ${CMAKE_CURRENT_SOURCE_DIR}/../dock
)

target_link_libraries(app_domain_module_management
    hal_common
    hal_communication
    hal_register
    hal_peripherals
    app_core_state_management
    app_core_safety
    app_infrastructure_communication
    app_validation
    # Domain cross-references (for auto-detect calls)
    app_domain_power
    app_domain_motion
    app_domain_safety_module
    app_domain_dock
)

