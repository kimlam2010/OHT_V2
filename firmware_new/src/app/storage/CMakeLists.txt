# Storage Layer CMakeLists.txt
# Module Data Storage Implementation

# Module Data Storage Library
add_library(app_storage module_data_storage.c register_value_cache.c)
target_include_directories(app_storage PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../api
    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/common
)
target_link_libraries(app_storage hal_common app_core app_api pthread)
