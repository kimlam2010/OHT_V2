# ============================================================================
# DEPRECATED: App Managers Library (Legacy Compatibility Shim)
# ============================================================================
# This library is DEPRECATED as of v1.0.1 (Domain-Driven Architecture Migration)
# All functionality has been moved to:
#   - infrastructure/ (communication, network, telemetry)
#   - domain/ (module management)
#   - application/ (safety orchestrator)
#
# This compatibility shim redirects to new libraries for backward compatibility.
# Will be REMOVED in v1.1.0
# ============================================================================

# Create interface library that links to new architecture
add_library(app_managers INTERFACE)

target_link_libraries(app_managers INTERFACE
    # Infrastructure Layer
    app_infrastructure_communication
    app_infrastructure_network
    app_infrastructure_telemetry
    
    # Domain Layer
    app_domain_module_management
    
    # Application Layer
    app_application_safety_orchestrator
    
    # HAL dependencies
    hal_common
    hal_communication
    hal_register
    hal_safety
    hal_peripherals
    http_server
    m
)
