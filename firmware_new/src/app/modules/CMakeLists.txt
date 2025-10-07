# ============================================================================
# DEPRECATED: App Modules Library (Legacy Compatibility Shim)
# ============================================================================
# This library is DEPRECATED as of v1.0.1 (Domain-Driven Architecture Migration)
# All functionality has been moved to domain/:
#   - domain/power/ (power_module_handler)
#   - domain/motion/ (travel_motor_module_handler)
#   - domain/safety_module/ (safety_module_handler)
#   - domain/dock/ (dock_module_handler)
#   - domain/module_management/ (module_registry)
#
# This compatibility shim redirects to new libraries for backward compatibility.
# Will be REMOVED in v1.1.0
# ============================================================================

# Create interface library that links to new domain architecture
add_library(app_modules INTERFACE)

target_link_libraries(app_modules INTERFACE
    # Domain Layer
    app_domain_power
    app_domain_motion
    app_domain_safety_module
    app_domain_dock
    app_domain_module_management
    
    # HAL dependencies
    hal_common
    hal_communication
    hal_register
    hal_peripherals
    hal_gpio
    hal_safety
)
