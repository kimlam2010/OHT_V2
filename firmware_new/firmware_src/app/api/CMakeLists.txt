# Minimal API build for min_api tool or full endpoints for firmware
if(REBUILD_MIN_API)
  add_library(app_api STATIC
      api_manager.c
      api_endpoints_stub.c
  )
else()
  add_library(app_api STATIC
      api_manager.c
      api_endpoints.c
      module_control_apis.c
      state_control_apis.c
      config_stats_apis.c
      security_auth.c
  )
endif()

target_link_libraries(app_api
    PUBLIC
      hal_common
      hal_gpio
      hal_communication
      hal_safety
      hal_peripherals
      app_core
      app_managers
      app_storage
)

target_include_directories(app_api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../storage
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/peripherals
)

# Note: Full endpoints are linked into main firmware via other targets; tests use minimal app_api
