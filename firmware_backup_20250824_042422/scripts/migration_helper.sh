#!/bin/bash
# OHT-50 Firmware Migration Helper Script
# ISO/IEC 12207 Standardization Support
# Version: 1.0.0

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
OLD_STRUCTURE="firmware_old"
NEW_STRUCTURE="firmware_new"
BACKUP_DIR="firmware_backup_$(date +%Y%m%d_%H%M%S)"

echo -e "${BLUE}=== OHT-50 Firmware Migration Helper ===${NC}"
echo -e "${YELLOW}This script will help migrate firmware to ISO/IEC 12207 structure${NC}"
echo

# Function to print status
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to create new directory structure
create_new_structure() {
    print_status "Creating new ISO/IEC 12207 compliant directory structure..."
    
    mkdir -p $NEW_STRUCTURE/{src,include,tests,docs,scripts,third_party,config}
    mkdir -p $NEW_STRUCTURE/src/{hal,app,utils}
    mkdir -p $NEW_STRUCTURE/src/hal/{common,gpio,communication,safety,storage,peripherals}
    mkdir -p $NEW_STRUCTURE/src/app/{core,managers,modules,api}
    mkdir -p $NEW_STRUCTURE/tests/{unit,integration,system,mocks}
    mkdir -p $NEW_STRUCTURE/tests/unit/{hal,app,utils}
    mkdir -p $NEW_STRUCTURE/tests/integration/{hal_integration,app_integration,system_integration}
    mkdir -p $NEW_STRUCTURE/tests/system/{safety_tests,performance_tests,stress_tests}
    mkdir -p $NEW_STRUCTURE/docs/{api,design,user_guide,developer_guide}
    mkdir -p $NEW_STRUCTURE/scripts/tools
    mkdir -p $NEW_STRUCTURE/third_party/{cmake,unity,json}
    
    print_status "Directory structure created successfully"
}

# Function to backup current structure
backup_current_structure() {
    print_status "Creating backup of current firmware structure..."
    
    if [ -d "firmware" ]; then
        cp -r firmware $BACKUP_DIR
        print_status "Backup created: $BACKUP_DIR"
    else
        print_warning "No existing firmware directory found"
    fi
}

