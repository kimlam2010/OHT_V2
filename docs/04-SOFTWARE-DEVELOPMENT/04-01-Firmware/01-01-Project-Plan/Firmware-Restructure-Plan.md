---
title: "Firmware Restructure Plan"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "Firmware Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['software-development']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# 📋 **KẾ HOẠCH TÁI CẤU TRÚC FIRMWARE - MISRA C:2012 + AUTOSAR**

**Phiên bản:** 1.0.0  
**Ngày tạo:** 2025-01-27  
**Tác giả:** Firmware Team  
**Phê duyệt:** CTO  
**Tuân thủ:** MISRA C:2012, AUTOSAR, ISO 26262, IEC 61508

---

## 🎯 **MỤC TIÊU**

Tái cấu trúc thư mục `firmware/src/` theo tiêu chuẩn MISRA C:2012 và AUTOSAR để đảm bảo:
- **Tính chuyên nghiệp:** Cấu trúc rõ ràng, dễ hiểu
- **Maintainability:** Dễ bảo trì và mở rộng
- **Safety Compliance:** Tuân thủ tiêu chuẩn an toàn
- **Code Quality:** MISRA C:2012 compliant

---

## 📊 **PHÂN TÍCH HIỆN TRẠNG**

### **Cấu trúc hiện tại:**
```
firmware/
├── src/
│   ├── app/                    # 29 files (Application layer)
│   ├── hal/                    # 12 files (Hardware abstraction)
│   └── main.c                  # Main entry point
├── include/                    # 35 header files
├── tests/                      # Test files
├── tools/                      # Build tools
└── build/                      # Build artifacts
```

### **Vấn đề hiện tại:**
1. **Thiếu phân lớp rõ ràng:** Tất cả app files trong 1 thư mục
2. **Không tuân thủ MISRA:** Thiếu naming conventions chuẩn
3. **Thiếu documentation:** Không có module headers
4. **Dependencies phức tạp:** Include paths không rõ ràng
5. **Backup files:** Có nhiều file .backup không cần thiết

---

## 🏗️ **CẤU TRÚC MỚI (MISRA C:2012 + AUTOSAR)**

### **1. Cấu trúc thư mục:**
```
firmware/
├── src/
│   ├── 01-CORE/                     # Core system components
│   │   ├── 01-01-Main/
│   │   │   ├── main.c
│   │   │   └── system_init.c
│   │   ├── 01-02-State-Machine/
│   │   │   ├── system_state_machine.c
│   │   │   └── state_transitions.c
│   │   └── 01-03-Configuration/
│   │       ├── config_system.c
│   │       └── system_config.c
│   ├── 02-HAL/                      # Hardware Abstraction Layer
│   │   ├── 02-01-Communication/
│   │   │   ├── hal_rs485.c
│   │   │   ├── hal_network.c
│   │   │   └── hal_usb_debug.c
│   │   ├── 02-02-IO-Devices/
│   │   │   ├── hal_gpio.c
│   │   │   ├── hal_led.c
│   │   │   └── hal_relay.c
│   │   ├── 02-03-Sensors/
│   │   │   └── hal_lidar.c
│   │   ├── 02-04-Safety/
│   │   │   └── hal_estop.c
│   │   └── 02-05-System/
│   │       ├── hal_common.c
│   │       ├── hal_config_persistence.c
│   │       └── hal_ota_update.c
│   ├── 03-MODULES/                  # Application modules
│   │   ├── 03-01-Motor/
│   │   │   └── motor_module_handler.c
│   │   ├── 03-02-Dock/
│   │   │   └── dock_module_handler.c
│   │   ├── 03-03-Power/
│   │   │   └── power_module_handler.c
│   │   ├── 03-04-IO/
│   │   │   ├── di_do_module_handler.c
│   │   │   └── io_module_handler.c
│   │   └── 03-05-LED/
│   │       └── led_manager.c
│   ├── 04-SERVICES/                 # System services
│   │   ├── 04-01-API/
│   │   │   ├── api_manager.c
│   │   │   ├── api_endpoints.c
│   │   │   └── http_server.c
│   │   ├── 04-02-Communication/
│   │   │   ├── communication_manager.c
│   │   │   ├── network_manager.c
│   │   │   └── websocket_server.c
│   │   ├── 04-03-Safety/
│   │   │   ├── safety_manager.c
│   │   │   └── safety_mechanisms.c
│   │   ├── 04-04-Security/
│   │   │   └── security_manager.c
│   │   └── 04-05-Diagnostics/
│   │       ├── diagnostics_manager.c
│   │       └── performance_manager.c
│   ├── 05-CONTROL/                  # Control systems
│   │   ├── 05-01-Control-Loop/
│   │   │   └── control_loop.c
│   │   ├── 05-02-Performance/
│   │   │   └── performance_metrics.c
│   │   └── 05-03-Module-Management/
│   │       ├── module_manager.c
│   │       └── module_registry.c
│   └── 06-UTILITIES/                # Utility functions
│       ├── 06-01-Mocks/
│       │   └── hal_log_mock.c
│       └── 06-02-Backups/
│           └── [backup files]
├── include/
│   ├── 01-CORE/                     # Core headers
│   ├── 02-HAL/                      # HAL headers
│   ├── 03-MODULES/                  # Module headers
│   ├── 04-SERVICES/                 # Service headers
│   ├── 05-CONTROL/                  # Control headers
│   └── 06-UTILITIES/                # Utility headers
├── tests/                           # Test files
├── tools/                           # Build tools
├── build/                           # Build artifacts
└── docs/                            # Firmware documentation
```

