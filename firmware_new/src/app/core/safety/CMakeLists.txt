# Safety System Library
# Safety Monitor, Critical Module Detector, Graduated Response, RS485 Integration

# Create library
add_library(app_core_safety STATIC
    safety_monitor.c
    critical_module_detector.c
    graduated_response_system.c
    safety_rs485_integration.c
)

# Include directories
target_include_directories(app_core_safety PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../state_management # For system_state_machine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include # Project includes
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/register
    ${CMAKE_CURRENT_SOURCE_DIR}/../../managers      # For module_manager.h
)

# Link dependencies
target_link_libraries(app_core_safety
    hal_common
    hal_safety
    hal_peripherals
    hal_communication
    hal_gpio
    hal_register
)

