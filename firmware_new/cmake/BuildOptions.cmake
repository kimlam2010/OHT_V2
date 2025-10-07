# OHT-50 Firmware Build Options
# Version: 1.0.0

# Build options
option(BUILD_TESTING "Build test suite" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers (ASan, UBSan)" OFF)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis" OFF)
option(REBUILD_MIN_API "Rebuild Minimal API v1 and skip tests" OFF)

# Feature options
option(ENABLE_LIDAR "Enable LiDAR support" ON)
option(ENABLE_WIFI_AP "Enable WiFi AP support" ON)
option(ENABLE_WEBSOCKET "Enable WebSocket support" ON)
option(ENABLE_HTTPS "Enable HTTPS support" ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Coverage flags
if(ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
        add_link_options(--coverage)
        message(STATUS "Code coverage enabled")
    else()
        message(WARNING "Coverage not supported for this compiler")
    endif()
endif()

# Sanitizer flags
if(ENABLE_SANITIZERS)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
        message(STATUS "Sanitizers enabled (ASan, UBSan)")
    else()
        message(WARNING "Sanitizers not supported for this compiler")
    endif()
endif()

# Static analysis
if(ENABLE_STATIC_ANALYSIS)
    set(CMAKE_C_CLANG_TIDY "clang-tidy;-checks=*")
    message(STATUS "Static analysis enabled")
endif()

# Install directories
include(GNUInstallDirs)
set(CMAKE_INSTALL_BINDIR "bin")
set(CMAKE_INSTALL_LIBDIR "lib")
set(CMAKE_INSTALL_INCLUDEDIR "include")
set(CMAKE_INSTALL_DATADIR "share/oht50")


