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

Backend hỗ trợ **2 chế độ hoạt động**:

#### **🧪 MOCK MODE (Development/Testing)**
Sử dụng mock data, không cần firmware thật:
```bash
# Copy file environment mẫu
cp env.example .env

# Hoặc tạo file .env mới với cấu hình mock mode
echo "DATABASE_URL=sqlite+aiosqlite:///./oht50.db" > .env
echo "JWT_SECRET=your-secret-key-here" >> .env
echo "USE_MOCK_FIRMWARE=true" >> .env
echo "TESTING=true" >> .env
echo "ENVIRONMENT=development" >> .env
echo "API_REDUCED=false" >> .env
```

#### **🔌 PRODUCTION MODE (Real Firmware)**
Kết nối với firmware thật:
```bash
echo "DATABASE_URL=sqlite+aiosqlite:///./oht50.db" > .env
echo "JWT_SECRET=your-secret-key-here" >> .env
echo "USE_MOCK_FIRMWARE=false" >> .env
echo "TESTING=false" >> .env
echo "ENVIRONMENT=production" >> .env
echo "FIRMWARE_URL=http://192.168.1.100:8081" >> .env
echo "API_REDUCED=true" >> .env
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

# 2. Hoặc sử dụng port khác (KHÔNG KHUYẾN NGHỊ - backend phải chạy port 8000)
$env:DATABASE_URL="sqlite+aiosqlite:///./oht50.db"
python -m uvicorn app.main:app --host 0.0.0.0 --port 8001

# 3. Hoặc kiểm tra process Python đang chạy
Get-Process | Where-Object {$_.ProcessName -like "*python*"}
Stop-Process -Name "python" -Force
```

#### **Lỗi Mock Mode không hoạt động**
```bash
# Nếu mock mode không hoạt động, kiểm tra:
# 1. Environment variables đã được set đúng chưa
echo $env:USE_MOCK_FIRMWARE  # Should be "true"
echo $env:TESTING            # Should be "true"
echo $env:ENVIRONMENT        # Should be "development"

# 2. Kiểm tra logs khi khởi động
# Phải thấy: 🧪 MOCK MODE: Using MockNetworkService
# Nếu thấy: 🔌 REAL FIRMWARE MODE → Đang ở production mode

# 3. Set lại environment variables
$env:USE_MOCK_FIRMWARE="true"
$env:TESTING="true"
$env:ENVIRONMENT="development"
$env:API_REDUCED="false"

# 4. Restart backend server
```

#### **Lỗi "All connection attempts failed" trong Mock Mode**
```bash
# Nếu gặp lỗi connection failed trong mock mode:
# 1. Kiểm tra service có check environment variables không
# 2. Restart backend với đầy đủ environment variables
cd backend
$env:USE_MOCK_FIRMWARE="true"
$env:TESTING="true"
$env:ENVIRONMENT="development"
$env:DATABASE_URL="sqlite+aiosqlite:///./oht50.db"
python -m uvicorn app.main:app --host 127.0.0.1 --port 8000

# 3. Kiểm tra logs phải thấy:
# - 🧪 MOCK MODE: Using mock telemetry data
# - 🧪 Mock: Getting RS485 modules list
# - 🧪 Mock: Found 7 RS485 modules
```

#### **Lỗi WiFi Connect conflict (403 Forbidden)**
```bash
# Nếu WiFi Connect API trả về 403 Forbidden:
# 1. Kiểm tra đang dùng đúng endpoint chưa
# OLD: POST /api/v1/network/wifi/connect (requires auth)
# NEW: POST /api/v1/wifi/connect (no auth required in mock mode)

# 2. Test với endpoint mới
$body = @{ssid="TestNetwork"; password="testpass123"} | ConvertTo-Json
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/wifi/connect" -Method POST -Body $body -ContentType "application/json"
```

