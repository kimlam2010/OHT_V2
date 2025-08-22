# 📋 **01-CORE - Core System Components**

**Phiên bản:** 1.0.0  
**Ngày tạo:** 2025-01-27  
**Tuân thủ:** MISRA C:2012, AUTOSAR  
**Safety Level:** SIL2

---

## 🎯 **MỤC TIÊU**

Thư mục này chứa các thành phần cốt lõi của hệ thống firmware, bao gồm:
- **Main Entry Point:** Điểm khởi đầu của ứng dụng
- **State Machine:** Quản lý trạng thái hệ thống
- **Configuration:** Quản lý cấu hình hệ thống

---

## 📁 **CẤU TRÚC THƯ MỤC**

```
01-CORE/
├── 01-01-Main/                    # Main entry point
│   ├── main.c                     # Main application entry
│   └── system_init.c              # System initialization
├── 01-02-State-Machine/           # State management
│   ├── system_state_machine.c     # Main state machine
│   └── state_transitions.c        # State transition logic
└── 01-03-Configuration/           # Configuration management
    ├── config_system.c            # System configuration
    └── system_config.c            # Configuration interface
```

---

## 🔧 **CODING STANDARDS**

### **Naming Convention:**
- **Files:** `module_name_verb_noun.c`
- **Functions:** `Module_Verb_Noun()`
- **Variables:** `module_verb_noun`

### **MISRA C:2012 Compliance:**
- Rule 1.1: All code shall be traceable to documented requirements
- Rule 2.1: A project shall not contain unreachable code
- Rule 3.1: The character sequences /* and // shall not be used within a comment

### **Safety Requirements:**
- SIL2 compliance for all core functions
- Error handling for all critical operations
- State validation before transitions

---

## 📋 **DEPENDENCIES**

### **Internal Dependencies:**
- `02-HAL/` - Hardware abstraction layer
- `03-MODULES/` - Application modules
- `04-SERVICES/` - System services

### **External Dependencies:**
- Standard C library
- Platform-specific headers

---

## 🚨 **CRITICAL FUNCTIONS**

### **Main Entry Point:**
- `main()` - Application entry point
- `System_Init()` - System initialization

### **State Management:**
- `System_State_Get()` - Get current state
- `System_State_Set()` - Set new state
- `System_State_Validate()` - Validate state transition

### **Configuration:**
- `Config_System_Load()` - Load system configuration
- `Config_System_Save()` - Save system configuration
- `Config_System_Validate()` - Validate configuration

---

## 🔍 **TESTING REQUIREMENTS**

### **Unit Tests:**
- State machine transitions
- Configuration validation
- Error handling scenarios

### **Integration Tests:**
- System initialization sequence
- State machine integration
- Configuration persistence

---

## 📚 **DOCUMENTATION**

### **Required Documentation:**
- Function documentation với Doxygen
- State machine diagrams
- Configuration schema
- Error codes và handling

---

**Changelog v1.0.0:**
- ✅ Created core structure
- ✅ Defined coding standards
- ✅ Specified dependencies
- ✅ Listed critical functions

**🚨 Lưu ý:** Tất cả code trong thư mục này phải tuân thủ MISRA C:2012 và safety requirements.
