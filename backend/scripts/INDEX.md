# 🛠️ Backend Scripts Index

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Tác giả:** Backend Team  
**Trạng thái:** Active  

---

## 🎯 **MỤC TIÊU**
Tổng hợp tất cả scripts và utilities cho OHT-50 Backend System, được tổ chức theo chức năng.

## 📁 **CẤU TRÚC SCRIPTS**

### **🔧 Setup Scripts**
- **Mục đích:** Khởi tạo và kiểm tra hệ thống
- **Số lượng:** 5 scripts
- **Trạng thái:** Active
- **[Xem chi tiết →](./setup/INDEX.md)**

### **🐛 Debug Scripts**
- **Mục đích:** Debug và troubleshooting
- **Số lượng:** 5 scripts
- **Trạng thái:** Active
- **[Xem chi tiết →](./debug/INDEX.md)**

### **🧪 Test Scripts**
- **Mục đích:** Test utilities và validation
- **Số lượng:** 8 scripts
- **Trạng thái:** Active
- **[Xem chi tiết →](./test/INDEX.md)**

### **🚀 Deployment Scripts**
- **Mục đích:** Triển khai và chạy hệ thống
- **Số lượng:** 5 scripts
- **Trạng thái:** Active
- **[Xem chi tiết →](./deployment/INDEX.md)**

## 📊 **THỐNG KÊ TỔNG QUAN**

### **Script Statistics**
- **Total Scripts:** 23
- **Python Scripts:** 18
- **PowerShell Scripts:** 3
- **Shell Scripts:** 2

### **Category Distribution**
- **Setup Scripts:** 22%
- **Debug Scripts:** 22%
- **Test Scripts:** 35%
- **Deployment Scripts:** 22%

## 🎯 **QUICK ACCESS**

### **Most Used Scripts**
1. **[setup_database.py](./setup/setup_database.py)** - Database initialization
2. **[run_production.ps1](./deployment/run_production.ps1)** - Production server
3. **[run_tests.ps1](./deployment/run_tests.ps1)** - Run all tests
4. **[check_db.py](./setup/check_db.py)** - Database health check
5. **[test_api_endpoints.ps1](./deployment/test_api_endpoints.ps1)** - API testing

### **Development Workflow**
```bash
# 1. Setup database
python scripts/setup/setup_database.py

# 2. Check system health
python scripts/setup/check_db.py

# 3. Run tests
powershell scripts/deployment/run_tests.ps1

# 4. Start production
powershell scripts/deployment/run_production.ps1
```

## 🔄 **CHANGELOG**
- **v1.0 (2025-01-28):** Tạo index file ban đầu và tổ chức lại scripts

---

**📅 Last Updated:** 2025-01-28  
**📁 Location:** `backend/scripts/`  
**🔗 Related:** [Backend README](../README.md)
