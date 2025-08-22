#!/bin/bash

# Script to fix header files with missing #endif
# MISRA C:2012 + AUTOSAR compliant structure

echo "Fixing header files with missing #endif..."

# Function to fix header file
fix_header_file() {
    local file="$1"
    local temp_file="${file}.tmp"
    local filename=$(basename "$file")
    local basename="${filename%.*}"
    
    echo "Fixing: $file"
    
    # Create backup
    cp "$file" "${file}.backup"
    
    # Check if file has #ifndef but no #endif
    if grep -q "#ifndef" "$file" && ! grep -q "#endif" "$file"; then
        echo "  Adding missing #endif to $file"
        # Add #endif at the end of the file
        echo "" >> "$file"
        echo "#endif /* ${basename^^}_H */" >> "$file"
    fi
    
    # Check if file has #ifndef but #endif is not properly formatted
    if grep -q "#ifndef" "$file" && grep -q "#endif" "$file"; then
        # Check if #endif is properly formatted
        if ! grep -q "#endif.*${basename^^}_H" "$file"; then
            echo "  Fixing #endif format in $file"
            # Replace the last #endif with properly formatted one
            sed -i '$s/#endif.*/#endif \/* '${basename^^}'_H *\//' "$file"
        fi
    fi
}

# Fix all header files
echo "Fixing header files..."
find include -name "*.h" -type f | while read -r file; do
    fix_header_file "$file"
done

echo "Header files fixed successfully!"
echo "Backup files created with .backup extension"
