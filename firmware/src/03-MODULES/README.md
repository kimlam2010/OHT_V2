# 🧩 **03-MODULES - Application Modules**

**Phiên bản:** 1.0.0  
**Ngày tạo:** 2025-01-27  
**Tuân thủ:** MISRA C:2012, AUTOSAR  
**Safety Level:** SIL2

---

## 🎯 **MỤC TIÊU**

Thư mục này chứa các module ứng dụng chính của hệ thống OHT-50:
- **Motor Module:** Điều khiển động cơ và chuyển động
- **Dock Module:** Quản lý docking và undocking
- **Power Module:** Quản lý nguồn điện và pin
- **IO Module:** Quản lý input/output digital
- **LED Module:** Quản lý đèn LED status

---

## 📁 **CẤU TRÚC THƯ MỤC**

```
03-MODULES/
├── 03-01-Motor/                    # Motor control module
│   └── motor_module_handler.c      # Motor control logic
├── 03-02-Dock/                     # Docking module
│   └── dock_module_handler.c       # Docking control logic
├── 03-03-Power/                    # Power management
│   └── power_module_handler.c      # Power control logic
├── 03-04-IO/                       # IO management
│   ├── di_do_module_handler.c      # Digital IO control
│   └── io_module_handler.c         # IO management logic
└── 03-05-LED/                      # LED management
    └── led_manager.c               # LED control logic
```

---

## 🔧 **CODING STANDARDS**

### **Naming Convention:**
- **Files:** `module_name_handler.c`
- **Functions:** `Module_Verb_Noun()`
- **Variables:** `module_verb_noun`

### **MISRA C:2012 Compliance:**
- Rule 10.1: Operands shall not be of an inappropriate essential type
- Rule 10.2: Expressions of essentially character type shall not be used inappropriately in addition and subtraction operations
- Rule 10.3: The value of an expression shall not be assigned to an object with a narrower essential type

### **Safety Requirements:**
- SIL2 compliance for all module functions
- State validation before operations
- Error recovery mechanisms
- Safety interlocks

---

## 📋 **DEPENDENCIES**

### **Internal Dependencies:**
- `01-CORE/` - Core system components
- `02-HAL/` - Hardware abstraction layer
- `04-SERVICES/` - System services

### **External Dependencies:**
- Motor control algorithms
- Safety protocols
- Communication protocols

---

## 🚨 **CRITICAL FUNCTIONS**

### **Motor Module:**
- `Motor_Init()` - Initialize motor module
- `Motor_Start()` - Start motor operation
- `Motor_Stop()` - Stop motor operation
- `Motor_Set_Speed()` - Set motor speed

### **Dock Module:**
- `Dock_Init()` - Initialize docking module
- `Dock_Start()` - Start docking sequence
- `Dock_Stop()` - Stop docking sequence
- `Dock_Get_Status()` - Get docking status

### **Power Module:**
- `Power_Init()` - Initialize power module
- `Power_Get_Level()` - Get battery level
- `Power_Get_Status()` - Get power status
- `Power_Shutdown()` - Power shutdown

### **IO Module:**
- `IO_Init()` - Initialize IO module
- `IO_Set_Output()` - Set digital output
- `IO_Get_Input()` - Get digital input
- `IO_Get_Status()` - Get IO status

### **LED Module:**
- `LED_Init()` - Initialize LED module
- `LED_Set_State()` - Set LED state
- `LED_Set_Pattern()` - Set LED pattern
- `LED_Get_Status()` - Get LED status

---

## 🔍 **TESTING REQUIREMENTS**

### **Unit Tests:**
- Module initialization
- State transitions
- Error handling
- Safety interlocks

### **Integration Tests:**
- Module interactions
- System integration
- Performance validation

### **Safety Tests:**
- Emergency stop scenarios
- Fault handling
- Recovery procedures

---

## 📚 **DOCUMENTATION**

### **Required Documentation:**
- Module interface specifications
- State machine diagrams
- Error codes và handling
- Safety procedures

---

## ⚠️ **SAFETY CONSIDERATIONS**

### **Critical Safety Functions:**
- All modules must respect E-Stop commands
- State validation before operations
- Timeout mechanisms for all operations
- Error recovery procedures

### **Module Interactions:**
- Inter-module communication safety
- Resource sharing protocols
- Conflict resolution mechanisms

---

**Changelog v1.0.0:**
- ✅ Created modules structure
- ✅ Defined coding standards
- ✅ Specified dependencies
- ✅ Listed critical functions
- ✅ Added safety considerations

**🚨 Lưu ý:** Tất cả modules phải tuân thủ MISRA C:2012 và có safety interlocks.
