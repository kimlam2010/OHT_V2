# Control System Library
# Control Loop and Estimator

# Create library
add_library(app_core_control STATIC
    control_loop.c
    estimator_1d.c
)

# Include directories
target_include_directories(app_core_control PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../safety           # For safety_monitor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../state_management # For system_state_machine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include # Project includes
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/gpio
)

# Link dependencies
target_link_libraries(app_core_control
    hal_common
    hal_safety
    hal_peripherals
    hal_communication
    hal_gpio
)

# Note: app_core_control depends on app_core_safety for safety_monitor_is_estop_active()
# This is handled by linking app_core_safety when app_core_control is used

