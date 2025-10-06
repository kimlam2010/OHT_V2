# Validation Module CMakeLists.txt
# Issue #135 - Register validation for RS485/Modbus fixes

# Validation library
add_library(app_validation
    register_validation.c
)

# Include directories
target_include_directories(app_validation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# Link libraries
target_link_libraries(app_validation
    hal_common
)
