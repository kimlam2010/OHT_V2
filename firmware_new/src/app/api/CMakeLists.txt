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
)

target_include_directories(app_api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/peripherals
)

# Note: Full endpoints are linked into main firmware via other targets; tests use minimal app_api
