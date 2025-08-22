#!/bin/bash

# Script to cleanup backup files and misplaced files
# Clean up the entire firmware structure

echo "Cleaning up backup files and misplaced files..."

# Remove all .backup files
echo "Removing all .backup files..."
find . -name "*.backup" -type f -delete

# Remove backup directories
echo "Removing backup directories..."
find . -name "*backup*" -type d -exec rm -rf {} + 2>/dev/null || true

# Clean up root level files that should be in proper directories
echo "Cleaning up root level files..."

# Move main.c to proper location if it's in root
if [ -f "src/main.c" ]; then
    echo "Moving main.c to 01-CORE/01-01-Main/"
    mkdir -p src/01-CORE/01-01-Main
    mv src/main.c src/01-CORE/01-01-Main/ 2>/dev/null || true
fi

# Clean up old directories that shouldn't exist
echo "Cleaning up old directories..."
if [ -d "src/hal" ]; then
    echo "Removing old src/hal directory..."
    rm -rf src/hal
fi

if [ -d "src/app" ]; then
    echo "Removing old src/app directory..."
    rm -rf src/app
fi

# Clean up root level files in firmware directory
echo "Cleaning up root level files in firmware..."
cd firmware 2>/dev/null || true

# Remove backup files from root
find . -maxdepth 1 -name "*.backup" -delete

# Remove old files that should be in proper locations
rm -f types.h.backup 2>/dev/null || true
rm -f modules.yaml.backup 2>/dev/null || true
rm -f test_api.sh.backup 2>/dev/null || true

echo "Cleanup completed successfully!"
echo "All backup files and misplaced files have been removed."