---

## 📋 **MAPPING FILES HIỆN TẠI → CẤU TRÚC MỚI**

### **01-CORE (Core System Components):**
```
src/app/system_state_machine.c → src/01-CORE/01-02-State-Machine/system_state_machine.c
src/app/config_system.c → src/01-CORE/01-03-Configuration/config_system.c
src/main.c → src/01-CORE/01-01-Main/main.c
```

### **02-HAL (Hardware Abstraction Layer):**
```
src/hal/hal_rs485.c → src/02-HAL/02-01-Communication/hal_rs485.c
src/hal/hal_network.c → src/02-HAL/02-01-Communication/hal_network.c
src/hal/hal_usb_debug.c → src/02-HAL/02-01-Communication/hal_usb_debug.c
src/hal/hal_gpio.c → src/02-HAL/02-02-IO-Devices/hal_gpio.c
src/hal/hal_led.c → src/02-HAL/02-02-IO-Devices/hal_led.c
src/hal/hal_relay.c → src/02-HAL/02-02-IO-Devices/hal_relay.c
src/hal/hal_lidar.c → src/02-HAL/02-03-Sensors/hal_lidar.c
src/hal/hal_estop.c → src/02-HAL/02-04-Safety/hal_estop.c
src/hal/hal_common.c → src/02-HAL/02-05-System/hal_common.c
src/hal/hal_config_persistence.c → src/02-HAL/02-05-System/hal_config_persistence.c
src/hal/hal_ota_update.c → src/02-HAL/02-05-System/hal_ota_update.c
```

### **03-MODULES (Application Modules):**
```
src/app/motor_module_handler.c → src/03-MODULES/03-01-Motor/motor_module_handler.c
src/app/dock_module_handler.c → src/03-MODULES/03-02-Dock/dock_module_handler.c
src/app/power_module_handler.c → src/03-MODULES/03-03-Power/power_module_handler.c
src/app/di_do_module_handler.c → src/03-MODULES/03-04-IO/di_do_module_handler.c
src/app/io_module_handler.c → src/03-MODULES/03-04-IO/io_module_handler.c
src/app/led_manager.c → src/03-MODULES/03-05-LED/led_manager.c
```

### **04-SERVICES (System Services):**
```
src/app/api_manager.c → src/04-SERVICES/04-01-API/api_manager.c
src/app/api_endpoints.c → src/04-SERVICES/04-01-API/api_endpoints.c
src/app/http_server.c → src/04-SERVICES/04-01-API/http_server.c
src/app/communication_manager.c → src/04-SERVICES/04-02-Communication/communication_manager.c
src/app/network_manager.c → src/04-SERVICES/04-02-Communication/network_manager.c
src/app/websocket_server.c → src/04-SERVICES/04-02-Communication/websocket_server.c
src/app/safety_manager.c → src/04-SERVICES/04-03-Safety/safety_manager.c
src/app/safety_mechanisms.c → src/04-SERVICES/04-03-Safety/safety_mechanisms.c
src/app/security_manager.c → src/04-SERVICES/04-04-Security/security_manager.c
src/app/diagnostics_manager.c → src/04-SERVICES/04-05-Diagnostics/diagnostics_manager.c
src/app/performance_manager.c → src/04-SERVICES/04-05-Diagnostics/performance_manager.c
```

### **05-CONTROL (Control Systems):**
```
src/app/control_loop.c → src/05-CONTROL/05-01-Control-Loop/control_loop.c
src/app/performance_metrics.c → src/05-CONTROL/05-02-Performance/performance_metrics.c
src/app/module_manager.c → src/05-CONTROL/05-03-Module-Management/module_manager.c
src/app/module_registry.c → src/05-CONTROL/05-03-Module-Management/module_registry.c
```

### **06-UTILITIES (Utility Functions):**
```
src/app/hal_log_mock.c → src/06-UTILITIES/06-01-Mocks/hal_log_mock.c
src/app/*.backup → src/06-UTILITIES/06-02-Backups/
src/hal/hal_relay_old.c → src/06-UTILITIES/06-02-Backups/
```

---

## 🔧 **QUY TẮC ĐẶT TÊN (MISRA C:2012 + AUTOSAR)**

### **File Naming Convention:**
- **Format:** `module_name_verb_noun.c`
- **Example:** `motor_module_handler.c`, `hal_rs485_driver.c`

