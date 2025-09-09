# Integration Tests CMakeLists.txt
# ISO/IEC/IEEE 29119 Integration Tests

# Add managers integration test
add_executable(test_managers_integration test_managers_integration.c)

# Link with required libraries
target_link_libraries(test_managers_integration
    unity
    hal_common
    hal_rs485
    hal_estop
    hal_led
    communication_manager
    module_manager
    safety_manager
    telemetry_manager
)

# Simple Integration tests
add_executable(test_simple_integration
    test_simple_integration.c
)

target_include_directories(test_simple_integration PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(test_simple_integration
    hal_common
    hal_gpio
    hal_peripherals
    hal_safety
    unity
)

# Basic Integration tests
add_executable(test_basic_integration
    test_basic_integration.c
)

target_include_directories(test_basic_integration PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/api
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app
)

target_link_libraries(test_basic_integration
    hal_common
    hal_gpio
    hal_peripherals
    hal_safety
    app_api
    unity
)

# Add to test suite
add_test(NAME ManagersIntegrationTest COMMAND test_managers_integration)
add_test(NAME SimpleIntegrationTest COMMAND test_simple_integration)
add_test(NAME BasicIntegrationTest COMMAND test_basic_integration)
