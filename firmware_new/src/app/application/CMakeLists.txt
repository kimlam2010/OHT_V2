# Application Layer
# High-level orchestration services

# Add subdirectories
add_subdirectory(safety_orchestrator)

# Note: system_orchestrator is reserved for future implementation

# Create unified application interface library
add_library(app_application INTERFACE)

target_link_libraries(app_application INTERFACE
    app_application_safety_orchestrator
)

