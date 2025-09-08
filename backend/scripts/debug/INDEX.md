# 🐛 Debug Scripts Index

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Tác giả:** Backend Team  
**Trạng thái:** Active  

---

## 🎯 **MỤC TIÊU**
Scripts debug và troubleshooting cho OHT-50 Backend System.

## 📋 **DANH SÁCH SCRIPTS**

### **📖 Debug Scripts**
- **[debug_500_errors.py](./debug_500_errors.py)** - Debug lỗi 500
- **[debug_auth.py](./debug_auth.py)** - Debug authentication
- **[debug_map_sensors_errors.py](./debug_map_sensors_errors.py)** - Debug map sensors
- **[debug_remaining_500.py](./debug_remaining_500.py)** - Debug remaining 500 errors
- **[debug_robot_api.py](./debug_robot_api.py)** - Debug robot API

## 🔍 **DEBUG WORKFLOW**

### **Error Investigation**
```bash
# 1. Debug 500 errors
python scripts/debug/debug_500_errors.py

# 2. Debug authentication issues
python scripts/debug/debug_auth.py

# 3. Debug robot API
python scripts/debug/debug_robot_api.py

# 4. Debug map sensors
python scripts/debug/debug_map_sensors_errors.py
```

### **System Health Check**
```bash
# 1. Check remaining 500 errors
python scripts/debug/debug_remaining_500.py

# 2. Verify all systems
python scripts/debug/debug_500_errors.py
```

## 📊 **DEBUG CATEGORIES**

### **HTTP Errors**
- **debug_500_errors.py:** Internal server errors
- **debug_remaining_500.py:** Remaining 500 errors

### **Authentication**
- **debug_auth.py:** JWT, RBAC, login issues

### **API Issues**
- **debug_robot_api.py:** Robot control API problems

### **Sensor Issues**
- **debug_map_sensors_errors.py:** Map và sensor errors

## 🔄 **CHANGELOG**
- **v1.0 (2025-01-28):** Tạo index file ban đầu

---

**📅 Last Updated:** 2025-01-28  
**📁 Location:** `backend/scripts/debug/`  
**🔗 Related:** [Scripts Index](../INDEX.md)
