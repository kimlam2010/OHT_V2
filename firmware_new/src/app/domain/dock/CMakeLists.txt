# Domain - Dock Management
# Dock module handler for Module 0x05

add_library(app_domain_dock STATIC
    dock_module_handler.c
)

target_include_directories(app_domain_dock PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/state_management
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/register
    ${CMAKE_CURRENT_SOURCE_DIR}/../../infrastructure/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../application/safety_orchestrator
    ${CMAKE_CURRENT_SOURCE_DIR}/../module_management
)

target_link_libraries(app_domain_dock
    hal_common
    hal_communication
    hal_register
    app_core_safety
    app_infrastructure_communication
)

