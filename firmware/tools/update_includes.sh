#!/bin/bash

# Script to update include paths in all files
# MISRA C:2012 + AUTOSAR compliant structure

echo "Updating include paths in all files..."

# Function to update includes in a file
update_includes_in_file() {
    local file="$1"
    local temp_file="${file}.tmp"
    
    echo "Processing: $file"
    
    # Create backup
    cp "$file" "${file}.backup"
    
    # Update include paths based on new structure
    sed -e 's|#include "hal_|#include "02-HAL/hal_|g' \
        -e 's|#include "motor_|#include "03-MODULES/motor_|g' \
        -e 's|#include "dock_|#include "03-MODULES/dock_|g' \
        -e 's|#include "power_|#include "03-MODULES/power_|g' \
        -e 's|#include "di_do_|#include "03-MODULES/di_do_|g' \
        -e 's|#include "io_|#include "03-MODULES/io_|g' \
        -e 's|#include "led_|#include "03-MODULES/led_|g' \
        -e 's|#include "api_|#include "04-SERVICES/api_|g' \
        -e 's|#include "communication_|#include "04-SERVICES/communication_|g' \
        -e 's|#include "network_|#include "04-SERVICES/network_|g' \
        -e 's|#include "websocket_|#include "04-SERVICES/websocket_|g' \
        -e 's|#include "safety_|#include "04-SERVICES/safety_|g' \
        -e 's|#include "security_|#include "04-SERVICES/security_|g' \
        -e 's|#include "diagnostics_|#include "04-SERVICES/diagnostics_|g' \
        -e 's|#include "performance_|#include "04-SERVICES/performance_|g' \
        -e 's|#include "http_|#include "04-SERVICES/http_|g' \
        -e 's|#include "control_|#include "05-CONTROL/control_|g' \
        -e 's|#include "module_|#include "05-CONTROL/module_|g' \
        -e 's|#include "system_|#include "01-CORE/system_|g' \
        -e 's|#include "config_|#include "01-CORE/config_|g' \
        -e 's|#include "state_|#include "01-CORE/state_|g' \
        "$file" > "$temp_file"
    
    # Replace original file
    mv "$temp_file" "$file"
}

# Update includes in all source files
echo "Updating source files..."
find src -name "*.c" -type f | while read -r file; do
    update_includes_in_file "$file"
done

# Update includes in all header files
echo "Updating header files..."
find include -name "*.h" -type f | while read -r file; do
    update_includes_in_file "$file"
done

echo "Include paths updated successfully!"
echo "Backup files created with .backup extension"
