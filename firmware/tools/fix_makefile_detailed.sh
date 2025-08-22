#!/bin/bash

# Script to fix Makefile with detailed paths for each file
# Update all object file paths to use exact new structure

echo "Fixing Makefile with detailed paths..."

# Create backup
cp Makefile Makefile.backup.detailed.$(date +%Y%m%d_%H%M%S)

# Update HAL object paths with exact subdirectories
echo "Updating HAL object paths with exact subdirectories..."
sed -i 's|$(OBJ_DIR)/02-HAL/hal_lidar.o|$(OBJ_DIR)/02-HAL/02-03-Sensors/hal_lidar.o|g' Makefile
sed -i 's|$(OBJ_DIR)/02-HAL/hal_rs485.o|$(OBJ_DIR)/02-HAL/02-01-Communication/hal_rs485.o|g' Makefile
sed -i 's|$(OBJ_DIR)/02-HAL/hal_network.o|$(OBJ_DIR)/02-HAL/02-01-Communication/hal_network.o|g' Makefile
sed -i 's|$(OBJ_DIR)/02-HAL/hal_usb_debug.o|$(OBJ_DIR)/02-HAL/02-01-Communication/hal_usb_debug.o|g' Makefile
sed -i 's|$(OBJ_DIR)/02-HAL/hal_gpio.o|$(OBJ_DIR)/02-HAL/02-02-IO-Devices/hal_gpio.o|g' Makefile
sed -i 's|$(OBJ_DIR)/02-HAL/hal_led.o|$(OBJ_DIR)/02-HAL/02-02-IO-Devices/hal_led.o|g' Makefile
sed -i 's|$(OBJ_DIR)/02-HAL/hal_relay.o|$(OBJ_DIR)/02-HAL/02-02-IO-Devices/hal_relay.o|g' Makefile
sed -i 's|$(OBJ_DIR)/02-HAL/hal_estop.o|$(OBJ_DIR)/02-HAL/02-04-Safety/hal_estop.o|g' Makefile
sed -i 's|$(OBJ_DIR)/02-HAL/hal_common.o|$(OBJ_DIR)/02-HAL/02-05-System/hal_common.o|g' Makefile
sed -i 's|$(OBJ_DIR)/02-HAL/hal_config_persistence.o|$(OBJ_DIR)/02-HAL/02-05-System/hal_config_persistence.o|g' Makefile
sed -i 's|$(OBJ_DIR)/02-HAL/hal_ota_update.o|$(OBJ_DIR)/02-HAL/02-05-System/hal_ota_update.o|g' Makefile

echo "Makefile updated with detailed paths!"
echo "Backup created as Makefile.backup.detailed.*"
