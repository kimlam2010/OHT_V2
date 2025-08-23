# Tests CMakeLists.txt
# ISO/IEC/IEEE 29119 compliant test suite

# Add subdirectories
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(system)
add_subdirectory(mocks)

# Enable testing
enable_testing()
