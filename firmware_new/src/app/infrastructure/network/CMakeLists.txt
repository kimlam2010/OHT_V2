# Infrastructure - Network Layer
# Handles network management (WiFi, AP, Fallback)

add_library(app_infrastructure_network STATIC
    network_manager.c
    wifi_manager.c
    wifi_ap_manager.c
    fallback_manager.c
)

target_include_directories(app_infrastructure_network PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/state_management
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../hal/peripherals
)

target_link_libraries(app_infrastructure_network
    hal_common
    hal_peripherals
)

