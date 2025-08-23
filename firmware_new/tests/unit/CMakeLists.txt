# Unit Tests CMakeLists.txt
# ISO/IEC/IEEE 29119 Unit Tests

# HAL Common tests
add_executable(test_hal_common
    test_hal_common.c
)

target_include_directories(test_hal_common PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_hal_common
    hal_common
    unity
)

# Add test to CTest
add_test(NAME test_hal_common COMMAND test_hal_common)

# Enable testing
enable_testing()
