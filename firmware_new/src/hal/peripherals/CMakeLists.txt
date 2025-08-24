# HAL Peripherals Library
# LED, Relay, LiDAR peripherals

# Create library
add_library(hal_peripherals STATIC
    hal_led.c
    hal_relay.c
    hal_lidar.c
)

# Include directories
target_include_directories(hal_peripherals PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# Link dependencies
target_link_libraries(hal_peripherals
    hal_common
    hal_gpio
    pthread
)
