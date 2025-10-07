# 🚀 OHT-50 Backend API

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-01-28  
**Mục tiêu:** Backend API cho hệ thống OHT-50 với focus vào performance, security và real-time communication

---

## 📋 **TỔNG QUAN**

Backend API cung cấp các dịch vụ chính cho hệ thống OHT-50:

- **🤖 Robot Control:** Điều khiển robot, chế độ vận hành, an toàn
- **📊 Telemetry:** Thu thập và xử lý dữ liệu real-time
- **🗺️ Mapping & Localization:** Xử lý bản đồ và định vị
- **🔒 Security:** Authentication, authorization, audit logging
- **🌐 Network:** Quản lý mạng, WiFi AP, fallback connectivity
- **⚡ Real-time:** WebSocket communication, live updates

---

## 🏗️ **KIẾN TRÚC**

```
backend/
├── app/                    # Ứng dụng chính
│   ├── api/               # API endpoints
│   │   ├── v1/           # API version 1
│   │   └── websocket/    # WebSocket handlers
│   ├── core/             # Core functionality
│   ├── models/           # Database models
│   ├── schemas/          # Pydantic schemas
│   └── services/         # Business logic
├── tests/                # Test suite
├── docs/                 # Documentation
├── docker/               # Docker configuration
└── alembic/              # Database migrations
```

---

## 🚀 **QUICK START**

### **1. Cài đặt Dependencies**
```bash
pip install -r requirements.txt
```

### **2. Cấu hình Environment**
```bash
cp env.example .env
# Chỉnh sửa .env với các giá trị phù hợp
```

### **3. Khởi tạo Database**
```bash
alembic upgrade head
```

### **4. Chạy Backend**
```bash
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000 --reload
```

### **5. Truy cập API**
- **API Docs:** http://localhost:8000/docs
- **Health Check:** http://localhost:8000/health

---

## 📚 **API ENDPOINTS**

### **🔐 Authentication**
- `POST /api/v1/auth/login` - Đăng nhập
- `POST /api/v1/auth/register` - Đăng ký
- `POST /api/v1/auth/logout` - Đăng xuất
- `GET /api/v1/auth/me` - Thông tin user

### **🤖 Robot Control**
- `GET /api/v1/robot/status` - Trạng thái robot
- `POST /api/v1/robot/control` - Điều khiển robot
- `POST /api/v1/robot/emergency-stop` - Dừng khẩn cấp
- `GET /api/v1/robot/position` - Vị trí robot

### **📊 Telemetry**
- `GET /api/v1/telemetry/current` - Dữ liệu hiện tại
- `GET /api/v1/telemetry/history` - Lịch sử dữ liệu
- `POST /api/v1/telemetry/collection/start` - Bắt đầu thu thập

### **🗺️ Mapping & Localization**
- `POST /api/v1/map/start-mapping` - Bắt đầu mapping
- `GET /api/v1/map/current` - Bản đồ hiện tại
- `GET /api/v1/localization/position` - Vị trí hiện tại

### **🌐 Network Management**
- `GET /api/v1/network/status` - Trạng thái mạng
- `POST /api/v1/network/ap/start` - Khởi động WiFi AP
- `GET /api/v1/network/wifi/scan` - Quét WiFi

### **🔒 Safety & Security**
- `GET /api/v1/safety/status` - Trạng thái an toàn
- `POST /api/v1/safety/emergency-stop` - Dừng khẩn cấp
- `GET /api/v1/monitoring/health` - Sức khỏe hệ thống

---

## 🧪 **TESTING**

### **Chạy Tests**
```bash
# Tất cả tests
pytest

# Tests cụ thể
pytest tests/unit/
pytest tests/integration/
pytest tests/performance/
pytest tests/security/
```

### **Test Coverage**
```bash
pytest --cov=app tests/
```

---

## 🐳 **DOCKER**

### **Build & Run**
```bash
# Build image
docker build -t oht50-backend .

# Run container
docker run -p 8000:8000 oht50-backend
```

### **Docker Compose**
```bash
docker-compose up -d
```

---

## 📊 **MONITORING**

### **Health Checks**
- **Basic:** `GET /health`
- **Detailed:** `GET /health/detailed`
- **Metrics:** `GET /metrics`

### **Performance Monitoring**
- Response time tracking
- Error rate monitoring
- Resource usage metrics
- WebSocket connection stats

---

## 🔧 **CONFIGURATION**

### **Environment Variables**
```bash
# Database
DATABASE_URL=sqlite+aiosqlite:///./oht50.db

# Security
JWT_SECRET=your-secret-key
JWT_EXPIRY_MINUTES=30

# Firmware Integration
FIRMWARE_URL=http://localhost:8080
USE_MOCK_FIRMWARE=false

# Performance
MAX_CONNECTIONS=100
REQUEST_TIMEOUT=30
```

---

## 📈 **PERFORMANCE REQUIREMENTS**

- **API Response Time:** < 50ms
- **WebSocket Latency:** < 20ms
- **Database Queries:** < 10ms
- **Uptime:** > 99.9%

---

## 🔒 **SECURITY**

- JWT Authentication
- RBAC Authorization
- Rate Limiting
- Input Validation
- Audit Logging
- CORS Protection

---

## 📚 **DOCUMENTATION**

- **API Docs:** `/docs` (Swagger UI)
- **ReDoc:** `/redoc`
- **OpenAPI:** `/openapi.json`

---

## 🤝 **CONTRIBUTING**

1. Fork repository
2. Tạo feature branch
3. Commit changes
4. Push to branch
5. Tạo Pull Request

---

## 📄 **LICENSE**

MIT License - Xem file LICENSE để biết thêm chi tiết.

---

## 📞 **SUPPORT**

- **Issues:** GitHub Issues
- **Documentation:** `/docs`
- **Email:** support@oht50.com

---

**🚀 OHT-50 Backend - Built for Performance, Security & Reliability!**