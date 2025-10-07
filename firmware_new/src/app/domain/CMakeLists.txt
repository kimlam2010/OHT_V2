# Domain Layer
# Business logic domains: Module Management, Power, Motion, Safety Module, Dock

# Add subdirectories
# NOTE: module_management depends on other domains, so build them first
add_subdirectory(power)
add_subdirectory(motion)
add_subdirectory(safety_module)
add_subdirectory(dock)
add_subdirectory(module_management)  # Last - depends on all above

# Create unified domain interface library
add_library(app_domain INTERFACE)

target_link_libraries(app_domain INTERFACE
    app_domain_module_management
    app_domain_power
    app_domain_motion
    app_domain_safety_module
    app_domain_dock
)