#### **Lỗi Redis config**
```bash
# Nếu gặp lỗi "'Settings' object has no attribute 'redis_host'":
# 1. Kiểm tra file config.py đã có redis_host, redis_port, redis_db chưa
# 2. Nếu chưa có, thêm vào app/config.py:
# redis_host: str = "localhost"
# redis_port: int = 6379
# redis_db: int = 0

# 3. Restart backend server
```

### **4. Chạy Backend**

#### **🧪 Chạy ở Mock Mode (Development)**
```powershell
# Windows PowerShell
cd backend
$env:USE_MOCK_FIRMWARE="true"
$env:TESTING="true"
$env:ENVIRONMENT="development"
$env:DATABASE_URL="sqlite+aiosqlite:///./oht50.db"
$env:API_REDUCED="false"
python -m uvicorn app.main:app --host 127.0.0.1 --port 8000 --log-level info

# Hoặc với reload mode (auto-restart khi code thay đổi)
python -m uvicorn app.main:app --host 127.0.0.1 --port 8000 --reload
```

```bash
# Linux/Mac
cd backend
export USE_MOCK_FIRMWARE=true
export TESTING=true
export ENVIRONMENT=development
export DATABASE_URL="sqlite+aiosqlite:///./oht50.db"
export API_REDUCED=false
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000 --log-level info
```

#### **🔌 Chạy ở Production Mode (Real Firmware)**
```powershell
# Windows PowerShell
cd backend
$env:USE_MOCK_FIRMWARE="false"
$env:TESTING="false"
$env:ENVIRONMENT="production"
$env:DATABASE_URL="sqlite+aiosqlite:///./oht50.db"
$env:FIRMWARE_URL="http://192.168.1.100:8081"
$env:API_REDUCED="true"
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000 --log-level info
```

```bash
# Linux/Mac
cd backend
export USE_MOCK_FIRMWARE=false
export TESTING=false
export ENVIRONMENT=production
export DATABASE_URL="sqlite+aiosqlite:///./oht50.db"
export FIRMWARE_URL="http://192.168.1.100:8081"
export API_REDUCED=true
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000 --log-level info
```

#### **📊 Kiểm tra Backend đang chạy**
```powershell
# Kiểm tra process
Get-Process | Where-Object {$_.ProcessName -like "*python*"}

# Kiểm tra port
netstat -ano | findstr :8000

# Kill process nếu cần
Stop-Process -Name "python" -Force
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
- `GET /api/v1/robot/status` - Trạng thái robot ✅ Mock tested
- `POST /api/v1/robot/control` - Điều khiển robot
- `POST /api/v1/robot/emergency-stop` - Dừng khẩn cấp
- `GET /api/v1/robot/position` - Vị trí robot

### **📊 Telemetry**
- `GET /api/v1/telemetry/current` - Dữ liệu hiện tại ✅ Mock tested
- `GET /api/v1/telemetry/history` - Lịch sử dữ liệu
- `POST /api/v1/telemetry/collection/start` - Bắt đầu thu thập

### **🔌 RS485 Modules**
- `GET /api/v1/rs485/modules` - Danh sách modules ✅ Mock tested (7 modules)
- `GET /api/v1/rs485/health` - Sức khỏe bus RS485
- `GET /api/v1/rs485/modules/{address}/telemetry` - Telemetry module

### **🗺️ Mapping & Localization**
- `POST /api/v1/map/start-mapping` - Bắt đầu mapping
- `GET /api/v1/map/current` - Bản đồ hiện tại
- `GET /api/v1/localization/position` - Vị trí hiện tại

### **🌐 Network Management**
- `GET /api/v1/network/status` - Trạng thái mạng ✅ Mock tested
- `POST /api/v1/network/ap/start` - Khởi động WiFi AP
- `GET /api/v1/network/wifi/scan` - Quét WiFi ✅ Mock tested (3 networks)
- `POST /api/v1/wifi/connect` - Kết nối WiFi ✅ Mock tested (new prefix)
- `POST /api/v1/wifi/disconnect` - Ngắt kết nối WiFi

### **🔒 Safety & Security**
- `GET /api/v1/safety/status` - Trạng thái an toàn
- `POST /api/v1/safety/emergency-stop` - Dừng khẩn cấp
- `GET /api/v1/monitoring/health` - Sức khỏe hệ thống

### **🏥 Health Checks**
- `GET /health` - Health check cơ bản ✅ Mock tested
- `GET /health/detailed` - Health check chi tiết
- `GET /health/fast` - Fast health check

### **🧪 Mock APIs Tested Successfully**

Các APIs sau đã được test thành công với mock data:

1. ✅ **WiFi Scan:** 3 networks (VanPhong5G, OHT-50-Hotspot, TestNetwork)
2. ✅ **Network Status:** Connected status, AP config
3. ✅ **Robot Status:** OHT-50-001, idle mode, battery 87%
4. ✅ **Telemetry Current:** Motor speed 1500, temp 45.5°C
5. ✅ **RS485 Modules:** 7 modules với real-time telemetry data
6. ✅ **WiFi Connect:** Mock connection success
7. ✅ **Health Check:** System healthy status

---

## 🧪 **TESTING**

### **Test Mock APIs**
```powershell
# Test WiFi Scan
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/network/wifi/scan" -Method GET

