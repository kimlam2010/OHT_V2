# 🚀 OHT-50 Architecture Quick Reference Card

**Version:** v1.0.1 (Domain-Driven Design)  
**Last Updated:** 2025-10-07

---

## 📁 Folder Structure Cheat Sheet

### 🎛️ CORE (`src/app/core/`)
**Purpose:** Independent business logic  
**Rule:** No external dependencies (chỉ HAL)

```
core/
├── state_management/    # System lifecycle
├── safety/              # Safety monitoring (CRITICAL)
└── control/             # Motion control
```

### 🔌 INFRASTRUCTURE (`src/app/infrastructure/`)
**Purpose:** Technical services (I/O, networking)  
**Rule:** Depends only on Core

```
infrastructure/
├── communication/       # RS485/Modbus
├── network/             # WiFi/LAN
└── telemetry/           # Data collection
```

### 🏭 DOMAIN (`src/app/domain/`)
**Purpose:** Business domain logic  
**Rule:** Depends on Infrastructure + Core

```
domain/
├── module_management/   # Discovery & registry
├── power/               # Module 0x02
├── motion/              # Module 0x04
├── safety_module/       # Module 0x03
└── dock/                # Module 0x05
```

### 🔐 APPLICATION (`src/app/application/`)
**Purpose:** Orchestration & coordination  
**Rule:** Depends on all layers

```
application/
├── safety_orchestrator/  # Multi-source safety
└── system_orchestrator/  # System-wide (future)
```

---

## 🔗 Include Path Examples

### From Core:
```c
// Core chỉ include HAL và internal core files
#include "hal_common.h"
#include "system_state_machine.h"
```

### From Infrastructure:
```c
// Infrastructure include Core
#include "../../core/state_management/system_state_machine.h"
#include "hal_rs485.h"
```

### From Domain:
```c
// Domain include Infrastructure + Core
#include "../../infrastructure/communication/communication_manager.h"
#include "../../core/state_management/system_state_machine.h"
#include "hal_common.h"
```

### From Application:
```c
// Application include all layers
#include "../../domain/module_management/module_manager.h"
#include "../../infrastructure/telemetry/telemetry_manager.h"
#include "../../core/safety/safety_monitor.h"
```

---

## 🏗️ Library Linking

### Link New Libraries:
```cmake
target_link_libraries(your_target
    # Unified interfaces (recommended)
    app_core
    app_infrastructure
    app_domain
    app_application
    
    # Or specific libraries
    app_infrastructure_communication
    app_domain_power
)
```

### Link Legacy (Still Works):
```cmake
target_link_libraries(your_target
    app_core
    app_managers    # Redirects to new libs
    app_modules     # Redirects to new libs
)
```

---

## 📊 Dependency Rules

### ✅ ALLOWED:
```
Application → Domain → Infrastructure → Core → HAL
```

### ❌ FORBIDDEN:
```
Core → Infrastructure      ❌ NO!
Infrastructure → Domain    ❌ NO!
Domain → Application       ❌ NO!
```

---

## 🔧 Adding New Code

### Add New Infrastructure Service:
```bash
# 1. Create folder
mkdir src/app/infrastructure/my_service

# 2. Add files
touch src/app/infrastructure/my_service/my_service_manager.c
touch src/app/infrastructure/my_service/my_service_manager.h

# 3. Create CMakeLists.txt
cat > src/app/infrastructure/my_service/CMakeLists.txt << EOF
add_library(app_infrastructure_my_service STATIC
    my_service_manager.c
)
target_include_directories(app_infrastructure_my_service PUBLIC ...)
target_link_libraries(app_infrastructure_my_service ...)
EOF

# 4. Add to infrastructure/CMakeLists.txt
add_subdirectory(my_service)
target_link_libraries(app_infrastructure INTERFACE
    ...existing...
    app_infrastructure_my_service
)
```

### Add New Domain:
```bash
# Similar pattern but in domain/
mkdir src/app/domain/my_domain
# ... follow same steps
```

---

## 🧪 Testing Layers

### Test Specific Layer:
```bash
cd build

# Test infrastructure
make app_infrastructure

# Test specific domain
make app_domain_power

# Test application
make app_application
```

### Test Full Build:
```bash
make -j$(nproc)
./oht50_main
```

---

## 📚 Documentation Files

| File | Purpose |
|------|---------|
| `README.md` | Main project documentation |
| `ARCHITECTURE_v1.0.1.md` | Detailed architecture guide |
| `DOMAIN_DRIVEN_MIGRATION_SUMMARY.md` | Migration details |
| `MIGRATION_LOG_v1.0.1.md` | Core migration log |
| `ARCHITECTURE_QUICK_REFERENCE.md` | This file |

---

## 🚨 Common Mistakes

### ❌ DON'T:
```c
// DON'T add new files vào managers/ hoặc modules/
src/app/managers/new_manager.c        // ❌ DEPRECATED!
src/app/modules/new_module.c          // ❌ DEPRECATED!

// DON'T violate dependency rules
// In core/
#include "../../infrastructure/..."   // ❌ Core can't depend on Infrastructure

// DON'T create circular dependencies
// Infrastructure → Domain → Infrastructure  ❌
```

### ✅ DO:
```c
// DO add to appropriate layer
src/app/infrastructure/my_service/    // ✅ Good!
src/app/domain/my_domain/             // ✅ Good!

// DO follow dependency direction
// In domain/
#include "../../infrastructure/..."   // ✅ OK - Domain can use Infrastructure
#include "../../core/..."             // ✅ OK - Domain can use Core

// DO use INTERFACE libraries
target_link_libraries(my_target app_infrastructure)  // ✅ Clean!
```

---

## 🔍 Finding Code

### "Where is...?"

**Communication với RS485?**
→ `infrastructure/communication/communication_manager.*`

**Module discovery?**
→ `domain/module_management/module_manager.*`

**Power module handler?**
→ `domain/power/power_module_handler.*`

**Safety coordination?**
→ `application/safety_orchestrator/safety_manager.*`

**System state machine?**
→ `core/state_management/system_state_machine.*`

---

## 🎯 Migration Status

| Folder | Status | Note |
|--------|--------|------|
| `core/` | ✅ Migrated | v1.0.1 (earlier) |
| `infrastructure/` | ✅ Migrated | v1.0.1 (now) |
| `domain/` | ✅ Migrated | v1.0.1 (now) |
| `application/` | ✅ Migrated | v1.0.1 (now) |
| `managers/` | ⚠️ DEPRECATED | Compatibility shim |
| `modules/` | ⚠️ DEPRECATED | Compatibility shim |

---

## 📞 Who to Ask?

| Question | Team |
|----------|------|
| Core architecture | CTO / FW Team Lead |
| Infrastructure | EMBED Team |
| Domain logic | FW Domain Team |
| Application orchestration | FW Application Team |
| Build issues | DevOps Team |

---

## 🏆 Best Practices

1. **Read the layer README** before modifying code
2. **Follow dependency rules** strictly
3. **Update tests** when changing code
4. **Document API changes** in headers
5. **Ask before** adding cross-layer dependencies
6. **Use unified interfaces** (`app_infrastructure`) not specific libs

---

**Quick Help:**
```bash
# View architecture
cat src/app/ARCHITECTURE_v1.0.1.md

# View migration details
cat DOMAIN_DRIVEN_MIGRATION_SUMMARY.md

# Build specific layer
cd build && make app_domain

# Run main executable
./build/oht50_main
```

---

**Version:** v1.0.1  
**Last Updated:** 2025-10-07  
**Maintained By:** Firmware Team


