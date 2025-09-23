# OHT-50 Firmware CMake Configuration
# ISO/IEC 12207 Compliant Build System
# Version: 1.0.0

cmake_minimum_required(VERSION 3.16)
project(OHT50_Firmware VERSION 1.0.0 LANGUAGES C)

# Minimal API rebuild switch (skip tests to avoid legacy API dependencies)
option(REBUILD_MIN_API "Rebuild Minimal API v1 and skip tests" OFF)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags for MISRA C:2012 compliance
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror=implicit-function-declaration")

# Enable testing (unless Minimal API rebuild)
enable_testing()

# Add subdirectories
add_subdirectory(config)
add_subdirectory(firmware_src)
# add_subdirectory(include)  # Removed - websocket_server.h moved to firmware_src/app/
# Tests (skip when Minimal API rebuild is active)
if(REBUILD_MIN_API)
  # Only build smoke tests when rebuilding Minimal API
  add_subdirectory(tests/smoke)
else()
  add_subdirectory(tests)
endif()
# add_subdirectory(docs)  # Removed - docs doesn't have CMakeLists.txt
add_subdirectory(third_party)

# Main executable
add_executable(oht50_main firmware_src/main.c)

# Link libraries
target_link_libraries(oht50_main 
    hal_common
    hal_gpio
    hal_communication
    hal_safety
    hal_peripherals
    app_core
    app_managers
    app_modules
    app_api
    websocket_server
    simple_http_8081
    utils
    pthread
    ssl
    crypto
    m
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

# Build guard: block if mock/simulate usage present in runtime sources (not tests)
file(GLOB_RECURSE RUNTIME_C_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

set(MOCK_PATTERN "mock_")

foreach(src_file IN LISTS RUNTIME_C_SOURCES)
    file(READ ${src_file} SRC_CONTENT)
    string(FIND "${SRC_CONTENT}" ${MOCK_PATTERN} HAS_MOCK)
    if(HAS_MOCK GREATER -1)
        message(FATAL_ERROR "Build blocked: Forbidden mock usage found in runtime source: ${src_file}. Remove mocks per EXEC PLAN.")
    endif()
endforeach()
