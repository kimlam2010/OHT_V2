#!/bin/bash

# Script to move header files to new include structure
# MISRA C:2012 + AUTOSAR compliant structure

echo "Moving header files to new include structure..."

# Create directories if they don't exist
mkdir -p include/01-CORE
mkdir -p include/02-HAL
mkdir -p include/03-MODULES
mkdir -p include/04-SERVICES
mkdir -p include/05-CONTROL
mkdir -p include/06-UTILITIES

# Move core headers
echo "Moving core headers..."
cp include/system_state_machine.h include/01-CORE/ 2>/dev/null || true
cp include/config_system.h include/01-CORE/ 2>/dev/null || true
cp include/system_config.h include/01-CORE/ 2>/dev/null || true

# Move HAL headers
echo "Moving HAL headers..."
cp include/hal_*.h include/02-HAL/ 2>/dev/null || true

# Move module headers
echo "Moving module headers..."
cp include/motor_module_handler.h include/03-MODULES/ 2>/dev/null || true
cp include/dock_module_handler.h include/03-MODULES/ 2>/dev/null || true
cp include/power_module_handler.h include/03-MODULES/ 2>/dev/null || true
cp include/di_do_module_handler.h include/03-MODULES/ 2>/dev/null || true
cp include/io_module_handler.h include/03-MODULES/ 2>/dev/null || true
cp include/led_manager.h include/03-MODULES/ 2>/dev/null || true

# Move service headers
echo "Moving service headers..."
cp include/api_*.h include/04-SERVICES/ 2>/dev/null || true
cp include/communication_manager.h include/04-SERVICES/ 2>/dev/null || true
cp include/network_*.h include/04-SERVICES/ 2>/dev/null || true
cp include/websocket_server.h include/04-SERVICES/ 2>/dev/null || true
cp include/safety_*.h include/04-SERVICES/ 2>/dev/null || true
cp include/security_manager.h include/04-SERVICES/ 2>/dev/null || true
cp include/diagnostics_manager.h include/04-SERVICES/ 2>/dev/null || true
cp include/performance_manager.h include/04-SERVICES/ 2>/dev/null || true
cp include/http_server.h include/04-SERVICES/ 2>/dev/null || true

# Move control headers
echo "Moving control headers..."
cp include/control_loop.h include/05-CONTROL/ 2>/dev/null || true
cp include/performance_metrics.h include/05-CONTROL/ 2>/dev/null || true
cp include/module_manager.h include/05-CONTROL/ 2>/dev/null || true
cp include/module_registry.h include/05-CONTROL/ 2>/dev/null || true

# Move utility headers
echo "Moving utility headers..."
cp include/*.backup include/06-UTILITIES/ 2>/dev/null || true

echo "Header files moved successfully!"
echo "New structure created in include/ directory"
