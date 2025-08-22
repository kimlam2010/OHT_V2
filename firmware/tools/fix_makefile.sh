#!/bin/bash

# Script to fix Makefile for new structure
# Update all object file paths to use new structure

echo "Fixing Makefile for new structure..."

# Create backup
cp Makefile Makefile.backup.$(date +%Y%m%d_%H%M%S)

# Update HAL object paths from hal/ to 02-HAL/
echo "Updating HAL object paths..."
sed -i 's|$(OBJ_DIR)/hal/|$(OBJ_DIR)/02-HAL/|g' Makefile

# Update APP object paths from app/ to new structure
echo "Updating APP object paths..."
sed -i 's|$(OBJ_DIR)/app/safety_manager.o|$(OBJ_DIR)/04-SERVICES/04-03-Safety/safety_manager.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/system_state_machine.o|$(OBJ_DIR)/01-CORE/01-02-State-Machine/system_state_machine.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/communication_manager.o|$(OBJ_DIR)/04-SERVICES/04-02-Communication/communication_manager.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/module_registry.o|$(OBJ_DIR)/05-CONTROL/05-03-Module-Management/module_registry.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/config_system.o|$(OBJ_DIR)/01-CORE/01-03-Configuration/config_system.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/network_manager.o|$(OBJ_DIR)/04-SERVICES/04-02-Communication/network_manager.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/motor_module_handler.o|$(OBJ_DIR)/03-MODULES/03-01-Motor/motor_module_handler.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/dock_module_handler.o|$(OBJ_DIR)/03-MODULES/03-02-Dock/dock_module_handler.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/power_module_handler.o|$(OBJ_DIR)/03-MODULES/03-03-Power/power_module_handler.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/io_module_handler.o|$(OBJ_DIR)/03-MODULES/03-04-IO/io_module_handler.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/di_do_module_handler.o|$(OBJ_DIR)/03-MODULES/03-04-IO/di_do_module_handler.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/led_manager.o|$(OBJ_DIR)/03-MODULES/03-05-LED/led_manager.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/api_manager.o|$(OBJ_DIR)/04-SERVICES/04-01-API/api_manager.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/api_endpoints.o|$(OBJ_DIR)/04-SERVICES/04-01-API/api_endpoints.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/http_server.o|$(OBJ_DIR)/04-SERVICES/04-01-API/http_server.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/websocket_server.o|$(OBJ_DIR)/04-SERVICES/04-02-Communication/websocket_server.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/module_manager.o|$(OBJ_DIR)/05-CONTROL/05-03-Module-Management/module_manager.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/control_loop.o|$(OBJ_DIR)/05-CONTROL/05-01-Control-Loop/control_loop.o|g' Makefile
sed -i 's|$(OBJ_DIR)/app/performance_metrics.o|$(OBJ_DIR)/05-CONTROL/05-02-Performance/performance_metrics.o|g' Makefile

# Update main.o path
sed -i 's|$(OBJ_DIR)/main.o|$(OBJ_DIR)/01-CORE/01-01-Main/main.o|g' Makefile

echo "Makefile updated successfully!"
echo "Backup created as Makefile.backup.*"
