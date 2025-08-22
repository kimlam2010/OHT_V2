---
title: "Coding Standards"
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

# 📋 **CODING STANDARDS - MISRA C:2012 + AUTOSAR**

**Phiên bản:** 1.0.0  
**Ngày tạo:** 2025-01-27  
**Tuân thủ:** MISRA C:2012, AUTOSAR, ISO 26262, IEC 61508  
**Safety Level:** SIL2

---

## 🎯 **MỤC TIÊU**

Định nghĩa tiêu chuẩn coding cho dự án OHT-50 firmware, đảm bảo:
- **MISRA C:2012 Compliance:** Tuân thủ đầy đủ tiêu chuẩn MISRA
- **AUTOSAR Compliance:** Tuân thủ kiến trúc AUTOSAR
- **Safety Compliance:** Tuân thủ ISO 26262 và IEC 61508
- **Code Quality:** Đảm bảo chất lượng code cao
- **Maintainability:** Dễ bảo trì và mở rộng

---

## 📁 **FILE ORGANIZATION**

### **Directory Structure:**
```
firmware/
├── src/
│   ├── 01-CORE/                     # Core system components
│   ├── 02-HAL/                      # Hardware abstraction layer
│   ├── 03-MODULES/                  # Application modules
│   ├── 04-SERVICES/                 # System services
│   ├── 05-CONTROL/                  # Control systems
│   └── 06-UTILITIES/                # Utility functions
├── include/
│   ├── 01-CORE/                     # Core headers
│   ├── 02-HAL/                      # HAL headers
│   ├── 03-MODULES/                  # Module headers
│   ├── 04-SERVICES/                 # Service headers
│   ├── 05-CONTROL/                  # Control headers
│   └── 06-UTILITIES/                # Utility headers
```

### **File Naming Convention:**
- **Format:** `module_name_verb_noun.c`
- **Example:** `motor_module_handler.c`, `hal_rs485_driver.c`

---

## 🔧 **NAMING CONVENTIONS**

### **Function Naming:**
- **Format:** `Module_Verb_Noun()`
- **Example:** `Motor_Start_Engine()`, `HAL_RS485_Send_Data()`

### **Variable Naming:**
- **Format:** `module_verb_noun`
- **Example:** `motor_current_speed`, `hal_rs485_buffer`

### **Constant Naming:**
- **Format:** `MODULE_CONSTANT_NAME`
- **Example:** `MOTOR_MAX_SPEED`, `HAL_RS485_TIMEOUT_MS`

### **Type Naming:**
- **Format:** `module_type_t`
- **Example:** `motor_state_t`, `hal_rs485_config_t`

---

## 📝 **HEADER FILE TEMPLATE**

### **Header File Structure:**
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

## 📝 **SOURCE FILE TEMPLATE**

### **Source File Structure:**
```c
/**
 * @file module_name.c
 * @brief Module implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @author Firmware Team
 * @license MIT
 * @compliance MISRA C:2012, AUTOSAR
 * @safety SIL2
 */

#include "module_name.h"
#include "hal_common.h"
#include "safety_manager.h"

/* Module private variables */
static module_state_t g_module_state = MODULE_STATE_IDLE;
static uint32_t g_module_error_count = 0U;

/* Module private functions */
static module_result_t Module_Validate_Parameters(const module_config_t* config);
static void Module_Update_State(module_state_t new_state);

/**
 * @brief Initialize module
 * @param config Pointer to module configuration
 * @return MODULE_RESULT_SUCCESS on success, error code otherwise
 */
module_result_t Module_Init(const module_config_t* config)
{
    module_result_t result = MODULE_RESULT_SUCCESS;
    
    /* Parameter validation */
    if (NULL == config)
    {
        result = MODULE_RESULT_INVALID_PARAMETER;
    }
    else
    {
        /* Initialize hardware */
        result = HAL_GPIO_Init_Module_Pins();
        
        if (MODULE_RESULT_SUCCESS == result)
        {
            /* Initialize module state */
            g_module_state = MODULE_STATE_IDLE;
            g_module_error_count = 0U;
        }
    }
    
    return result;
}
```

---

## 🚨 **MISRA C:2012 RULES**

### **Critical Rules (Must Follow):**

#### **Rule 1.1 - Traceability:**
- All code shall be traceable to documented requirements
- Every function must have clear purpose documented

#### **Rule 2.1 - Unreachable Code:**
- A project shall not contain unreachable code
- Remove all dead code and unused functions

