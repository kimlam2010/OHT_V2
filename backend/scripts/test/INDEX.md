# 🧪 Test Scripts Index

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Tác giả:** Backend Team  
**Trạng thái:** Active  

---

## 🎯 **MỤC TIÊU**
Test utilities và validation scripts cho OHT-50 Backend System.

## 📋 **DANH SÁCH SCRIPTS**

### **📖 Test Scripts**
- **[test_admin_privileges.py](./test_admin_privileges.py)** - Test admin privileges
- **[test_fixture.py](./test_fixture.py)** - Test fixtures
- **[test_jwt.py](./test_jwt.py)** - Test JWT authentication
- **[test_map_service.py](./test_map_service.py)** - Test map service
- **[test_monitoring.py](./test_monitoring.py)** - Test monitoring
- **[test_simple_auth.py](./test_simple_auth.py)** - Test simple auth
- **[fix_robot_mode_default.py](./fix_robot_mode_default.py)** - Fix robot mode
- **[test_robot_mode_consistency.py](./test_robot_mode_consistency.py)** - Test robot mode

## 🧪 **TEST WORKFLOW**

### **Authentication Testing**
```bash
# 1. Test JWT authentication
python scripts/test/test_jwt.py

# 2. Test simple authentication
python scripts/test/test_simple_auth.py

# 3. Test admin privileges
python scripts/test/test_admin_privileges.py
```

### **Service Testing**
```bash
# 1. Test map service
python scripts/test/test_map_service.py

# 2. Test monitoring
python scripts/test/test_monitoring.py

# 3. Test fixtures
python scripts/test/test_fixture.py
```

### **Robot Mode Testing**
```bash
# 1. Fix robot mode default
python scripts/test/fix_robot_mode_default.py

# 2. Test robot mode consistency
python scripts/test/test_robot_mode_consistency.py
```

## 📊 **TEST CATEGORIES**

### **Authentication Tests**
- **test_jwt.py:** JWT token validation
- **test_simple_auth.py:** Basic authentication
- **test_admin_privileges.py:** Admin role testing

### **Service Tests**
- **test_map_service.py:** Map service functionality
- **test_monitoring.py:** System monitoring
- **test_fixture.py:** Test fixtures validation

### **Robot Tests**
- **fix_robot_mode_default.py:** Robot mode fixes
- **test_robot_mode_consistency.py:** Robot mode consistency

## 🔄 **CHANGELOG**
- **v1.0 (2025-01-28):** Tạo index file ban đầu

---

**📅 Last Updated:** 2025-01-28  
**📁 Location:** `backend/scripts/test/`  
**🔗 Related:** [Scripts Index](../INDEX.md)
