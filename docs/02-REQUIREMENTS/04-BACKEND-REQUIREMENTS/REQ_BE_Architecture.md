# 🌐 REQ_BE_Architecture - Yêu cầu kiến trúc Backend

**Mục đích:** Định nghĩa yêu cầu kiến trúc Backend cho OHT-50 Master Module

---

## 📋 **Thông tin tài liệu:**
- **Phiên bản:** v1.0
- **Ngày tạo:** 2025-01-24
- **Người tạo:** PM Team
- **Trạng thái:** Draft

---

## 🎯 **Mục tiêu:**

### **Yêu cầu chính:**
1. **Layered Architecture** - Kiến trúc phân lớp rõ ràng
2. **Microservices Pattern** - Pattern microservices cho scalability
3. **Real-time Communication** - Giao tiếp thời gian thực
4. **Safety Integration** - Tích hợp hệ thống an toàn
5. **High Availability** - Độ sẵn sàng cao

---

## 🏗️ **Kiến trúc tổng thể:**

### **1. Presentation Layer:**
```
┌─────────────────────────────────────┐
│           API Gateway               │
│  ┌─────────────┬─────────────────┐  │
│  │   REST API  │  WebSocket API  │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

**Yêu cầu:**
- ✅ **FastAPI Framework** - REST API với OpenAPI documentation
- ✅ **WebSocket Support** - Real-time communication
- ✅ **API Versioning** - Version control cho API
- ✅ **Rate Limiting** - Giới hạn request rate
- ✅ **CORS Support** - Cross-origin resource sharing
- ✅ **Module Management API** - Quản lý 5 module bắt buộc + 3 optional + 3 plug-and-play

### **2. Business Logic Layer:**
```
┌─────────────────────────────────────┐
│         Business Services           │
│  ┌─────────────┬─────────────────┐  │
│  │ Mission Svc │  Safety Svc     │  │
│  ├─────────────┼─────────────────┤  │
│  │ State Svc   │  Movement Svc   │  │
│  ├─────────────┼─────────────────┤  │
│  │ Path Svc    │  Config Svc     │  │
│  ├─────────────┼─────────────────┤  │
│  │ Power Svc   │  Location Svc   │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

**Yêu cầu:**
- ✅ **MissionService** - Quản lý lifecycle mission
- ✅ **SafetyService** - Xử lý an toàn và E-Stop (Safety Module 0x03)
- ✅ **StateManagementService** - Quản lý trạng thái hệ thống
- ✅ **MovementService** - Điều khiển chuyển động (Travel Motor Module 0x04)
- ✅ **PathPlanningService** - Lập kế hoạch đường đi
- ✅ **ConfigService** - Quản lý cấu hình
- ✅ **PowerManagementService** - Quản lý nguồn (Power Module 0x02)
- ✅ **LocationService** - Quản lý vị trí và docking (Dock & Location Module 0x05)

### **3. Integration Layer:**
```
┌─────────────────────────────────────┐
│         Integration Services        │
│  ┌─────────────┬─────────────────┐  │
│  │ RS485 Bus   │  Center Client  │  │
│  ├─────────────┼─────────────────┤  │
│  │ HW Comm     │  Telemetry Svc  │  │
│  ├─────────────┼─────────────────┤  │
│  │ Module Mgr  │  Safety Comm    │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

**Yêu cầu:**
- ✅ **RS485Bus** - Giao tiếp với slave modules (5 bắt buộc + 3 optional + 3 plug-and-play)
- ✅ **CenterClient** - Kết nối với Center system
- ✅ **HardwareCommunicationService** - Giao tiếp phần cứng
- ✅ **TelemetryService** - Thu thập và gửi telemetry
- ✅ **ModuleManagerService** - Auto-detection và quản lý module lifecycle
- ✅ **SafetyCommunicationService** - Giao tiếp an toàn với Safety Module

### **4. Data Layer:**
```
┌─────────────────────────────────────┐
│           Data Services             │
│  ┌─────────────┬─────────────────┐  │
│  │ PostgreSQL  │  Redis Cache    │  │
│  ├─────────────┼─────────────────┤  │
│  │ File Storage│  Log Storage    │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

**Yêu cầu:**
- ✅ **PostgreSQL** - Database chính
- ✅ **Redis Cache** - Cache layer
- ✅ **File Storage** - Lưu trữ file
- ✅ **Log Storage** - Lưu trữ log

---

## 🔧 **Yêu cầu kỹ thuật:**

### **1. Framework & Libraries:**
- ✅ **FastAPI** - Web framework chính
- ✅ **SQLAlchemy** - ORM cho database
- ✅ **Pydantic** - Data validation
- ✅ **Redis** - Caching và session
- ✅ **WebSocket** - Real-time communication

