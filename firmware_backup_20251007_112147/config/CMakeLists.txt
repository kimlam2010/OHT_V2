# Config CMakeLists.txt
# Configuration files

# Configure build-time configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/build_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/build_config.h
)

# Create config library
add_library(config INTERFACE)

target_include_directories(config INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
