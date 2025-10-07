# Include CMakeLists.txt
# Public API headers

# Create include library
add_library(include INTERFACE)

target_include_directories(include INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
