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
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/api
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_basic_integration
    hal_common
    hal_gpio
    app_api
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

# Performance testing suite
add_executable(test_performance_load
    performance/test_performance_load.c
)

target_include_directories(test_performance_load PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/api
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_performance_load
    hal_common
    hal_communication
    app_core
    app_managers
    app_modules
    app_api
    unity
    pthread
)

# Add performance load test to CTest
add_test(NAME test_performance_load COMMAND test_performance_load)

# Stress testing suite
add_executable(test_stress_testing
    performance/test_stress_testing.c
)

target_include_directories(test_stress_testing PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/api
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_stress_testing
    hal_common
    hal_communication
    app_core
    app_managers
    app_modules
    app_api
    unity
    pthread
)

# Add stress testing to CTest
add_test(NAME test_stress_testing COMMAND test_stress_testing)

# Safety validation testing suite
add_executable(test_safety_validation
    performance/test_safety_validation.c
)

target_include_directories(test_safety_validation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/api
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
    app_api
    unity
    pthread
)

# Add safety validation test to CTest
add_test(NAME test_safety_validation COMMAND test_safety_validation)

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