#### **Rule 3.1 - Comments:**
- The character sequences /* and // shall not be used within a comment
- Use proper comment format

#### **Rule 8.1 - Types:**
- Types shall be explicitly specified
- Avoid implicit type conversions

#### **Rule 8.2 - Function Types:**
- Function types shall be in prototype form
- Always declare function parameters

#### **Rule 8.3 - Compatible Types:**
- All declarations of the same object or function shall have compatible types
- Ensure consistency across declarations

#### **Rule 10.1 - Essential Types:**
- Operands shall not be of an inappropriate essential type
- Use appropriate data types

#### **Rule 10.2 - Character Operations:**
- Expressions of essentially character type shall not be used inappropriately
- Be careful with character arithmetic

#### **Rule 10.3 - Type Assignment:**
- The value of an expression shall not be assigned to an object with a narrower essential type
- Avoid data loss through assignment

### **Important Rules (Should Follow):**

#### **Rule 11.1 - Casting:**
- Conversions shall not be performed between a pointer to object type and a pointer to a different object type
- Use proper casting

#### **Rule 12.1 - Control Flow:**
- The value of an expression shall be the same under any order of evaluation that the standard permits
- Avoid side effects in expressions

#### **Rule 13.1 - Side Effects:**
- The value of an expression and its persistent side effects shall be the same under all permitted evaluation orders
- Be careful with side effects

---

## ⚠️ **SAFETY REQUIREMENTS**

### **SIL2 Compliance:**
- All safety-critical functions must have error handling
- Use defensive programming techniques
- Implement proper error recovery mechanisms

### **Error Handling:**
```c
/* Example of proper error handling */
module_result_t Module_Function(void)
{
    module_result_t result = MODULE_RESULT_SUCCESS;
    
    /* Parameter validation */
    if (NULL == parameter)
    {
        result = MODULE_RESULT_INVALID_PARAMETER;
    }
    else
    {
        /* Function implementation */
        result = Module_Internal_Function(parameter);
        
        if (MODULE_RESULT_SUCCESS != result)
        {
            /* Error handling */
            Module_Handle_Error(result);
        }
    }
    
    return result;
}
```

### **State Validation:**
```c
/* Example of state validation */
bool Module_Is_State_Valid(module_state_t state)
{
    bool is_valid = false;
    
    if ((state >= MODULE_STATE_MIN) && (state <= MODULE_STATE_MAX))
    {
        is_valid = true;
    }
    
    return is_valid;
}
```

---

## 🔍 **CODE REVIEW CHECKLIST**

### **Structure Review:**
- [ ] File organization follows directory structure
- [ ] File naming follows convention
- [ ] Header guards are properly implemented
- [ ] Include dependencies are minimal

### **Naming Review:**
- [ ] Function names follow convention
- [ ] Variable names follow convention
- [ ] Constant names follow convention
- [ ] Type names follow convention

### **MISRA Review:**
- [ ] All MISRA rules are followed
- [ ] No unreachable code
- [ ] Proper type usage
- [ ] No implicit conversions

### **Safety Review:**
- [ ] Error handling is implemented
- [ ] State validation is performed
- [ ] Defensive programming is used
- [ ] Safety interlocks are in place

### **Documentation Review:**
- [ ] Function documentation is complete
- [ ] Parameter validation is documented
- [ ] Error codes are documented
- [ ] Safety considerations are documented

---

## 📊 **COMPLIANCE METRICS**

### **MISRA C:2012 Compliance:**
- **Critical Rules:** 100% compliance required
- **Important Rules:** 95% compliance required
- **Advisory Rules:** 90% compliance required

### **Safety Compliance:**
- **SIL2 Requirements:** 100% compliance required
- **Error Handling:** 100% coverage required
- **State Validation:** 100% coverage required

### **Code Quality:**
- **Function Complexity:** Max 20 lines per function
- **File Size:** Max 1000 lines per file
- **Comment Ratio:** Min 20% comments
- **Test Coverage:** Min 90% coverage

---

## 🛠️ **TOOLS & VALIDATION**

### **Static Analysis Tools:**
- **MISRA Checker:** PC-lint Plus
- **Code Quality:** SonarQube
- **Safety Analysis:** Coverity

### **Build Tools:**
- **Compiler:** GCC with MISRA flags
- **Linker:** GNU LD
- **Build System:** Make

### **Testing Tools:**
- **Unit Testing:** Unity Framework
- **Coverage:** GCOV
- **Memory Check:** Valgrind

---

## 📚 **REFERENCES**

### **Standards:**
- **MISRA C:2012:** Guidelines for the Use of the C Language in Critical Systems
- **AUTOSAR:** Automotive Open System Architecture
- **ISO 26262:** Road vehicles - Functional safety
- **IEC 61508:** Functional safety of electrical/electronic/programmable electronic safety-related systems

### **Best Practices:**
- **Defensive Programming:** Programming for reliability
- **Error Handling:** Comprehensive error management
- **State Machines:** Proper state management
- **Safety Interlocks:** Safety-critical system design

---

**Changelog v1.0.0:**
- ✅ Created coding standards template
- ✅ Defined MISRA C:2012 rules
- ✅ Specified naming conventions
- ✅ Added safety requirements
- ✅ Created review checklist

**🚨 Lưu ý:** Tất cả code phải tuân thủ các tiêu chuẩn này để đảm bảo chất lượng và an toàn.
