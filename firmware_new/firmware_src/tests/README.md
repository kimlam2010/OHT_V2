# 🔧 Register Info Unit Tests - Issue #179

## 📋 **Tổng quan**

Đây là unit tests cho Register Info functionality được implement trong Issue #179. Tests này validate register address và mode support cho OHT-50 Master Module.

## 🎯 **Mục tiêu**

- ✅ Validate register_info_t structure implementation
- ✅ Test register access control và validation
- ✅ Verify register value validation
- ✅ Test safety critical register detection
- ✅ Validate communication manager integration

## 🏗️ **Cấu trúc Tests**

### **Test Categories:**

1. **Register Info Retrieval** (`test_get_register_info`)
   - Test valid register info lookup
   - Test invalid module/register addresses
   - Validate register properties

2. **Access Control** (`test_validate_register_access`)
   - Test user/admin/system access levels
   - Test read/write permission validation
   - Test access denial scenarios

3. **Value Validation** (`test_validate_register_value`)
   - Test min/max value constraints
   - Test invalid value rejection
   - Test range validation

4. **Mode Validation** (`test_register_mode_validation`)
   - Test register mode constants
   - Test invalid mode rejection

5. **Safety Critical Detection** (`test_safety_critical_detection`)
   - Test safety critical register identification
   - Test non-safety register detection

6. **Support Validation** (`test_register_support_validation`)
   - Test read/write support detection
   - Test operation compatibility

## 🚀 **Cách sử dụng**

### **Build Tests:**
```bash
cd firmware_new/firmware_src/tests
make
```

### **Run Tests:**
```bash
make test
```

### **Clean Build:**
```bash
make clean
```

### **Show Help:**
```bash
make help
```

## 📊 **Test Results**

Tests sẽ output:
- ✅ **PASS**: Test case thành công
- ❌ **FAIL**: Test case thất bại
- 📊 **Summary**: Tổng kết kết quả

### **Expected Output:**
```
🧪 Register Info Unit Tests (Issue #179)
========================================

🔍 Testing get_register_info()...
✅ PASS: Power module battery voltage register info
✅ PASS: Register address match
✅ PASS: Battery voltage is read-only
...

📊 Test Results:
✅ Passed: 25
❌ Failed: 0
📈 Total: 25

🎉 All tests passed! Register Info implementation is working correctly.
```

## 🔧 **Implementation Details**

### **Files Tested:**
- `register_info.c` - Register info implementation
- `register_info.h` - Register info header
- `register_map.h` - Register map definitions
- `communication_manager.c` - Communication manager integration
- `power_module_handler.c` - Module handler integration

### **Test Coverage:**
- ✅ Register info structure validation
- ✅ Access control validation
- ✅ Value range validation
- ✅ Mode validation
- ✅ Safety critical detection
- ✅ Communication manager integration
- ✅ Module handler integration

## 🐛 **Troubleshooting**

### **Common Issues:**

1. **Compilation Errors:**
   ```bash
   # Check include paths
   make clean && make
   ```

2. **Missing Dependencies:**
   ```bash
   # Install required packages
   sudo apt-get install build-essential
   ```

3. **Test Failures:**
   ```bash
   # Check implementation files
   # Verify register map definitions
   # Check access control logic
   ```

## 📝 **Adding New Tests**

Để thêm test cases mới:

1. **Tạo test function:**
   ```c
   void test_new_functionality(void) {
       printf("\n🔍 Testing new functionality...\n");
       
       // Test cases
       ASSERT(condition, "Test description");
   }
   ```

2. **Call trong main:**
   ```c
   int main(void) {
       // ... existing tests ...
       test_new_functionality();
       
       // ... rest of main ...
   }
   ```

3. **Rebuild và test:**
   ```bash
   make clean && make test
   ```

## 📚 **Related Documentation**

- [Issue #179](https://github.com/kimlam2010/OHT_V2/issues/179) - Register Address and Mode Support
- [Register Map Documentation](../../include/register_map.h)
- [Communication Manager Documentation](../app/managers/communication_manager.h)
- [Module Handler Documentation](../app/modules/power_module_handler.h)

## 🎯 **Success Criteria**

Tests được coi là thành công khi:
- ✅ Tất cả test cases pass (100% success rate)
- ✅ Không có compilation errors
- ✅ Không có memory leaks
- ✅ Performance trong acceptable limits

---

**📅 Last Updated:** 2025-01-28  
**👥 Team:** FW  
**🏷️ Issue:** #179