# Test Robot Status
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/robot/status" -Method GET

# Test Telemetry Current
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/telemetry/current" -Method GET

# Test RS485 Modules
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/rs485/modules" -Method GET

# Test WiFi Connect (new prefix)
$body = @{ssid="TestNetwork"; password="testpass123"} | ConvertTo-Json
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/wifi/connect" -Method POST -Body $body -ContentType "application/json"
```

### **Chạy Unit Tests**
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

#### **🧪 Mock Mode Configuration**
```bash
# Database
DATABASE_URL=sqlite+aiosqlite:///./oht50.db

# Security
JWT_SECRET=your-secret-key
JWT_EXPIRY_MINUTES=30

# Mock Mode Settings (Development/Testing)
USE_MOCK_FIRMWARE=true
TESTING=true
ENVIRONMENT=development
API_REDUCED=false

# Redis (optional for mock mode)
redis_host=localhost
redis_port=6379
redis_db=0

# Performance
MAX_CONNECTIONS=100
REQUEST_TIMEOUT=30
```

#### **🔌 Production Mode Configuration**
```bash
# Database
DATABASE_URL=sqlite+aiosqlite:///./oht50.db

# Security
JWT_SECRET=your-secret-key-production
JWT_EXPIRY_MINUTES=30

# Production Mode Settings
USE_MOCK_FIRMWARE=false
TESTING=false
ENVIRONMENT=production
API_REDUCED=true

# Firmware Integration (REQUIRED for production)
FIRMWARE_URL=http://192.168.1.100:8081

# Redis (recommended for production)
redis_host=localhost
redis_port=6379
redis_db=0

# Performance
MAX_CONNECTIONS=100
REQUEST_TIMEOUT=30
```

### **🔄 Tự động chuyển đổi giữa Mock và Production**

Backend sẽ **tự động** chọn service phù hợp dựa trên environment variables:

| Service | Mock Mode | Production Mode |
|---------|-----------|-----------------|
| **Network** | `MockNetworkService` | `NetworkIntegrationService` → Firmware HTTP API |
| **RS485** | `MockRS485Service` (7 modules) | `RS485Service` → Firmware → RS485 Hardware |
| **Telemetry** | Mock data (motor speed, temp, etc.) | `TelemetryService` → Firmware HTTP API |
| **Robot Control** | Mock responses | `RobotControlService` → Firmware HTTP API |

**Logs xác nhận:**
- Mock mode: `🧪 MOCK MODE: Using MockNetworkService`
- Production: `🔌 REAL FIRMWARE MODE: Using NetworkIntegrationService`

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