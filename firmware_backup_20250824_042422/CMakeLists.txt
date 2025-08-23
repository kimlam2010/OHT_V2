# OHT-50 Firmware CMake Configuration
# ISO/IEC 12207 Compliant Build System
# Version: 1.0.0

cmake_minimum_required(VERSION 3.16)
project(OHT50_Firmware VERSION 1.0.0 LANGUAGES C)

# Set C standard for MISRA C:2012 compliance
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags for safety and quality
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror=implicit-function-declaration")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")

# Enable testing
enable_testing()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add subdirectories
add_subdirectory(config)
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(tests)
add_subdirectory(third_party)

# Main application target
add_executable(oht50_main src/main.c)
target_link_libraries(oht50_main hal app utils)

# Install rules
install(TARGETS oht50_main DESTINATION bin)
install(FILES include/oht50_firmware.h DESTINATION include)
