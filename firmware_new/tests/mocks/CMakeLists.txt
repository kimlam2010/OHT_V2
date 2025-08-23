# Mock Objects CMakeLists.txt
# ISO/IEC/IEEE 29119 Mock Objects

# Mock library - Empty for now
add_library(mocks INTERFACE)

target_include_directories(mocks INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