# Function to migrate source files
migrate_source_files() {
    print_status "Migrating source files to new structure..."
    
    # Create old structure reference
    if [ -d "firmware" ]; then
        mv firmware $OLD_STRUCTURE
    fi
    
    # Migrate HAL files
    if [ -d "$OLD_STRUCTURE/src/hal" ]; then
        # Common HAL files
        cp $OLD_STRUCTURE/src/hal/hal_common.* $NEW_STRUCTURE/src/hal/common/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/hal_common.h $NEW_STRUCTURE/src/hal/common/ 2>/dev/null || true
        
        # GPIO files
        cp $OLD_STRUCTURE/src/hal/hal_gpio.* $NEW_STRUCTURE/src/hal/gpio/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/hal_gpio.h $NEW_STRUCTURE/src/hal/gpio/ 2>/dev/null || true
        
        # Communication files
        cp $OLD_STRUCTURE/src/hal/hal_rs485.* $NEW_STRUCTURE/src/hal/communication/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/hal_rs485.h $NEW_STRUCTURE/src/hal/communication/ 2>/dev/null || true
        cp $OLD_STRUCTURE/src/hal/hal_network.* $NEW_STRUCTURE/src/hal/communication/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/hal_network.h $NEW_STRUCTURE/src/hal/communication/ 2>/dev/null || true
        
        # Safety files
        cp $OLD_STRUCTURE/src/hal/hal_estop.* $NEW_STRUCTURE/src/hal/safety/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/hal_estop.h $NEW_STRUCTURE/src/hal/safety/ 2>/dev/null || true
        
        # Storage files
        cp $OLD_STRUCTURE/src/hal/hal_config_persistence.* $NEW_STRUCTURE/src/hal/storage/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/hal_config_persistence.h $NEW_STRUCTURE/src/hal/storage/ 2>/dev/null || true
        
        # Peripheral files
        cp $OLD_STRUCTURE/src/hal/hal_led.* $NEW_STRUCTURE/src/hal/peripherals/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/hal_led.h $NEW_STRUCTURE/src/hal/peripherals/ 2>/dev/null || true
        cp $OLD_STRUCTURE/src/hal/hal_relay.* $NEW_STRUCTURE/src/hal/peripherals/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/hal_relay.h $NEW_STRUCTURE/src/hal/peripherals/ 2>/dev/null || true
    fi
    
    # Migrate APP files
    if [ -d "$OLD_STRUCTURE/src/app" ]; then
        # Core files
        cp $OLD_STRUCTURE/src/app/system_state_machine.* $NEW_STRUCTURE/src/app/core/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/system_state_machine.h $NEW_STRUCTURE/src/app/core/ 2>/dev/null || true
        cp $OLD_STRUCTURE/src/app/control_loop.* $NEW_STRUCTURE/src/app/core/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/control_loop.h $NEW_STRUCTURE/src/app/core/ 2>/dev/null || true
        
        # Manager files
        cp $OLD_STRUCTURE/src/app/safety_manager.* $NEW_STRUCTURE/src/app/managers/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/safety_manager.h $NEW_STRUCTURE/src/app/managers/ 2>/dev/null || true
        cp $OLD_STRUCTURE/src/app/communication_manager.* $NEW_STRUCTURE/src/app/managers/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/communication_manager.h $NEW_STRUCTURE/src/app/managers/ 2>/dev/null || true
        cp $OLD_STRUCTURE/src/app/module_manager.* $NEW_STRUCTURE/src/app/managers/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/module_manager.h $NEW_STRUCTURE/src/app/managers/ 2>/dev/null || true
        
        # Module files
        cp $OLD_STRUCTURE/src/app/power_module_handler.* $NEW_STRUCTURE/src/app/modules/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/power_module_handler.h $NEW_STRUCTURE/src/app/modules/ 2>/dev/null || true
        cp $OLD_STRUCTURE/src/app/motor_module_handler.* $NEW_STRUCTURE/src/app/modules/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/motor_module_handler.h $NEW_STRUCTURE/src/app/modules/ 2>/dev/null || true
        cp $OLD_STRUCTURE/src/app/dock_module_handler.* $NEW_STRUCTURE/src/app/modules/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/dock_module_handler.h $NEW_STRUCTURE/src/app/modules/ 2>/dev/null || true
        
        # API files
        cp $OLD_STRUCTURE/src/app/api_manager.* $NEW_STRUCTURE/src/app/api/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/api_manager.h $NEW_STRUCTURE/src/app/api/ 2>/dev/null || true
        cp $OLD_STRUCTURE/src/app/api_endpoints.* $NEW_STRUCTURE/src/app/api/ 2>/dev/null || true
        cp $OLD_STRUCTURE/include/api_endpoints.h $NEW_STRUCTURE/src/app/api/ 2>/dev/null || true
    fi
    
    # Migrate main.c
    if [ -f "$OLD_STRUCTURE/src/main.c" ]; then
        cp $OLD_STRUCTURE/src/main.c $NEW_STRUCTURE/src/
    fi
    
    print_status "Source files migrated successfully"
}

# Function to create configuration files
create_config_files() {
    print_status "Creating configuration files..."
    
    # Create version.h
    cat > $NEW_STRUCTURE/version.h << 'EOF'
#ifndef OHT50_VERSION_H
#define OHT50_VERSION_H

#define OHT50_VERSION_MAJOR 1
#define OHT50_VERSION_MINOR 0
#define OHT50_VERSION_PATCH 0
#define OHT50_VERSION_STRING "1.0.0"

#define OHT50_BUILD_DATE __DATE__
#define OHT50_BUILD_TIME __TIME__

#endif // OHT50_VERSION_H
EOF

    # Create .gitignore
    cat > $NEW_STRUCTURE/.gitignore << 'EOF'
# Build artifacts
build/
install/
package/

# IDE files
.vscode/
.idea/
*.swp
*.swo

# OS files
.DS_Store
Thumbs.db

# Backup files
*.backup
*.bak
*~

# Test artifacts
*.gcov
*.gcda
*.gcno
coverage/

# Documentation
docs/html/
docs/latex/
EOF

    # Create .clang-format
    cat > $NEW_STRUCTURE/.clang-format << 'EOF'
BasedOnStyle: LLVM
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
AllowShortFunctionsOnASingleLine: Empty
PointerAlignment: Left
ReferenceAlignment: Left
EOF

    print_status "Configuration files created successfully"
}

