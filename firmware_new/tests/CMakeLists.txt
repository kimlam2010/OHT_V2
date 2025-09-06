# Tests CMakeLists.txt
# ISO/IEC/IEEE 29119 Testing Framework - CLEANED VERSION
# FW Team - Simplified test suite

# Include unit tests - ENABLED for HAL testing
add_subdirectory(unit)

# Basic integration test
add_executable(test_basic_integration
    integration/test_basic_integration.c
)

target_include_directories(test_basic_integration PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(test_basic_integration
    hal_common
    hal_gpio
    unity
)

# Add integration test to CTest
add_test(NAME test_basic_integration COMMAND test_basic_integration)

# Module discovery integration test - REMOVED (file deleted)
# add_executable(test_module_discovery_simple
#     integration/test_module_discovery_simple.c
# )
# 
# target_include_directories(test_module_discovery_simple PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/rs485
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
#     ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
#     ${CMAKE_CURRENT_SOURCE_DIR}/../include
# )
# 
# target_link_libraries(test_module_discovery_simple
#     hal_common
#     hal_communication
#     app_managers
#     app_modules
#     unity
# )
# 
# # Add module discovery test to CTest
# add_test(NAME test_module_discovery_simple COMMAND test_module_discovery_simple)

# Basic performance testing suite (simplified)
add_executable(test_basic_performance
    performance/test_basic_performance.c
)

target_include_directories(test_basic_performance PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(test_basic_performance
    hal_common
    unity
    pthread
    m
)

# Add basic performance test to CTest
add_test(NAME test_basic_performance COMMAND test_basic_performance)

# Enable testing
enable_testing()
