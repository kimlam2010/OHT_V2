# Application API Library
# API layer

# Create library
add_library(app_api STATIC
    api_manager.c
    # api_endpoints.c # Commented out - uses different API structure
)

# Include directories
target_include_directories(app_api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/common
)

# Link dependencies
target_link_libraries(app_api
    hal_common
    app_core
    app_managers
    app_modules
)
