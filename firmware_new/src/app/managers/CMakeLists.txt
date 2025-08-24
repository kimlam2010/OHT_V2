# Application Managers Library
# System managers

# Create library
add_library(app_managers STATIC
    safety_manager.c
    communication_manager.c
    module_manager.c
)

# Include directories
target_include_directories(app_managers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules
)

# Link dependencies
target_link_libraries(app_managers
    hal_common
    hal_communication
    hal_safety
    hal_peripherals
    hal_gpio
    hal_storage
    app_core
    app_modules
)
