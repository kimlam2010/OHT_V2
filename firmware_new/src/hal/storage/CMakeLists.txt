# HAL Storage Library
# Configuration persistence

# Create library
add_library(hal_storage STATIC
    hal_config_persistence.c
)

# Include directories
target_include_directories(hal_storage PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# Link dependencies
target_link_libraries(hal_storage
    hal_common
)
