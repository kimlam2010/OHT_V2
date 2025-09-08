# 🔧 Setup Scripts Index

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Tác giả:** Backend Team  
**Trạng thái:** Active  

---

## 🎯 **MỤC TIÊU**
Scripts khởi tạo và kiểm tra hệ thống backend OHT-50.

## 📋 **DANH SÁCH SCRIPTS**

### **📖 Setup Scripts**
- **[setup_database.py](./setup_database.py)** - Khởi tạo database
- **[check_admin.py](./check_admin.py)** - Kiểm tra admin privileges
- **[check_db.py](./check_db.py)** - Kiểm tra database connection
- **[check_test_db.py](./check_test_db.py)** - Kiểm tra test database
- **[check_users.py](./check_users.py)** - Kiểm tra users

## 🚀 **SETUP WORKFLOW**

### **Initial Setup**
```bash
# 1. Setup database
python scripts/setup/setup_database.py

# 2. Check database connection
python scripts/setup/check_db.py

# 3. Check admin user
python scripts/setup/check_admin.py

# 4. Check all users
python scripts/setup/check_users.py
```

### **Test Environment Setup**
```bash
# 1. Setup test database
python scripts/setup/check_test_db.py

# 2. Verify test environment
python scripts/setup/check_db.py
```

## 📊 **SCRIPT FUNCTIONS**

### **Database Management**
- **setup_database.py:** Tạo tables, indexes, default data
- **check_db.py:** Verify database connection và health
- **check_test_db.py:** Verify test database setup

### **User Management**
- **check_admin.py:** Verify admin user và permissions
- **check_users.py:** List và verify all users

## 🔄 **CHANGELOG**
- **v1.0 (2025-01-28):** Tạo index file ban đầu

---

**📅 Last Updated:** 2025-01-28  
**📁 Location:** `backend/scripts/setup/`  
**🔗 Related:** [Scripts Index](../INDEX.md)
