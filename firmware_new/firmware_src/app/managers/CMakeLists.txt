# App Managers Library
add_library(app_managers
    safety_manager.c
    communication_manager.c
    module_manager.c
    module_polling_manager.c
    telemetry_manager.c
    network/network_manager.c
    network/wifi_manager.c
    network/wifi_ap_manager.c
    network/fallback_manager.c
)

target_include_directories(app_managers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../validation
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal/peripherals
)

target_link_libraries(app_managers
    app_core
    app_modules
    app_validation
    hal_common
    hal_communication
    hal_safety
    hal_peripherals
    http_server
    m
)
