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

# Enable testing
enable_testing()
