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
# Copy file environment mẫu
cp env.example .env

# Hoặc tạo file .env mới với cấu hình cơ bản
echo "DATABASE_URL=sqlite+aiosqlite:///./oht50.db" > .env
echo "JWT_SECRET=your-secret-key-here" >> .env
echo "USE_MOCK_FIRMWARE=true" >> .env
```

### **3. Khởi tạo Database**
```bash
# Nếu gặp lỗi psycopg2, thử uninstall psycopg2
pip uninstall psycopg2 psycopg2-binary -y

# Kiểm tra file .env có đúng cấu hình SQLite không
Remove-Item .env -Force -ErrorAction SilentlyContinue
New-Item -Path .env -ItemType File -Force
Add-Content -Path .env -Value "DATABASE_URL=sqlite+aiosqlite:///./oht50.db"
Add-Content -Path .env -Value "JWT_SECRET=your-secret-key-here"
Add-Content -Path .env -Value "USE_MOCK_FIRMWARE=true"

# Chạy migration để tạo database
python -m alembic upgrade head

# Nếu vẫn lỗi, thử tạo database trực tiếp
python -c "from app.core.database import init_db; import asyncio; asyncio.run(init_db())"

# Nếu vẫn lỗi, thử chạy backend trực tiếp (sẽ tự tạo database)
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000 --reload
```

### **⚠️ TROUBLESHOOTING**

#### **Lỗi psycopg2 (PostgreSQL driver conflict)**
```bash
# Nếu gặp lỗi "No module named 'psycopg2'", thử:
pip uninstall psycopg2 psycopg2-binary -y
pip install aiosqlite

# Hoặc sử dụng script fix tự động:
python fix_database.py

# Nếu vẫn lỗi, thử cài đặt psycopg2-binary:
pip install psycopg2-binary

# Hoặc thử chạy backend với environment variable:
$env:DATABASE_URL="sqlite+aiosqlite:///./oht50.db"
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000 --reload
```

#### **Lỗi database connection**
```bash
# Nếu database không kết nối được, thử:
# 1. Xóa file .env cũ và tạo mới
Remove-Item .env -Force -ErrorAction SilentlyContinue
New-Item -Path .env -ItemType File -Force
Add-Content -Path .env -Value "DATABASE_URL=sqlite+aiosqlite:///./oht50.db"
Add-Content -Path .env -Value "JWT_SECRET=your-secret-key-here"
Add-Content -Path .env -Value "USE_MOCK_FIRMWARE=true"

# 2. Thử chạy backend trực tiếp
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000 --reload
```

#### **Lỗi "No module named 'app'"**
```bash
# Nếu gặp lỗi module không tìm thấy, thử:
# 1. Chạy không dùng --reload flag
$env:DATABASE_URL="sqlite+aiosqlite:///./oht50.db"
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000

# 2. Hoặc chạy từ thư mục backend
cd backend
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000 --reload
```

#### **Lỗi "Port already in use" (Port đã được sử dụng)**
```bash
# Nếu gặp lỗi port 8000 đã được sử dụng:
# 1. Tìm và kill process đang sử dụng port 8000
netstat -ano | findstr :8000
taskkill /PID <PID_NUMBER> /F

# 2. Hoặc sử dụng port khác
$env:DATABASE_URL="sqlite+aiosqlite:///./oht50.db"
python -m uvicorn app.main:app --host 0.0.0.0 --port 8001

# 3. Hoặc kiểm tra process Python đang chạy
Get-Process | Where-Object {$_.ProcessName -like "*python*"}
Stop-Process -Name "python" -Force
```

### **4. Chạy Backend**
```bash
# Chạy backend với environment variable (khuyến nghị)
$env:DATABASE_URL="sqlite+aiosqlite:///./oht50.db"
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000

# Hoặc chạy với reload mode (nếu gặp lỗi module, thử không dùng --reload)
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