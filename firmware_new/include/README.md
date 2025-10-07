# OHT-50 Firmware Public Headers

Public header files for OHT-50 firmware. These headers define the public API and interfaces.

## Header Files

### Core Headers

#### `constants.h`
Global constants and definitions used throughout the firmware.
- System-wide constants
- Hardware specifications
- Configuration defaults
- Protocol definitions

**Size:** 7,300 bytes

#### `register_map.h`
Register mapping and definitions for hardware communication.
- Hardware register addresses
- Register bit fields
- Register access macros
- Module register definitions

**Size:** 30,607 bytes (largest header)

#### `safety_types.h`
Safety system type definitions and enumerations.
- Safety states
- Error codes
- Safety configuration types
- Safety status structures

**Size:** 4,908 bytes

#### `version.h`
Firmware version information.
- Version numbers
- Build information
- Compatibility data

**Size:** 272 bytes

## Directory Structure

```
include/
├── constants.h          # Global constants
├── register_map.h       # Hardware registers
├── safety_types.h       # Safety definitions
├── version.h            # Version info
├── CMakeLists.txt       # CMake config
└── README.md            # This file
```

**Note:** All empty subdirectories and unused headers have been removed.

## Usage Guidelines

### Including Headers

**From source files:**
```c
#include "constants.h"
#include "register_map.h"
#include "safety_types.h"
#include "version.h"
```

**From CMake:**
```cmake
target_include_directories(target_name PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

### Header Organization

1. **Public headers:** Place in `include/`
2. **Private headers:** Keep with implementation in `src/`
3. **Test headers:** Place in `tests/`

### Header Dependencies

**Dependency Tree:**
```
constants.h (no dependencies)
    ↓
safety_types.h (depends on constants.h)
    ↓
register_map.h (depends on constants.h, safety_types.h)
    ↓
Application headers
```

## Header Naming Conventions

### File Names
- **Lowercase:** `module_name.h`
- **Descriptive:** `safety_types.h` not `safety.h`
- **Specific:** `register_map.h` not `registers.h`

### Include Guards
```c
#ifndef MODULE_NAME_H
#define MODULE_NAME_H

// Header content

#endif // MODULE_NAME_H
```

### Documentation
All public headers should include:
- File description
- Author information
- Date created/modified
- Doxygen comments

## Header Quality Standards

### Include Guards
✅ All headers must have include guards

### Forward Declarations
✅ Use forward declarations when possible

### Minimal Dependencies
✅ Include only what you need

### Documentation
✅ Document all public APIs

### No Implementation
❌ Headers should not contain implementation (except inline functions)

## Maintenance

### Adding New Headers
1. Create header in appropriate directory
2. Add include guards
3. Document the header
4. Update this README
5. Add to CMakeLists.txt if needed

### Removing Headers
1. Check for usage: `grep -r "header_name.h" .`
2. Remove references
3. Remove header file
4. Update CMakeLists.txt
5. Update this README

### Checking for Unused Headers
```bash
# Find headers not included anywhere
for h in include/*.h; do
    name=$(basename "$h")
    if ! grep -r "$name" src/ tests/ >/dev/null 2>&1; then
        echo "Unused: $name"
    fi
done
```

## Header Analysis

### Total Size
- **Total:** ~43 KB
- **Largest:** `register_map.h` (30 KB)
- **Smallest:** `version.h` (272 bytes)

### Usage Status
- ✅ `constants.h` - Actively used (2 references)
- ✅ `register_map.h` - Actively used (2 references)
- ✅ `safety_types.h` - Actively used (3 references)
- ✅ `version.h` - Actively used (build-time generation)

## Best Practices

### Header Design
1. **Single Responsibility:** Each header has one clear purpose
2. **Minimal Coupling:** Reduce dependencies between headers
3. **Clear Naming:** Names reflect content
4. **Well Documented:** All public interfaces documented

### Include Order
```c
// 1. System headers
#include <stdint.h>
#include <stdbool.h>

// 2. Project headers
#include "constants.h"
#include "safety_types.h"

// 3. Local headers
#include "module_specific.h"
```

### Avoiding Circular Dependencies
- Use forward declarations
- Split headers if needed
- Consider dependency inversion

## Compatibility

### C Standard
- **Minimum:** C11
- **Features:** stdint.h, stdbool.h, stdatomic.h

### Platform
- **Linux:** Primary target
- **POSIX:** POSIX API usage
- **GCC:** GCC extensions acceptable

## Related Documentation

- `/docs/` - Detailed documentation
- `CODE_STRUCTURE.md` - Overall code structure
- `src/README.md` - Source code organization

---

**Last Updated:** 2025-10-07  
**Version:** 1.0