### **Function Naming Convention:**
- **Format:** `Module_Verb_Noun()`
- **Example:** `Motor_Start_Engine()`, `HAL_RS485_Send_Data()`

### **Variable Naming Convention:**
- **Format:** `module_verb_noun`
- **Example:** `motor_current_speed`, `hal_rs485_buffer`

### **Header File Template (MISRA C:2012):**
```c
/**
 * @file module_name.h
 * @brief Module description
 * @version 1.0.0
 * @date 2025-01-27
 * @author Firmware Team
 * @license MIT
 * @compliance MISRA C:2012, AUTOSAR
 * @safety SIL2
 */

#ifndef MODULE_NAME_H
#define MODULE_NAME_H

/* Include dependencies */
#include <stdint.h>
#include <stdbool.h>

/* Module constants */
#define MODULE_MAX_VALUE 100U

/* Module types */
typedef enum {
    MODULE_STATE_IDLE = 0U,
    MODULE_STATE_ACTIVE = 1U,
    MODULE_STATE_ERROR = 2U
} module_state_t;

/* Module function declarations */
module_result_t Module_Init(void);
module_result_t Module_Start(void);
module_result_t Module_Stop(void);

#endif /* MODULE_NAME_H */
```

---

## 📅 **TIMELINE CHI TIẾT**

### **Day 1: Analysis và Planning**
- [x] Phân tích cấu trúc hiện tại
- [x] Tạo mapping plan
- [ ] Xác định dependencies
- [ ] Tạo kế hoạch chi tiết

### **Day 2: Structure Creation**
- [ ] Tạo tất cả thư mục mới
- [ ] Tạo README.md cho mỗi thư mục
- [ ] Tạo module index files
- [ ] Setup include structure

### **Day 3: File Migration**
- [ ] Di chuyển files theo mapping
- [ ] Cập nhật include paths
- [ ] Thêm MISRA headers
- [ ] Tạo cross-reference index

### **Day 4: Build System Update**
- [ ] Cập nhật Makefile
- [ ] Cập nhật build scripts
- [ ] Test build process
- [ ] Fix build errors

### **Day 5: Validation và Testing**
- [ ] Kiểm tra tất cả includes
- [ ] Validate MISRA compliance
- [ ] Run unit tests
- [ ] Performance testing

### **Day 6: Cleanup và Documentation**
- [ ] Xóa backup files
- [ ] Optimize dependencies
- [ ] Update documentation
- [ ] Final review

---

## 🚨 **QUALITY GATES**

### **Gate 1: Planning Approval**
- [ ] Restructure plan approved by CTO
- [ ] Dependencies mapped
- [ ] Timeline agreed

### **Gate 2: Structure Creation**
- [ ] All folders created theo MISRA/AUTOSAR
- [ ] README files added
- [ ] Module structure implemented

### **Gate 3: Migration Complete**
- [ ] All files moved successfully
- [ ] All includes updated
- [ ] MISRA headers added

### **Gate 4: Build Success**
- [ ] Makefile updated
- [ ] Build successful
- [ ] All tests pass

### **Gate 5: Validation Pass**
- [ ] MISRA compliance check
- [ ] Code review completed
- [ ] Documentation updated

---

## 📊 **DEPENDENCIES ANALYSIS**

### **Critical Dependencies:**
1. **main.c** → system_state_machine.c, config_system.c
2. **api_manager.c** → communication_manager.c, network_manager.c
3. **safety_manager.c** → hal_estop.c, safety_mechanisms.c
4. **module_manager.c** → tất cả module handlers

### **Include Path Updates:**
- Tất cả relative includes cần cập nhật
- Absolute includes cần review
- Circular dependencies cần resolve

---

## 🔍 **RISK ASSESSMENT**

### **High Risk:**
- **Build failures** do include path changes
- **Circular dependencies** trong new structure
- **Missing files** trong migration

### **Medium Risk:**
- **Performance impact** do include changes
- **Code review complexity** tăng
- **Documentation gaps**

### **Low Risk:**
- **Naming convention** adoption
- **Backup file cleanup**

---

## 📞 **COMMUNICATION PLAN**

### **Daily Updates:**
- File: `firmware/docs/01-01-Project-Plan/Daily-Progress-Report.md`
- Content: Progress, build issues, next steps

### **Team Notifications:**
- Slack/Teams notifications cho build issues
- Code review requests
- Documentation updates

### **CTO Reporting:**
- Daily progress reports
- Build failure escalations
- Final validation report

---

## ✅ **APPROVAL STATUS**

- [ ] **CTO Approval:** Pending
- [ ] **Team Review:** Pending
- [ ] **Architecture Review:** Pending
- [ ] **Safety Review:** Pending

---

**Changelog v1.0.0:**
- ✅ Created initial restructure plan
- ✅ Analyzed current structure
- ✅ Defined MISRA/AUTOSAR compliance
- ✅ Created detailed mapping
- ✅ Defined quality gates
- ✅ Created timeline

**🚨 Lưu ý:** Kế hoạch này cần được CTO phê duyệt trước khi bắt đầu implementation.
