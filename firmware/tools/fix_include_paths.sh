#!/bin/bash

# Script to fix include paths in Makefile
# Simplify include paths to avoid conflicts

echo "Fixing include paths in Makefile..."

# Create backup
cp Makefile Makefile.backup.paths.$(date +%Y%m%d_%H%M%S)

# Fix HAL include path - only include HAL directory
echo "Fixing HAL include path..."
sed -i 's|-I$(HAL_INCLUDE_DIR) -I$(CORE_INCLUDE_DIR)|-I$(HAL_INCLUDE_DIR)|g' Makefile

# Fix other include paths to be simpler
echo "Fixing other include paths..."
sed -i 's|-I$(MODULES_INCLUDE_DIR) -I$(HAL_INCLUDE_DIR) -I$(CORE_INCLUDE_DIR) -I$(SERVICES_INCLUDE_DIR)|-I$(MODULES_INCLUDE_DIR) -I$(HAL_INCLUDE_DIR)|g' Makefile
sed -i 's|-I$(SERVICES_INCLUDE_DIR) -I$(HAL_INCLUDE_DIR) -I$(CORE_INCLUDE_DIR) -I$(MODULES_INCLUDE_DIR) -I$(CONTROL_INCLUDE_DIR)|-I$(SERVICES_INCLUDE_DIR) -I$(HAL_INCLUDE_DIR)|g' Makefile
sed -i 's|-I$(CONTROL_INCLUDE_DIR) -I$(HAL_INCLUDE_DIR) -I$(CORE_INCLUDE_DIR) -I$(MODULES_INCLUDE_DIR) -I$(SERVICES_INCLUDE_DIR)|-I$(CONTROL_INCLUDE_DIR) -I$(HAL_INCLUDE_DIR)|g' Makefile
sed -i 's|-I$(UTILITIES_INCLUDE_DIR) -I$(HAL_INCLUDE_DIR) -I$(CORE_INCLUDE_DIR)|-I$(UTILITIES_INCLUDE_DIR) -I$(HAL_INCLUDE_DIR)|g' Makefile

# Fix main.o include path
sed -i 's|-I$(CORE_INCLUDE_DIR) -I$(HAL_INCLUDE_DIR) -I$(MODULES_INCLUDE_DIR) -I$(SERVICES_INCLUDE_DIR) -I$(CONTROL_INCLUDE_DIR) -I$(UTILITIES_INCLUDE_DIR)|-I$(CORE_INCLUDE_DIR) -I$(HAL_INCLUDE_DIR)|g' Makefile

echo "Include paths fixed successfully!"
echo "Backup created as Makefile.backup.paths.*"
