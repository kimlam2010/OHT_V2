# Unity Test Framework CMakeLists.txt
# https://github.com/ThrowTheSwitch/Unity

# Create Unity library
add_library(unity STATIC
    unity.c
)

target_include_directories(unity PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
