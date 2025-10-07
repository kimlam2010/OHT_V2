# OHT-50 Firmware Compiler Options
# Version: 1.0.0

# C Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Warning flags
set(WARNING_FLAGS
    -Wall
    -Wextra
    -Wpedantic
    -Werror=implicit-function-declaration
    -Werror=return-type
    -Werror=uninitialized
    -Wformat=2
    -Wformat-security
    -Wnull-dereference
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wold-style-definition
    -Wshadow
    -Wcast-align
    -Wconversion
    -Wsign-conversion
)

# Add warning flags to compile options
add_compile_options(${WARNING_FLAGS})

# Debug build options
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -O0 -DDEBUG")

# Release build options
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")

# RelWithDebInfo build options
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -g -O2 -DNDEBUG")

# MinSizeRel build options
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Os -DNDEBUG")

# Platform-specific flags
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions(_GNU_SOURCE)
    add_compile_definitions(_POSIX_C_SOURCE=200809L)
endif()

# Threading support
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Export compile commands for IDE/tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


