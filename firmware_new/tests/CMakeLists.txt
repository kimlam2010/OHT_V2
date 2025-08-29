# Tests CMakeLists.txt
# ISO/IEC/IEEE 29119 Testing Framework

# Include unit tests - ENABLED for HAL testing
add_subdirectory(unit)

# Integration tests
add_executable(test_basic_integration
    integration/test_basic_integration.c
)

target_include_directories(test_basic_integration PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_basic_integration
    hal_common
    hal_gpio
    unity
)

# Add integration test to CTest
add_test(NAME test_basic_integration COMMAND test_basic_integration)

# Module discovery integration test
add_executable(test_module_discovery
    integration/test_module_discovery.c
)

target_include_directories(test_module_discovery PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_module_discovery
    hal_common
    hal_communication
    app_managers
    app_modules
    unity
)

# Add module discovery test to CTest
add_test(NAME test_module_discovery COMMAND test_module_discovery)

# Managers integration test
add_executable(test_managers_integration
    integration/test_managers_integration.c
)

target_include_directories(test_managers_integration PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_managers_integration
    hal_common
    hal_communication
    hal_safety
    hal_peripherals
    app_managers
    app_modules
    unity
)

# Add managers integration test to CTest
add_test(NAME test_managers_integration COMMAND test_managers_integration)

# Basic performance testing suite
add_executable(test_basic_performance
    performance/test_basic_performance.c
)

target_include_directories(test_basic_performance PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_basic_performance
    hal_common
    unity
    pthread
    m
)

# Add basic performance test to CTest
add_test(NAME test_basic_performance COMMAND test_basic_performance)

# Enhanced logging testing suite
add_executable(test_enhanced_logging
    performance/test_enhanced_logging.c
)

target_include_directories(test_enhanced_logging PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_enhanced_logging
    hal_common
    unity
)

# Add enhanced logging test to CTest
add_test(NAME test_enhanced_logging COMMAND test_enhanced_logging)

# Performance testing suite - DISABLED (depends on removed app_api)
# add_executable(test_performance_load
#     performance/test_performance_load.c
# )
#
# target_include_directories(test_performance_load PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
#     ${CMAKE_CURRENT_SOURCE_DIR}/../include
#     ${CMAKE_CURRENT_SOURCE_DIR}/mocks
# )
#
# target_link_libraries(test_performance_load
#     hal_common
#     hal_communication
#     app_core
#     app_managers
#     app_modules
#     unity
#     pthread
#     m
# )
#
# add_test(NAME test_performance_load COMMAND test_performance_load)

# Stress testing suite - DISABLED (depends on removed app_api)
# add_executable(test_stress_testing
#     performance/test_stress_testing.c
# )
#
# target_include_directories(test_stress_testing PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
#     ${CMAKE_CURRENT_SOURCE_DIR}/../include
#     ${CMAKE_CURRENT_SOURCE_DIR}/mocks
# )
#
# target_link_libraries(test_stress_testing
#     hal_common
#     hal_communication
#     app_core
#     app_managers
#     app_modules
#     unity
#     pthread
#     m
# )
#
# add_test(NAME test_stress_testing COMMAND test_stress_testing)

# Safety validation testing suite
add_executable(test_safety_validation
    performance/test_safety_validation.c
)

# Simple safety validation testing suite
add_executable(test_safety_validation_simple
    performance/test_safety_validation_simple.c
)

target_include_directories(test_safety_validation_simple PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_safety_validation_simple
    hal_common
    hal_communication
    hal_safety
    app_core
    app_managers
    app_modules
    unity
    pthread
    m
)

target_include_directories(test_safety_validation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_safety_validation
    hal_common
    hal_communication
    hal_safety
    app_core
    app_managers
    app_modules
    unity
    pthread
)

# Add safety validation test to CTest
add_test(NAME test_safety_validation COMMAND test_safety_validation)

# Add simple safety validation test to CTest
add_test(NAME test_safety_validation_simple COMMAND test_safety_validation_simple)

# Basic safety testing suite (no Unity)
add_executable(test_safety_basic
    performance/test_safety_basic.c
)

target_include_directories(test_safety_basic PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_safety_basic
    hal_common
    hal_communication
    hal_safety
    app_core
    app_managers
    app_modules
    pthread
    m
)

# Add basic safety test to CTest
add_test(NAME test_safety_basic COMMAND test_safety_basic)

# Safety Monitor Test Runner
add_executable(safety_monitor_test_runner
    performance/safety_monitor_test_runner.c
    unit/safety_monitor_test.c
)

# Safety Monitor Mock Test Runner (no hardware required)
add_executable(safety_monitor_test_runner_mock
    performance/safety_monitor_test_runner_mock.c
    unit/safety_monitor_test.c
)

target_include_directories(safety_monitor_test_runner_mock PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/unit
)

target_link_libraries(safety_monitor_test_runner_mock
    hal_common
    hal_gpio
    hal_peripherals
    hal_safety
    hal_storage
    app_core
)

target_include_directories(safety_monitor_test_runner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/unit
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(safety_monitor_test_runner
    hal_common
    hal_gpio
    hal_peripherals
    hal_safety
    hal_storage
    app_core
    unity
    pthread
    m
)

# Add safety monitor test runner to CTest
add_test(NAME safety_monitor_test_runner COMMAND safety_monitor_test_runner)

# Mock safety testing suite (no GPIO access required)
add_executable(test_safety_basic_mock
    performance/test_safety_basic_mock.c
)

target_include_directories(test_safety_basic_mock PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/unity
)

target_link_libraries(test_safety_basic_mock
    hal_common
    hal_communication
    hal_safety
    app_core
    app_managers
    app_modules
    unity
    pthread
    m
)

# Add mock safety test to CTest
add_test(NAME test_safety_basic_mock COMMAND test_safety_basic_mock)

# Safety system integration test - DISABLED due to API incompatibility
# add_executable(test_safety_system
#     integration/test_safety_system.c
# )
# 
# target_include_directories(test_safety_system PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/safety
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
#     ${CMAKE_CURRENT_SOURCE_DIR}/../include
#     ${CMAKE_CURRENT_SOURCE_DIR}/mocks
# )
# 
# target_link_libraries(test_safety_system
#     hal_common
#     hal_safety
#     app_modules
#     unity
# )
# 
# Add safety system test to CTest
# add_test(NAME test_safety_system COMMAND test_safety_system)

# Enable testing
enable_testing()
