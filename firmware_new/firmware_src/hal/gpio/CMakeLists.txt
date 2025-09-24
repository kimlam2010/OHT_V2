# HAL GPIO Library
# GPIO abstraction layer

# Create library
add_library(hal_gpio STATIC
    hal_gpio.c
)

# Include directories
target_include_directories(hal_gpio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# Link dependencies
target_link_libraries(hal_gpio
    hal_common
)
