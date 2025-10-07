# Application Core Library - Reorganized Structure
# Domain-Driven Architecture: State Management, Safety, Control

# Add subdirectories (each builds its own library)
add_subdirectory(state_management)
add_subdirectory(safety)
add_subdirectory(control)

# Create unified app_core INTERFACE library
# This aggregates all subdomain libraries for backward compatibility
add_library(app_core INTERFACE)

# Link all subdomain libraries
# Order matters: control depends on safety
target_link_libraries(app_core INTERFACE
    app_core_state_management
    app_core_safety
    app_core_control
)

# Resolve circular dependency: control uses safety functions
target_link_libraries(app_core_control
    app_core_safety
)

# Include directories for backward compatibility
# Other modules can still include core files without knowing subdirectory structure
target_include_directories(app_core INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/state_management
    ${CMAKE_CURRENT_SOURCE_DIR}/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/control
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/gpio
)
