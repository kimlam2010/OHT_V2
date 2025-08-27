# Application Core Library
# State machine and control logic

# Create library
add_library(app_core STATIC
    system_state_machine.c
    control_loop.c
    safety_monitor.c
    system_controller.c
)

# Include directories
target_include_directories(app_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/storage
)

# Link dependencies
target_link_libraries(app_core
    hal_common
    hal_safety
    hal_peripherals
    hal_communication
    hal_gpio
    hal_storage
)
