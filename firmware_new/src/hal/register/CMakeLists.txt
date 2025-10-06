# HAL Register CMakeLists.txt
# Register Info and Validation Library

# Create register library
add_library(hal_register STATIC
    register_info.c
)

# Set target properties
set_target_properties(hal_register PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Include directories
target_include_directories(hal_register PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/hal/common
)

# Link with common HAL
target_link_libraries(hal_register
    hal_common
)

# Install the library
install(TARGETS hal_register
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
