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

# Add to test suite
add_test(NAME ManagersIntegrationTest COMMAND test_managers_integration)