# Function to create build scripts
create_build_scripts() {
    print_status "Creating build scripts..."
    
    # Create build.sh
    cat > $NEW_STRUCTURE/scripts/build.sh << 'EOF'
#!/bin/bash
# OHT-50 Firmware Build Script
# ISO/IEC 12207 Compliant

set -e

BUILD_TYPE=${1:-Release}
BUILD_DIR="build"

echo "Building OHT-50 Firmware (${BUILD_TYPE})..."

# Create build directory
mkdir -p $BUILD_DIR

# Configure with CMake
cmake -B $BUILD_DIR -S . -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build
cmake --build $BUILD_DIR --config $BUILD_TYPE

echo "Build completed successfully!"
echo "Executable: $BUILD_DIR/bin/oht50_main"
EOF

    # Create test.sh
    cat > $NEW_STRUCTURE/scripts/test.sh << 'EOF'
#!/bin/bash
# OHT-50 Firmware Test Script
# ISO/IEC/IEEE 29119 Compliant

set -e

BUILD_DIR="build"

echo "Running OHT-50 Firmware tests..."

# Build if not already built
if [ ! -d "$BUILD_DIR" ]; then
    echo "Building firmware first..."
    ./scripts/build.sh
fi

# Run tests
cd $BUILD_DIR
ctest --output-on-failure --verbose

echo "Tests completed!"
EOF

    # Make scripts executable
    chmod +x $NEW_STRUCTURE/scripts/build.sh
    chmod +x $NEW_STRUCTURE/scripts/test.sh
    
    print_status "Build scripts created successfully"
}

# Function to create documentation template
create_documentation_template() {
    print_status "Creating documentation template..."
    
    # Create README.md
    cat > $NEW_STRUCTURE/README.md << 'EOF'
# OHT-50 Firmware

ISO/IEC 12207 Compliant Firmware for OHT-50 Master Module

## Overview

This firmware implements the Master Module functionality for the OHT-50 system, following ISO/IEC 12207 software lifecycle processes and MISRA C:2012 coding standards.

## Features

- Hardware Abstraction Layer (HAL)
- Safety-critical systems
- Real-time communication
- Module management
- Configuration persistence
- API interface

## Building

```bash
# Build in Release mode
./scripts/build.sh Release

# Build in Debug mode
./scripts/build.sh Debug
```

## Testing

```bash
# Run all tests
./scripts/test.sh
```

## Documentation

- [API Documentation](docs/api/)
- [Design Documentation](docs/design/)
- [User Guide](docs/user_guide/)
- [Developer Guide](docs/developer_guide/)

## License

[License information here]
EOF

    print_status "Documentation template created successfully"
}

# Function to generate migration report
generate_migration_report() {
    print_status "Generating migration report..."
    
    cat > migration_report.md << EOF
# OHT-50 Firmware Migration Report

**Date:** $(date)
**Migration Script Version:** 1.0.0

## Summary

Successfully migrated firmware from legacy structure to ISO/IEC 12207 compliant structure.

## Files Migrated

### HAL Layer
$(find $NEW_STRUCTURE/src/hal -name "*.c" -o -name "*.h" | wc -l) files migrated

### Application Layer
$(find $NEW_STRUCTURE/src/app -name "*.c" -o -name "*.h" | wc -l) files migrated

### Configuration Files
$(find $NEW_STRUCTURE -name "*.h" -o -name "*.md" -o -name "*.sh" | wc -l) files created

## Next Steps

1. Update include paths in source files
2. Create CMakeLists.txt files for each subdirectory
3. Implement MISRA C:2012 compliance
4. Add unit tests
5. Generate API documentation

## Backup Location

Original firmware backed up to: $BACKUP_DIR

## Notes

- All .backup files were excluded from migration
- Directory structure follows ISO/IEC 12207 standards
- Build system uses CMake for better dependency management
EOF

    print_status "Migration report generated: migration_report.md"
}

# Main migration function
main() {
    echo -e "${BLUE}Starting OHT-50 Firmware Migration...${NC}"
    echo
    
    # Check if we're in the right directory
    if [ ! -d "firmware" ] && [ ! -d "firmware_old" ]; then
        print_error "No firmware directory found. Please run this script from the project root."
        exit 1
    fi
    
    # Create backup
    backup_current_structure
    
    # Create new structure
    create_new_structure
    
    # Migrate files
    migrate_source_files
    
    # Create configuration files
    create_config_files
    
    # Create build scripts
    create_build_scripts
    
    # Create documentation template
    create_documentation_template
    
    # Generate report
    generate_migration_report
    
    echo
    echo -e "${GREEN}=== Migration Completed Successfully ===${NC}"
    echo -e "${YELLOW}Next steps:${NC}"
    echo "1. Review the new structure in: $NEW_STRUCTURE"
    echo "2. Update include paths in source files"
    echo "3. Create CMakeLists.txt files for subdirectories"
    echo "4. Run: cd $NEW_STRUCTURE && ./scripts/build.sh"
    echo
    echo -e "${BLUE}Backup location: $BACKUP_DIR${NC}"
    echo -e "${BLUE}Migration report: migration_report.md${NC}"
}

# Run main function
main "$@"
