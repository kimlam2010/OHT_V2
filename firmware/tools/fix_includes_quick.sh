#!/bin/bash

# Quick fix for include statements in source files
# Remove the layer prefix from includes

echo "Quick fixing include statements..."

# Create backup
cp -r src src_backup_$(date +%Y%m%d_%H%M%S)

# Fix include statements - remove layer prefixes
echo "Fixing include statements in source files..."

# Find all .c files and fix includes
find src -name "*.c" -type f | while read -r file; do
    echo "Processing: $file"
    
    # Create backup of the file
    cp "$file" "${file}.backup"
    
    # Fix include statements
    sed -i 's|#include "02-HAL/|#include "|g' "$file"
    sed -i 's|#include "01-CORE/|#include "|g' "$file"
    sed -i 's|#include "03-MODULES/|#include "|g' "$file"
    sed -i 's|#include "04-SERVICES/|#include "|g' "$file"
    sed -i 's|#include "05-CONTROL/|#include "|g' "$file"
    sed -i 's|#include "06-UTILITIES/|#include "|g' "$file"
done

echo "Include statements fixed!"
echo "Backup created in src_backup_*"
