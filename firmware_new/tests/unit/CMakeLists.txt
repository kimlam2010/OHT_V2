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

# HAL GPIO tests
add_executable(test_hal_gpio
    hal/test_hal_gpio.c
)

target_include_directories(test_hal_gpio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_hal_gpio
    hal_gpio
    hal_common
    unity
)

# Application API Manager tests
add_executable(test_api_manager
    app/test_api_manager.c
)

target_include_directories(test_api_manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/api
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_api_manager
    app_api
    hal_common
    unity
)

# HAL LiDAR tests
add_executable(test_hal_lidar
    hal/test_hal_lidar.c
)

target_include_directories(test_hal_lidar PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_hal_lidar
    hal_peripherals
    hal_common
    unity
)

# Add tests to CTest
add_test(NAME test_hal_common COMMAND test_hal_common)
add_test(NAME test_hal_gpio COMMAND test_hal_gpio)
add_test(NAME test_api_manager COMMAND test_api_manager)
add_test(NAME test_hal_lidar COMMAND test_hal_lidar)

# Enable testing
enable_testing()
