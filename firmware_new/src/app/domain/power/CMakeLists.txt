# Domain - Power Management
# Power module handler for Module 0x02

add_library(app_domain_power STATIC
    power_module_handler.c
)

target_include_directories(app_domain_power PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/state_management
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/register
    ${CMAKE_CURRENT_SOURCE_DIR}/../../infrastructure/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../module_management
)

target_link_libraries(app_domain_power
    hal_common
    hal_communication
    hal_register
    app_infrastructure_communication
)

