#!/bin/bash

# Script to fix the header file structure
# Move files to correct locations and clean up backups

echo "Fixing header file structure..."

# Create backup of current state
echo "Creating backup of current state..."
cp -r include include_backup_$(date +%Y%m%d_%H%M%S)

# Clean up all include directories
echo "Cleaning up include directories..."
rm -rf include/01-CORE/*
rm -rf include/02-HAL/*
rm -rf include/03-MODULES/*
rm -rf include/04-SERVICES/*
rm -rf include/05-CONTROL/*
rm -rf include/06-UTILITIES/*

# Remove backup files from root include directory
echo "Removing backup files from root include..."
find include -name "*.backup" -delete

# Move files to correct locations
echo "Moving files to correct locations..."

# 01-CORE headers
echo "Moving core headers..."
mv include/system_init.h include/01-CORE/ 2>/dev/null || true
mv include/state_transitions.h include/01-CORE/ 2>/dev/null || true
mv include/system_config.h include/01-CORE/ 2>/dev/null || true
mv include/system_state_machine.h include/01-CORE/ 2>/dev/null || true
mv include/config_system.h include/01-CORE/ 2>/dev/null || true

# 02-HAL headers
echo "Moving HAL headers..."
mv include/hal_*.h include/02-HAL/ 2>/dev/null || true

# 03-MODULES headers
echo "Moving module headers..."
mv include/motor_module_handler.h include/03-MODULES/ 2>/dev/null || true
mv include/dock_module_handler.h include/03-MODULES/ 2>/dev/null || true
mv include/power_module_handler.h include/03-MODULES/ 2>/dev/null || true
mv include/io_module_handler.h include/03-MODULES/ 2>/dev/null || true
mv include/di_do_module_handler.h include/03-MODULES/ 2>/dev/null || true
mv include/led_manager.h include/03-MODULES/ 2>/dev/null || true

# 04-SERVICES headers
echo "Moving service headers..."
mv include/api_manager.h include/04-SERVICES/ 2>/dev/null || true
mv include/api_endpoints.h include/04-SERVICES/ 2>/dev/null || true
mv include/http_server.h include/04-SERVICES/ 2>/dev/null || true
mv include/communication_manager.h include/04-SERVICES/ 2>/dev/null || true
mv include/network_manager.h include/04-SERVICES/ 2>/dev/null || true
mv include/websocket_server.h include/04-SERVICES/ 2>/dev/null || true
mv include/safety_manager.h include/04-SERVICES/ 2>/dev/null || true
mv include/safety_mechanisms.h include/04-SERVICES/ 2>/dev/null || true
mv include/security_manager.h include/04-SERVICES/ 2>/dev/null || true
mv include/diagnostics_manager.h include/04-SERVICES/ 2>/dev/null || true
mv include/performance_manager.h include/04-SERVICES/ 2>/dev/null || true
mv include/network_api.h include/04-SERVICES/ 2>/dev/null || true

# 05-CONTROL headers
echo "Moving control headers..."
mv include/control_loop.h include/05-CONTROL/ 2>/dev/null || true
mv include/module_manager.h include/05-CONTROL/ 2>/dev/null || true
mv include/module_registry.h include/05-CONTROL/ 2>/dev/null || true
mv include/performance_metrics.h include/05-CONTROL/ 2>/dev/null || true

# 06-UTILITIES headers (if any)
echo "Moving utility headers..."
# No utility headers found

# Clean up any remaining files in root include
echo "Cleaning up root include directory..."
find include -maxdepth 1 -name "*.h" -delete

echo "Header file structure fixed successfully!"
echo "Backup created in include_backup_*"
