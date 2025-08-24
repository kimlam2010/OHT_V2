# Application Modules Library
# Module handlers

# Create library
add_library(app_modules STATIC
    power_module_handler.c
    motor_module_handler.c
    dock_module_handler.c
    module_registry.c
)

# Include directories
target_include_directories(app_modules PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/communication
)

# Link dependencies
target_link_libraries(app_modules
    hal_common
    hal_communication
    app_core
)
