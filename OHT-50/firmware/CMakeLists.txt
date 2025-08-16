cmake_minimum_required(VERSION 3.16)
project(OHT50_Firmware VERSION 1.0.0 LANGUAGES C CXX)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files - organized by modules
file(GLOB SCHEDULER_SOURCES "src/scheduler/*.c" "src/scheduler/*.cpp")
file(GLOB HAL_SOURCES "src/hal/*.c" "src/hal/*.cpp")
file(GLOB COMMS_SOURCES "src/comms/*.c" "src/comms/*.cpp")
file(GLOB STATE_MACHINE_SOURCES "src/state_machine/*.c" "src/state_machine/*.cpp")
file(GLOB CONTROL_SOURCES "src/control/*.c" "src/control/*.cpp")
file(GLOB SAFETY_SOURCES "src/safety/*.c" "src/safety/*.cpp")

# Main source files
file(GLOB MAIN_SOURCES "src/*.c" "src/*.cpp")

# Combine all sources
set(ALL_SOURCES 
    ${MAIN_SOURCES}
    ${SCHEDULER_SOURCES}
    ${HAL_SOURCES}
    ${COMMS_SOURCES}
    ${STATE_MACHINE_SOURCES}
    ${CONTROL_SOURCES}
    ${SAFETY_SOURCES}
)

# Create executable
add_executable(oht50_firmware ${ALL_SOURCES})

# Link libraries
target_link_libraries(oht50_firmware 
    gpiod
    serialport
    pthread
    rt
)

# Install target
install(TARGETS oht50_firmware DESTINATION bin)

# Enable testing
enable_testing()

# Add test executable
file(GLOB_RECURSE TEST_SOURCES "tests/*.c" "tests/*.cpp")
if(TEST_SOURCES)
    add_executable(oht50_tests ${TEST_SOURCES})
    target_link_libraries(oht50_tests 
        gpiod
        serialport
        pthread
        rt
    )
    add_test(NAME OHT50_Tests COMMAND oht50_tests)
endif()

# Print configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Scheduler sources: ${SCHEDULER_SOURCES}")
message(STATUS "HAL sources: ${HAL_SOURCES}")
message(STATUS "Comms sources: ${COMMS_SOURCES}")
message(STATUS "State machine sources: ${STATE_MACHINE_SOURCES}")
message(STATUS "Control sources: ${CONTROL_SOURCES}")
message(STATUS "Safety sources: ${SAFETY_SOURCES}")
