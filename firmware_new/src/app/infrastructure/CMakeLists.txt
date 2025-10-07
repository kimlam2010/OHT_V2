# Infrastructure Layer
# Low-level services: Communication, Network, Telemetry

# Add subdirectories
add_subdirectory(communication)
add_subdirectory(network)
add_subdirectory(telemetry)

# Create unified infrastructure interface library
add_library(app_infrastructure INTERFACE)

target_link_libraries(app_infrastructure INTERFACE
    app_infrastructure_communication
    app_infrastructure_network
    app_infrastructure_telemetry
)

