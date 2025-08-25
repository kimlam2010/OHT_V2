# Application Modules Library
# Module handlers

# Create library
add_library(app_modules STATIC
    power_module_handler.c
    travel_motor_module_handler.c
    safety_module_handler.c
    dock_module_handler.c
    module_registry.c
)

# Include directories
target_include_directories(app_modules PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/storage
)

# Link dependencies
target_link_libraries(app_modules
    hal_common
    hal_communication
    hal_peripherals
    hal_gpio
    hal_safety
    hal_storage
    app_core
    app_managers
)
