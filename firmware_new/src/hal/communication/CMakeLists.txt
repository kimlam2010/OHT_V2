# HAL Communication Library
# RS485, Network, USB communication

# Create library
add_library(hal_communication STATIC
    hal_rs485.c
    hal_network.c
    hal_wifi_ap.c
)

# Include directories
target_include_directories(hal_communication PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../register
)

# Link dependencies
target_link_libraries(hal_communication
    hal_common
    pthread
)