### **2. Communication Protocols:**
- ✅ **REST API** - HTTP/HTTPS
- ✅ **WebSocket** - Real-time updates
- ✅ **RS485/Modbus** - Hardware communication
- ✅ **MQTT** - IoT messaging (optional)

### **3. Security Requirements:**
- ✅ **JWT Authentication** - Token-based auth
- ✅ **Role-based Access Control** - RBAC
- ✅ **HTTPS/TLS** - Encrypted communication
- ✅ **Input Validation** - Data sanitization
- ✅ **Rate Limiting** - DDoS protection

### **4. Performance Requirements:**
- ✅ **Response Time** < 100ms cho API calls
- ✅ **Throughput** > 1000 requests/second
- ✅ **Memory Usage** < 512MB
- ✅ **CPU Usage** < 80% under load
- ✅ **Database Connections** < 100 concurrent

---

## 🔄 **Data Flow Architecture:**

### **1. Mission Flow:**
```
Frontend → API Gateway → MissionService → StateManagementService → MovementService → RS485Bus → Hardware
```

### **2. Safety Flow:**
```
Hardware → RS485Bus → SafetyService → StateManagementService → NotificationService → Frontend
```

### **3. Telemetry Flow:**
```
Hardware → RS485Bus → TelemetryService → Database → CenterClient → Center System
```

---

## 🛡️ **Safety & Reliability:**

### **1. Error Handling:**
- ✅ **Graceful Degradation** - Hệ thống vẫn hoạt động khi có lỗi
- ✅ **Circuit Breaker Pattern** - Ngăn cascade failures
- ✅ **Retry Mechanism** - Tự động thử lại khi lỗi
- ✅ **Fallback Strategies** - Chiến lược dự phòng

### **2. Monitoring & Logging:**
- ✅ **Structured Logging** - Log có cấu trúc
- ✅ **Health Checks** - Kiểm tra sức khỏe hệ thống
- ✅ **Metrics Collection** - Thu thập metrics
- ✅ **Alerting System** - Hệ thống cảnh báo

### **3. Backup & Recovery:**
- ✅ **Database Backup** - Sao lưu database
- ✅ **Configuration Backup** - Sao lưu cấu hình
- ✅ **Disaster Recovery** - Khôi phục thảm họa
- ✅ **Data Replication** - Nhân bản dữ liệu

---

## 📊 **Scalability Requirements:**

### **1. Horizontal Scaling:**
- ✅ **Load Balancer** - Cân bằng tải
- ✅ **Service Discovery** - Tìm kiếm service
- ✅ **Container Orchestration** - Điều phối container
- ✅ **Auto-scaling** - Tự động mở rộng

### **2. Vertical Scaling:**
- ✅ **Resource Optimization** - Tối ưu tài nguyên
- ✅ **Memory Management** - Quản lý bộ nhớ
- ✅ **CPU Optimization** - Tối ưu CPU
- ✅ **Database Optimization** - Tối ưu database

---

## 🔗 **Integration Requirements:**

### **1. Hardware Integration:**
- ✅ **RS485 Communication** - Giao tiếp RS485
- ✅ **Modbus Protocol** - Protocol Modbus
- ✅ **Real-time Control** - Điều khiển thời gian thực
- ✅ **Hardware Abstraction** - Trừu tượng hóa phần cứng

### **2. Frontend Integration:**
- ✅ **REST API** - API cho frontend
- ✅ **WebSocket** - Real-time updates
- ✅ **CORS Support** - Cross-origin support
- ✅ **API Documentation** - Tài liệu API

### **3. External System Integration:**
- ✅ **Center System** - Tích hợp với Center
- ✅ **Third-party APIs** - API bên thứ 3
- ✅ **Cloud Services** - Dịch vụ đám mây
- ✅ **IoT Platforms** - Nền tảng IoT

---

## 📋 **Acceptance Criteria:**

### **✅ Functional Requirements:**
- [ ] Tất cả API endpoints hoạt động đúng
- [ ] WebSocket connection ổn định
- [ ] Database operations thành công
- [ ] Hardware communication ổn định
- [ ] Safety system hoạt động đúng

### **✅ Non-Functional Requirements:**
- [ ] Response time < 100ms
- [ ] Uptime > 99.9%
- [ ] Memory usage < 512MB
- [ ] CPU usage < 80%
- [ ] Security compliance đạt chuẩn

---

## 📚 **References:**
- [Backend Implementation](../05-IMPLEMENTATION/backend/)
- [API Specifications](REQ_BE_API_Specifications.md)
- [Data Models](REQ_BE_Data_Models.md)
- [Business Logic](REQ_BE_Business_Logic.md)

---

**Cập nhật lần cuối:** 2025-01-24
**Phiên bản:** v1.0
