# Application Layer CMakeLists.txt
# ISO/IEC 12207 - Software Design
# Architecture: Domain-Driven Design v1.0.1

# ============================================================================
# DOMAIN-DRIVEN ARCHITECTURE LAYERS (v1.0.1)
# ============================================================================

# 🎛️ Core Layer - Fundamental business logic (no external dependencies)
add_subdirectory(core)

# 🔌 Infrastructure Layer - Low-level services (communication, network, telemetry)
add_subdirectory(infrastructure)

# 🏭 Domain Layer - Business domains (module management, handlers)
add_subdirectory(domain)

# 🔐 Application Layer - High-level orchestration
add_subdirectory(application)

# 🌐 API Layer - External interfaces
add_subdirectory(api)

# ✅ Cross-Cutting Concerns
add_subdirectory(validation)
add_subdirectory(storage)

# ============================================================================
# LEGACY COMPATIBILITY (Deprecated - will be removed in v1.1.0)
# ============================================================================
# Note: managers/ and modules/ folders are now EMPTY (only CMakeLists.txt remains)
# They provide compatibility shims that redirect to new architecture
# All functionality has been moved to infrastructure/, domain/, and application/
add_subdirectory(managers)  # DEPRECATED - Compatibility shim
add_subdirectory(modules)   # DEPRECATED - Compatibility shim

# WebSocket server library - COMPLETELY REMOVED (CTO Decision)
# Firmware only uses HTTP/REST API; WebSocket only exists in Backend for Frontend

# Simple HTTP server for port 8081 (Issue #113 Fix) - REMOVED (duplicate functionality)

# HTTP server library (Issue #113, #98 Fix)
add_library(http_server http_server.c)
target_include_directories(http_server PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../hal/common
)
target_link_libraries(http_server hal_common pthread)

# TODO: Security authentication library (Issue #123 - Future implementation)
# add_library(security_auth security_auth.c)
# target_include_directories(security_auth PUBLIC 
#     ${CMAKE_CURRENT_SOURCE_DIR}
#     ${CMAKE_CURRENT_SOURCE_DIR}/api
# )
# target_link_libraries(security_auth hal_common app_core app_api pthread ssl crypto jwt)

# TODO: Module control APIs library (Issue #100 - Future implementation)  
# add_library(module_control_apis module_control_apis.c)
# target_include_directories(module_control_apis PUBLIC 
#     ${CMAKE_CURRENT_SOURCE_DIR}
#     ${CMAKE_CURRENT_SOURCE_DIR}/api
#     ${CMAKE_CURRENT_SOURCE_DIR}/managers
#     ${CMAKE_CURRENT_SOURCE_DIR}/modules
# )
# target_link_libraries(module_control_apis hal_common app_core app_api app_managers app_modules security_auth pthread)


