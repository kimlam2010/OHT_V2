# Utils Library
# Utility functions

# Create library - Empty for now
add_library(utils INTERFACE)

# Include directories
target_include_directories(utils INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# Link dependencies - Empty for now
# target_link_libraries(utils
#     hal_common
# )
