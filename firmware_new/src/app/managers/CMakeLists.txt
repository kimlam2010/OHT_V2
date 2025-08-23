# Application Managers Library
# System managers

# Create library
add_library(app_managers STATIC
    safety_manager.c
    communication_manager.c
    module_manager.c
)

# Include directories
target_include_directories(app_managers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# Link dependencies
target_link_libraries(app_managers
    hal_common
    hal_communication
    hal_safety
    hal_peripherals
    app_core
)
