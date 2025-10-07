# HAL Safety Library
# E-Stop and safety systems

# Create library
add_library(hal_safety STATIC
    hal_estop.c
)

# Include directories
target_include_directories(hal_safety PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# Link dependencies
target_link_libraries(hal_safety
    hal_common
    hal_gpio
)
