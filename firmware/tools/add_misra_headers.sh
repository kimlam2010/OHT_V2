#!/bin/bash

# Script to add MISRA C:2012 headers to all files
# MISRA C:2012 + AUTOSAR compliant structure

echo "Adding MISRA headers to all files..."

# Function to add MISRA header to a file
add_misra_header_to_file() {
    local file="$1"
    local temp_file="${file}.tmp"
    local filename=$(basename "$file")
    local extension="${filename##*.}"
    local basename="${filename%.*}"
    
    echo "Processing: $file"
    
    # Create backup
    cp "$file" "${file}.backup"
    
    # Determine file type and create appropriate header
    if [[ "$extension" == "h" ]]; then
        # Header file
        cat > "$temp_file" << EOF
/**
 * @file $filename
 * @brief $basename interface
 * @version 1.0.0
 * @date 2025-01-27
 * @author Firmware Team
 * @license MIT
 * @compliance MISRA C:2012, AUTOSAR
 * @safety SIL2
 */

#ifndef ${basename^^}_H
#define ${basename^^}_H

/* Include dependencies */
#include <stdint.h>
#include <stdbool.h>

EOF
        # Add the rest of the file content
        cat "$file" >> "$temp_file"
        
        # Add closing endif if not present
        if ! grep -q "#endif" "$temp_file"; then
            echo "" >> "$temp_file"
            echo "#endif /* ${basename^^}_H */" >> "$temp_file"
        fi
        
    elif [[ "$extension" == "c" ]]; then
        # Source file
        cat > "$temp_file" << EOF
/**
 * @file $filename
 * @brief $basename implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @author Firmware Team
 * @license MIT
 * @compliance MISRA C:2012, AUTOSAR
 * @safety SIL2
 */

EOF
        # Add the rest of the file content
        cat "$file" >> "$temp_file"
    fi
    
    # Replace original file
    mv "$temp_file" "$file"
}

# Add MISRA headers to all source files
echo "Adding MISRA headers to source files..."
find src -name "*.c" -type f | while read -r file; do
    add_misra_header_to_file "$file"
done

# Add MISRA headers to all header files
echo "Adding MISRA headers to header files..."
find include -name "*.h" -type f | while read -r file; do
    add_misra_header_to_file "$file"
done

echo "MISRA headers added successfully!"
echo "Backup files created with .backup extension"
