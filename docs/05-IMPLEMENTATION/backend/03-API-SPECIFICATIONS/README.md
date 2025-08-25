# 03-API-SPECIFICATIONS - API Specifications

**Phiên bản:** v2.0  
**Cập nhật:** 2025-01-28

**Mục đích:** Tài liệu đặc tả API và giao tiếp hệ thống OHT-50 Backend

---

## 📋 **Nội dung thư mục**

### **🔌 API Specifications**
- **README.md** - Tổng quan API (CORE)
- **REST_API_SPECS.md** - REST API specifications
- **WEBSOCKET_API_SPECS.md** - WebSocket API specifications
- **API_EXAMPLES.md** - Ví dụ sử dụng API

### **📡 Module APIs**
- **CONFIG_API.md** - API cấu hình hệ thống
- **TELEMETRY_API.md** - API telemetry data
- **WEBSOCKET_EVENTS.md** - WebSocket events
- **SAFETY_API.md** - API hệ thống an toàn
- **MASTER_CONTROL_API.md** - API điều khiển chính
- **POWER_API.md** - API quản lý nguồn
- **DOCK_LOCATION_API.md** - API dock và định vị
- **DC_MOTOR_API.md** - API điều khiển motor DC
- **SYSTEM_STATUS_API.md** - API trạng thái hệ thống
- **MODULE_DISCOVERY_API.md** - API khám phá module
- **INTEGRATION_CENTER.md** - Tích hợp với Center

---

## 🎯 **Hướng dẫn đọc**

### **Cho Developer mới:**
1. **README.md** - Hiểu tổng quan API
2. **REST_API_SPECS.md** - Hiểu REST API
3. **API_EXAMPLES.md** - Xem ví dụ sử dụng

### **Cho API Developer:**
1. **README.md** - Hiểu tổng quan
2. **REST_API_SPECS.md** - Chi tiết REST API
3. **WEBSOCKET_API_SPECS.md** - Chi tiết WebSocket
4. **Module APIs** - API cho từng module

### **Cho QA Engineer:**
1. **API_EXAMPLES.md** - Ví dụ testing
2. **Module APIs** - API endpoints cần test
3. **WEBSOCKET_EVENTS.md** - Events cần verify

### **Cho System Integrator:**
1. **INTEGRATION_CENTER.md** - Tích hợp với Center
2. **WEBSOCKET_EVENTS.md** - Real-time events
3. **Module APIs** - API cho từng module

---

## ⚡ **Quick Links**

- **Architecture:** [../02-ARCHITECTURE/](../02-ARCHITECTURE/)
- **Implementation:** [../04-IMPLEMENTATION/](../04-IMPLEMENTATION/)
- **Testing:** [../09-TESTING/](../09-TESTING/)
- **Deployment:** [../06-DEPLOYMENT/](../06-DEPLOYMENT/)

---

## 📊 **Thời gian đọc ước tính**

| Tài liệu | Thời gian | Mục đích |
|----------|-----------|----------|
| **README.md** | 30-45 phút | Hiểu tổng quan API |
| **REST_API_SPECS.md** | 60-90 phút | Hiểu REST API |
| **WEBSOCKET_API_SPECS.md** | 45-60 phút | Hiểu WebSocket API |
| **API_EXAMPLES.md** | 30-45 phút | Xem ví dụ sử dụng |
| **Module APIs** | 15-30 phút mỗi API | Hiểu API từng module |

**Tổng thời gian:** 4-6 giờ để hiểu toàn bộ API specifications

---

## 🔑 **Tài liệu CORE (Quan trọng nhất)**

### **README.md** 🔴 **CRITICAL**
- Tổng quan API architecture
- Module address mapping (0x01-0x05)
- API coverage và implementation status
- Testing procedures
- Integration points

### **REST_API_SPECS.md** 🔴 **CRITICAL**
- Complete REST API documentation
- Endpoint specifications
- Request/response schemas
- Authentication và authorization
- Error handling

### **WEBSOCKET_API_SPECS.md** 🔴 **CRITICAL**
- Real-time communication
- Event specifications
- Message formats
- Connection management
- Error handling

---

## 🧪 **Testing Resources**

### **API Testing Tools:**
- **Postman Collection:** [../09-TESTING/postman_collection.json](../09-TESTING/postman_collection.json)
- **Insomnia Workspace:** [../09-TESTING/INSOMNIA_WORKSPACE.json](../09-TESTING/INSOMNIA_WORKSPACE.json)
- **cURL Scripts:** [../09-TESTING/curl_suite.sh](../09-TESTING/curl_suite.sh)

### **Testing Guidelines:**
- **Unit Testing:** [../09-TESTING/UNIT_TESTING.md](../09-TESTING/UNIT_TESTING.md)
- **Integration Testing:** [../09-TESTING/INTEGRATION_TESTING.md](../09-TESTING/INTEGRATION_TESTING.md)
- **Performance Testing:** [../09-TESTING/PERFORMANCE_TESTING.md](../09-TESTING/PERFORMANCE_TESTING.md)

---

**Changelog v2.0:**
- ✅ Added comprehensive API specifications structure
- ✅ Added module-specific API documentation
- ✅ Added testing resources and guidelines
- ✅ Added quick links and navigation
- ✅ Added reading time estimates
- ✅ Added critical documentation identification

**Changelog v1.0:**
- ✅ Initial API specifications structure
- ✅ Basic module API documentation
- ✅ Core API endpoints specification
