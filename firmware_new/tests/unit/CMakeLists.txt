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

# HAL RS485 tests
add_executable(test_hal_rs485
    hal/test_hal_rs485.c
)

target_include_directories(test_hal_rs485 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_hal_rs485
    hal_communication
    hal_common
    unity
)

# HAL Network tests
add_executable(test_hal_network
    hal/test_hal_network.c
)

target_include_directories(test_hal_network PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_hal_network
    hal_communication
    hal_common
    unity
)

# HAL E-Stop tests
add_executable(test_hal_estop
    hal/test_hal_estop.c
)

target_include_directories(test_hal_estop PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_hal_estop
    hal_safety
    hal_common
    unity
    pthread
)

# HAL Storage tests
add_executable(test_hal_storage
    hal/test_hal_storage.c
)

target_include_directories(test_hal_storage PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_hal_storage
    hal_storage
    hal_common
    unity
)

# HAL USB tests
add_executable(test_hal_usb
    hal/test_hal_usb.c
)

target_include_directories(test_hal_usb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_hal_usb
    hal_communication
    hal_common
    unity
    pthread
)

# Application Core tests
add_executable(test_system_state_machine
    app/test_system_state_machine.c
)

target_include_directories(test_system_state_machine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_system_state_machine
    app_core
    hal_common
    unity
)

add_executable(test_control_loop
    app/test_control_loop.c
)

target_include_directories(test_control_loop PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_control_loop
    app_core
    hal_common
    unity
)

# Application Communication Manager tests
add_executable(test_communication_manager
    app/test_communication_manager.c
)

target_include_directories(test_communication_manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_communication_manager
    app_managers
    hal_common
    hal_communication
    unity
    pthread
)

# Application Module Manager tests
add_executable(test_module_manager
    app/test_module_manager.c
)

target_include_directories(test_module_manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_module_manager
    app_managers
    hal_common
    unity
    pthread
)

# Safety Module Handler tests
add_executable(test_safety_module_handler
    app/test_safety_module_handler.c
)

target_include_directories(test_safety_module_handler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_safety_module_handler
    app_modules
    hal_common
    unity
)

# Travel Motor Module Handler tests
add_executable(test_travel_motor_module_handler
    app/test_travel_motor_module_handler.c
)

target_include_directories(test_travel_motor_module_handler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_travel_motor_module_handler
    app_modules
    hal_common
    unity
)

# Dock Module Handler tests
add_executable(test_dock_module_handler
    app/test_dock_module_handler.c
)

target_include_directories(test_dock_module_handler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_dock_module_handler
    app_modules
    hal_common
    unity
)

# Power Module Handler tests
add_executable(test_power_module_handler
    app/test_power_module_handler.c
)

target_include_directories(test_power_module_handler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
)

target_link_libraries(test_power_module_handler
    app_modules
    hal_common
    hal_communication
    unity
    pthread
)

# Add tests to CTest
add_test(NAME test_hal_common COMMAND test_hal_common)
add_test(NAME test_hal_gpio COMMAND test_hal_gpio)
add_test(NAME test_api_manager COMMAND test_api_manager)
add_test(NAME test_hal_lidar COMMAND test_hal_lidar)
add_test(NAME test_hal_rs485 COMMAND test_hal_rs485)
add_test(NAME test_hal_network COMMAND test_hal_network)
add_test(NAME test_hal_estop COMMAND test_hal_estop)
add_test(NAME test_hal_storage COMMAND test_hal_storage)
add_test(NAME test_hal_usb COMMAND test_hal_usb)
add_test(NAME test_system_state_machine COMMAND test_system_state_machine)
add_test(NAME test_control_loop COMMAND test_control_loop)
add_test(NAME test_communication_manager COMMAND test_communication_manager)
add_test(NAME test_module_manager COMMAND test_module_manager)
add_test(NAME test_safety_module_handler COMMAND test_safety_module_handler)
add_test(NAME test_travel_motor_module_handler COMMAND test_travel_motor_module_handler)
add_test(NAME test_dock_module_handler COMMAND test_dock_module_handler)
add_test(NAME test_power_module_handler COMMAND test_power_module_handler)

# Enable testing
enable_testing()
