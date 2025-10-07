# State Management Library
# System State Machine and System Controller

# Create library
add_library(app_core_state_management STATIC
    system_state_machine.c
    system_controller.c
)

# Include directories
target_include_directories(app_core_state_management PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../safety           # For safety_monitor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../control          # For control_loop.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include # Project includes
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/gpio
)

# Link dependencies
target_link_libraries(app_core_state_management
    hal_common
    hal_peripherals
    hal_communication
    hal_safety
    hal_gpio
)

