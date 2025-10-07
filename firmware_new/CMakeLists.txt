# OHT-50 Firmware CMake Configuration
# ISO/IEC 12207 Compliant Build System
# Version: 1.0.0

cmake_minimum_required(VERSION 3.16)
project(OHT50_Firmware VERSION 1.0.0 LANGUAGES C)

# CMake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include CMake modules
include(CompilerOptions)
include(BuildOptions)
include(Dependencies)

# Configure project
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/version.h"
    @ONLY
)

# Enable testing
if(BUILD_TESTING AND NOT REBUILD_MIN_API)
    enable_testing()
endif()

# Add subdirectories
add_subdirectory(config)
add_subdirectory(third_party)
add_subdirectory(src)

# Tests (skip when Minimal API rebuild is active)
if(BUILD_TESTING)
    if(REBUILD_MIN_API)
        # Only build smoke tests when rebuilding Minimal API
        add_subdirectory(tests/smoke)
    else()
        add_subdirectory(tests)
    endif()
endif()

# Main executable
add_executable(oht50_main src/main.c)

# Link libraries
target_link_libraries(oht50_main PRIVATE
    # Application libraries
    app_core
    app_managers
    app_modules
    app_api
    app_storage
    
    # HAL libraries
    hal_common
    hal_gpio
    hal_communication
    hal_safety
    hal_peripherals
    
    # Utility libraries
    utils
    
    # System libraries
    Threads::Threads
    ${ALL_SYSTEM_LIBS}
)

# Include directories
target_include_directories(oht50_main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/api
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal/peripherals
    ${OPENSSL_INCLUDE_DIR}
)

# Compile definitions
target_compile_definitions(oht50_main PRIVATE
    FIRMWARE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    FIRMWARE_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    FIRMWARE_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Build guard: block if mock/simulate usage present in runtime sources (not tests)
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
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
    message(STATUS "Mock guard: passed (no mock usage in runtime sources)")
endif()

# Installation rules
install(TARGETS oht50_main
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install configuration files
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules.yaml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/config
)

# Summary
message(STATUS "==============================================")
message(STATUS "OHT-50 Firmware Build Configuration Summary")
message(STATUS "==============================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "Build testing: ${BUILD_TESTING}")
message(STATUS "Enable coverage: ${ENABLE_COVERAGE}")
message(STATUS "Enable sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "Enable LiDAR: ${ENABLE_LIDAR}")
message(STATUS "Enable WiFi AP: ${ENABLE_WIFI_AP}")
message(STATUS "Enable WebSocket: ${ENABLE_WEBSOCKET}")
message(STATUS "Enable HTTPS: ${ENABLE_HTTPS}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "==============================================")
