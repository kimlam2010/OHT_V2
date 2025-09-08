# 🚀 Deployment Scripts Index

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Tác giả:** Backend Team  
**Trạng thái:** Active  

---

## 🎯 **MỤC TIÊU**
Scripts triển khai và chạy hệ thống OHT-50 Backend.

## 📋 **DANH SÁCH SCRIPTS**

### **📖 Deployment Scripts**
- **[run_production.ps1](./run_production.ps1)** - Chạy production server
- **[run_tests.ps1](./run_tests.ps1)** - Chạy tất cả tests
- **[test_api_endpoints.ps1](./test_api_endpoints.ps1)** - Test API endpoints
- **[deploy.sh](./deploy.sh)** - Deploy script (Linux)
- **[deploy_production.sh](./deploy_production.sh)** - Production deploy

## 🚀 **DEPLOYMENT WORKFLOW**

### **Development**
```bash
# 1. Run tests
powershell scripts/deployment/run_tests.ps1

# 2. Test API endpoints
powershell scripts/deployment/test_api_endpoints.ps1

# 3. Start development server
python -m uvicorn app.main:app --reload --host 127.0.0.1 --port 8000
```

### **Production**
```bash
# 1. Deploy to production (Linux)
bash scripts/deployment/deploy_production.sh

# 2. Run production server (Windows)
powershell scripts/deployment/run_production.ps1
```

### **Testing**
```bash
# 1. Run all tests
powershell scripts/deployment/run_tests.ps1

# 2. Test specific endpoints
powershell scripts/deployment/test_api_endpoints.ps1
```

## 📊 **SCRIPT FUNCTIONS**

### **Server Management**
- **run_production.ps1:** Start production server
- **deploy.sh:** Linux deployment
- **deploy_production.sh:** Production deployment

### **Testing**
- **run_tests.ps1:** Execute all tests
- **test_api_endpoints.ps1:** API endpoint testing

## 🔄 **CHANGELOG**
- **v1.0 (2025-01-28):** Tạo index file ban đầu

---

**📅 Last Updated:** 2025-01-28  
**📁 Location:** `backend/scripts/deployment/`  
**🔗 Related:** [Scripts Index](../INDEX.md)
