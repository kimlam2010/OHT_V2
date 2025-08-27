# OHT-50 Firmware CMake Configuration
# ISO/IEC 12207 Compliant Build System
# Version: 1.0.0

cmake_minimum_required(VERSION 3.16)
project(OHT50_Firmware VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags for MISRA C:2012 compliance
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror=implicit-function-declaration")

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(config)
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(tests)
# add_subdirectory(docs)  # Removed - docs doesn't have CMakeLists.txt
add_subdirectory(third_party)

# Main executable
add_executable(oht50_main src/main.c)

# Link libraries
target_link_libraries(oht50_main 
    hal_common
    hal_gpio
    hal_communication
    hal_safety
    hal_storage
    hal_peripherals
    app_core
    app_managers
    app_modules
    app_api
    http_server
    utils
)

# Include directories
target_include_directories(oht50_main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/api
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal/peripherals
)
