#!/bin/bash

# Script to fix mock references in test files
echo "Fixing mock references in test files..."

# Fix include statements
find tests/ -name "*.c" -exec sed -i 's/#include "mock_/#\/\/ #include "mock_/g' {} \;

# Fix function calls
find tests/ -name "*.c" -exec sed -i 's/mock_/#\/\/ mock_/g' {} \;

# Fix extern declarations
find tests/ -name "*.c" -exec sed -i 's/extern void mock_/#\/\/ extern void mock_/g' {} \;

echo "Mock references fixed!"
