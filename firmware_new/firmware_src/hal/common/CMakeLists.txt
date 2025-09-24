# HAL Common Library
# Common utilities and types

# Create library
add_library(hal_common STATIC
    hal_common.c
)

# Include directories
target_include_directories(hal_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)
