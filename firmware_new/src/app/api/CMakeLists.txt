# Application API Library
# API layer - DISABLED (no source files)

# Create library
# add_library(app_api STATIC
#     # api_manager.c - Removed, not needed
#     # api_endpoints.c - File not found, removed
# )

# Include directories
# target_include_directories(app_api PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}
#     ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
#     ${CMAKE_CURRENT_SOURCE_DIR}/../managers
#     ${CMAKE_CURRENT_SOURCE_DIR}/../modules
#     ${CMAKE_CURRENT_SOURCE_DIR}/../core
#     ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/common
#     ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/communication
#     ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/safety
#     ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/peripherals
#     ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/gpio
#     ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/storage
# )

# Link dependencies
# target_link_libraries(app_api
#     hal_common
#     hal_communication
#     hal_safety
#     hal_peripherals
#     hal_gpio
#     hal_storage
#     app_core
#     app_managers
#     app_modules
#     # http_server - Removed, not found
# )
