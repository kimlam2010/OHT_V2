# Tests CMakeLists.txt
# ISO/IEC/IEEE 29119 Testing Framework

# Include unit tests
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
    hal_rs485
    app_managers
    app_modules
    unity
)

# Add module discovery test to CTest
add_test(NAME test_module_discovery COMMAND test_module_discovery)

# Safety system integration test
add_executable(test_safety_system
    integration/test_safety_system.c
)

target_include_directories(test_safety_system PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/estop
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_safety_system
    hal_common
    hal_estop
    app_modules
    unity
)

# Add safety system test to CTest
add_test(NAME test_safety_system COMMAND test_safety_system)

# Enable testing
enable_testing()
