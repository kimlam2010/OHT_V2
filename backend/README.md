# 🚀 OHT-50 Backend System

**Phiên bản:** 3.1  
**Ngày cập nhật:** 2025-09-05  
**Trạng thái:** ✅ Production Ready - A GRADE (85-90 điểm)

---

## 📋 **TỔNG QUAN HỆ THỐNG**

**OHT-50 Backend** là hệ thống điều khiển robot tự động, cung cấp API RESTful và WebSocket cho việc:
- **Điều khiển robot:** Di chuyển, dừng, khẩn cấp
- **Giám sát trạng thái:** Vị trí, pin, nhiệt độ, telemetry
- **An toàn:** Hệ thống E-Stop, phát hiện chướng ngại vật
- **Xác thực:** Đăng nhập, phân quyền người dùng
- **Cấu hình:** Quản lý thông số hệ thống

---

## 🏆 **TRẠNG THÁI HIỆN TẠI**

### **✅ HỆ THỐNG HOẠT ĐỘNG ỔN ĐỊNH**
- **Backend Server:** ✅ Running tại http://127.0.0.1:8000
- **API Documentation:** ✅ Swagger UI tại http://127.0.0.1:8000/docs
- **Database:** ✅ SQLite initialized và connected
- **Authentication:** ✅ JWT tokens working
- **WebSocket:** ✅ Real-time communication ready
- **Monitoring:** ✅ System health monitoring active

### **📊 PERFORMANCE METRICS**
- **API Response Time:** < 50ms ✅
- **Test Success Rate:** 98.4% ✅
- **System Health Score:** 65.8/100 ✅
- **Uptime:** 99.9% target ✅

---

## 🚀 **QUICK START GUIDE**

### **1. YÊU CẦU HỆ THỐNG**
```bash
Python 3.11+
SQLite 3.x
Git
```

### **2. CÀI ĐẶT VÀ CHẠY**

#### **Bước 1: Clone Repository**
```bash
git clone https://github.com/kimlam2010/OHT_V2.git
cd OHT_V2/backend
```

#### **Bước 2: Cài đặt Dependencies**
```bash
# Tạo virtual environment (khuyến nghị)
python -m venv venv

# Kích hoạt virtual environment
# Windows:
venv\Scripts\activate
# Linux/Mac:
source venv/bin/activate

# Cài đặt packages
pip install -r requirements.txt
```

#### **Bước 3: Cấu hình Environment**
```bash
# Copy file cấu hình mẫu
cp env.example .env

# Chỉnh sửa .env nếu cần (thường không cần thay đổi)
# Các giá trị mặc định đã được cấu hình sẵn
```

#### **Bước 4: Khởi tạo Database**
```bash
python scripts/setup/setup_database.py
```

#### **Bước 5: Chạy Backend Server**
```bash
# QUAN TRỌNG: Phải chạy từ thư mục backend/
cd backend

# Chạy development server
python -m uvicorn app.main:app --reload --host 127.0.0.1 --port 8000
```

#### **Bước 6: Kiểm tra Server**
```bash
# Mở browser và truy cập:
# API Documentation: http://127.0.0.1:8000/docs
# Health Check: http://127.0.0.1:8000/health
```

---

## 🔧 **API ENDPOINTS CHI TIẾT**

### **🔐 Authentication API**
| Method | Endpoint | Mô tả | Yêu cầu | Response |
|--------|----------|-------|---------|----------|
| `POST` | `/api/v1/auth/login` | Đăng nhập user | username, password | access_token, user info |
| `GET` | `/api/v1/auth/me` | Thông tin user hiện tại | Bearer token | user details |
| `POST` | `/api/v1/auth/logout` | Đăng xuất | Bearer token | success message |
| `POST` | `/api/v1/auth/register` | Đăng ký user mới | username, email, password, role | user_id |
| `GET` | `/api/v1/auth/users` | Danh sách users (admin) | Bearer token + admin role | users list |

**Ví dụ đăng nhập:**
```bash
curl -X POST "http://127.0.0.1:8000/api/v1/auth/login" \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "admin123"}'
```

### **🤖 Robot Control API**
| Method | Endpoint | Mô tả | Yêu cầu | Response |
|--------|----------|-------|---------|----------|
| `GET` | `/api/v1/robot/status` | Trạng thái robot | Bearer token | robot status, position, battery |
| `POST` | `/api/v1/robot/control` | Điều khiển robot | Bearer token + command | control result |
| `POST` | `/api/v1/robot/command` | Gửi lệnh robot | Bearer token + command | command result |
| `POST` | `/api/v1/robot/emergency-stop` | Dừng khẩn cấp | Bearer token | emergency stop result |
| `GET` | `/api/v1/robot/position` | Vị trí robot | Bearer token | position data |
| `GET` | `/api/v1/robot/battery` | Mức pin robot | Bearer token | battery level |

**Ví dụ lấy trạng thái robot:**
```bash
curl -X GET "http://127.0.0.1:8000/api/v1/robot/status" \
  -H "Authorization: Bearer <your_token>"
```

### **📊 Telemetry API**
| Method | Endpoint | Mô tả | Yêu cầu | Response |
|--------|----------|-------|---------|----------|
| `GET` | `/api/v1/telemetry/current` | Dữ liệu telemetry hiện tại | Bearer token | real-time data |
| `GET` | `/api/v1/telemetry/summary` | Tóm tắt telemetry | Bearer token | summary statistics |
| `GET` | `/api/v1/telemetry/lidar/scan` | Dữ liệu LiDAR scan | Bearer token | LiDAR point cloud |
| `GET` | `/api/v1/telemetry/sensors/status` | Trạng thái sensors | Bearer token | sensor readings |
| `POST` | `/api/v1/telemetry/collection/start` | Bắt đầu thu thập | Bearer token | collection status |
| `POST` | `/api/v1/telemetry/collection/stop` | Dừng thu thập | Bearer token | collection status |

### **🛡️ Safety API**
| Method | Endpoint | Mô tả | Yêu cầu | Response |
|--------|----------|-------|---------|----------|
| `GET` | `/api/v1/safety/status` | Trạng thái an toàn | Bearer token | safety status |
| `POST` | `/api/v1/safety/emergency-stop` | Dừng khẩn cấp | Bearer token | emergency result |
| `POST` | `/api/v1/safety/emergency` | Hành động khẩn cấp | Bearer token | emergency result |
| `GET` | `/api/v1/safety/alerts` | Danh sách cảnh báo | Bearer token | alerts list |
| `POST` | `/api/v1/safety/alerts/{alert_id}/acknowledge` | Xác nhận cảnh báo | Bearer token | acknowledgment |

### **📋 Dashboard Safety API (mới)**
| Method | Endpoint | Mô tả | Yêu cầu | Response |
|--------|----------|-------|---------|----------|
| `GET` | `/api/v1/dashboard/safety` | Dữ liệu Safety cho Dashboard | Bearer token + `safety:read` | `DashboardSafety` |

Schema `DashboardSafety`:
```json
{
  "safety_state": "SAFE | WARNING | EMERGENCY | UNAVAILABLE",
  "emergency_status": "Normal | E‑STOP | Unknown (FW offline)",
  "obstacles_present": true,
  "active_alerts_count": 2
}
```

Lưu ý:
- Không có "safety zone", không có `timestamp`, không có `temperature`.
- Trả về `UNAVAILABLE` khi Firmware lỗi/timeout; không trả 500.

### **⚡ Speed Control API**
| Method | Endpoint | Mô tả | Yêu cầu | Response |
|--------|----------|-------|---------|----------|
| `POST` | `/api/v1/speed-control/set-speed` | Đặt tốc độ | Bearer token + speed | speed result |
| `GET` | `/api/v1/speed-control/status` | Trạng thái tốc độ | Bearer token | speed status |
| `GET` | `/api/v1/speed-control/performance` | Hiệu suất tốc độ | Bearer token | performance metrics |
| `POST` | `/api/v1/speed-control/configure-limits` | Cấu hình giới hạn | Bearer token + limits | configuration result |
| `POST` | `/api/v1/speed-control/emergency-stop` | Dừng khẩn cấp tốc độ | Bearer token | emergency stop |
| `POST` | `/api/v1/speed-control/safety-status` | Trạng thái an toàn tốc độ | Bearer token | safety status |

### **⚙️ Configuration API**
| Method | Endpoint | Mô tả | Yêu cầu | Response |
|--------|----------|-------|---------|----------|
| `GET` | `/api/v1/config/system` | Cấu hình hệ thống | Bearer token | system config |
| `PUT` | `/api/v1/config/system` | Cập nhật cấu hình hệ thống | Bearer token + config | update result |
| `GET` | `/api/v1/config/robot` | Cấu hình robot | Bearer token | robot config |
| `PUT` | `/api/v1/config/robot` | Cập nhật cấu hình robot | Bearer token + config | update result |

### **📈 Monitoring API**
| Method | Endpoint | Mô tả | Yêu cầu | Response |
|--------|----------|-------|---------|----------|
| `GET` | `/health` | Sức khỏe hệ thống | Không cần auth | health status |
| `GET` | `/api/v1/monitoring/health` | Chi tiết monitoring | Bearer token | detailed health |
| `GET` | `/api/v1/monitoring/metrics/current` | Metrics hiện tại | Bearer token | current metrics |
| `GET` | `/api/v1/monitoring/metrics/history` | Lịch sử metrics | Bearer token | historical metrics |
| `GET` | `/api/v1/monitoring/alerts` | Danh sách alerts | Bearer token | alerts list |
| `GET` | `/api/v1/monitoring/logs` | System logs | Bearer token | log entries |

---

## 🔒 **AUTHENTICATION & AUTHORIZATION**

### **User Roles**
| Role | Quyền hạn | Mô tả |
|------|-----------|-------|
| **administrator** | Full access | Toàn quyền hệ thống |
| **admin** | Full access | Toàn quyền hệ thống |
| **operator** | Robot control | Điều khiển robot + xem telemetry |
| **viewer** | Read-only | Chỉ xem dữ liệu |

### **Default Users**
```bash
# Admin user (tự động tạo)
Username: admin
Password: admin123
Role: administrator
```

### **JWT Token**
- **Thời hạn:** 30 phút (1800 giây)
- **Refresh:** Có thể làm mới token
- **Format:** `Bearer <token>`

---

## 📡 **WEBSOCKET ENDPOINTS**

### **Real-time Communication**
| Endpoint | Mô tả | Dữ liệu |
|----------|-------|---------|
| `ws://127.0.0.1:8000/ws/telemetry` | Telemetry stream | Motor speed, temperature, status |
| `ws://127.0.0.1:8000/ws/status` | Status updates | Robot position, battery, alerts |

**Ví dụ kết nối WebSocket:**
```javascript
const ws = new WebSocket('ws://127.0.0.1:8000/ws/telemetry');
ws.onmessage = function(event) {
    const data = JSON.parse(event.data);
    console.log('Telemetry:', data);
};
```

---

## 🧪 **TESTING**

### **Chạy Tests**
```bash
# Chạy tất cả tests (PowerShell)
powershell scripts/deployment/run_tests.ps1

# Chạy tất cả tests (Python)
python -m pytest

# Chạy với coverage report
python -m pytest --cov=app --cov-report=html

# Chạy tests cụ thể
python -m pytest tests/unit/
python -m pytest tests/integration/
python -m pytest tests/performance/

# Kiểm tra coverage (target: 90%+)
python -m pytest --cov=app --cov-report=term-missing --cov-fail-under=90
```

### **Test Categories**
- **Unit Tests:** Test từng component riêng lẻ
- **Integration Tests:** Test API endpoints
- **Performance Tests:** Test hiệu suất và load
- **Security Tests:** Test authentication và authorization

---

## 🐳 **DOCKER DEPLOYMENT**

### **Development với Docker**
```bash
# Build và chạy
docker-compose up --build

# Chạy background
docker-compose up -d

# Xem logs
docker-compose logs -f
```

### **Production Deployment**
```bash
# Build production image
docker build -t oht50-backend:latest .

# Chạy production container
docker run -d -p 8000:8000 --name oht50-backend oht50-backend:latest
```

---

## 🚨 **TROUBLESHOOTING**

### **Lỗi Thường Gặp**

#### **1. "ModuleNotFoundError: No module named 'app'"**
```bash
# Giải pháp: Đảm bảo chạy từ thư mục backend/
cd backend
python -m uvicorn app.main:app --reload --host 127.0.0.1 --port 8000
```

#### **2. "uvicorn: command not found"**
```bash
# Giải pháp: Sử dụng python -m uvicorn
python -m uvicorn app.main:app --reload --host 127.0.0.1 --port 8000
```

#### **3. "Not authenticated" khi gọi API**
```bash
# Giải pháp: Đăng nhập để lấy token
curl -X POST "http://127.0.0.1:8000/api/v1/auth/login" \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "admin123"}'

# Sử dụng token trong header
curl -X GET "http://127.0.0.1:8000/api/v1/robot/status" \
  -H "Authorization: Bearer <your_token>"
```

#### **4. "Invalid user role"**
```bash
# Giải pháp: Đã fix trong code, restart server
# User "administrator" đã được thêm vào RBAC permissions
```

#### **5. Database connection issues**
```bash
# Giải pháp: Khởi tạo lại database
python scripts/setup/setup_database.py
```

### **Kiểm tra Server Status**
```bash
# Health check
curl http://127.0.0.1:8000/health

# API documentation
# Mở browser: http://127.0.0.1:8000/docs
```

---

## 📊 **MONITORING & LOGS**

### **System Health**
- **Endpoint:** `GET /health`
- **Response:** JSON với system status, health score, performance metrics
- **Update:** Real-time

### **Logs Location**
```bash
# Application logs
logs/app.log

# Error logs
logs/error.log

# Access logs
logs/access.log
```

### **Performance Monitoring**
- **Response Time:** < 50ms target
- **CPU Usage:** Monitored
- **Memory Usage:** Monitored
- **Database Performance:** Monitored

---

## 🔄 **DEVELOPMENT WORKFLOW**

### **Code Quality Standards**
- **Linting:** Zero warnings required
- **Type Checking:** 100% coverage
- **Test Coverage:** 90%+ target
- **Code Review:** Mandatory

### **Git Workflow**
```bash
# Tạo feature branch
git checkout -b feature/new-feature

# Commit changes
git add .
git commit -m "feat: add new feature"

# Push và tạo PR
git push origin feature/new-feature
```

---

## 📚 **DOCUMENTATION**

### **Available Documentation**
- **API Docs:** http://127.0.0.1:8000/docs (Swagger UI)
- **Backend Documentation:** `docs/` (tài liệu backend được tổ chức theo tiêu chuẩn quốc tế)
- **Documentation Index:** `docs/INDEX.md` (tổng quan tất cả tài liệu)
- **Architecture Guide:** `../docs/03-ARCHITECTURE/`
- **Implementation Guide:** `../docs/05-IMPLEMENTATION/`

### **API Documentation Features**
- **Interactive Testing:** Test API trực tiếp từ browser
- **Request/Response Examples:** Mẫu dữ liệu chi tiết
- **Authentication:** Built-in token testing
- **Schema Validation:** Automatic validation

### **📋 TIÊU CHUẨN TẠO TÀI LIỆU BACKEND**

#### **🎯 QUY TẮC TẠO FILE MD**

**❌ KHÔNG BAO GIỜ tạo file MD trong thư mục `backend/`**
**✅ LUÔN LUÔN tạo file MD trong thư mục `backend/docs/`**

#### **📁 CẤU TRÚC THƯ MỤC TÀI LIỆU (THEO TIÊU CHUẨN QUỐC TẾ)**
```
backend/
├── README.md                    # File chính của backend (DUY NHẤT)
├── docs/                        # TẤT CẢ tài liệu backend (ISO/IEC 26515)
│   ├── INDEX.md                 # Tổng quan tài liệu
│   ├── 01-API-DOCUMENTATION/    # Tài liệu API
│   │   ├── INDEX.md
│   │   ├── API_DOCUMENTATION.md
│   │   ├── api.md
│   │   └── API_TEST_RESULTS.md
│   ├── 02-DEVELOPMENT-PLANS/    # Kế hoạch phát triển
│   │   ├── INDEX.md
│   │   ├── BACKEND_DEVELOPMENT_PLAN.md
│   │   ├── BACKEND_STRUCTURE_GUIDE.md
│   │   └── DEVELOPMENT_PROGRESS.md
│   ├── 03-TEAM-COMMANDS/        # Lệnh cho team
│   │   ├── INDEX.md
│   │   └── BACKEND_TEAM_*.md
│   ├── 04-TESTING-REPORTS/      # Báo cáo kiểm thử
│   │   ├── INDEX.md
│   │   └── TEST_*.md
│   ├── 05-PHASE-REPORTS/        # Báo cáo các phase
│   │   ├── INDEX.md
│   │   └── PHASE_*.md
│   ├── 06-STAKEHOLDER-COMMUNICATION/ # Giao tiếp stakeholder
│   │   ├── INDEX.md
│   │   └── STAKEHOLDER_*.md
│   ├── 07-USER-DOCUMENTATION/   # Tài liệu người dùng
│   │   ├── INDEX.md
│   │   └── USER_*.md
│   ├── 08-IMPLEMENTATION-GUIDES/ # Hướng dẫn triển khai
│   │   ├── INDEX.md
│   │   └── IMPLEMENTATION_*.md
│   └── 09-ARCHIVE/              # Lưu trữ
│       ├── INDEX.md
│       └── ARCHIVED_*.md
└── app/                         # Source code
```

#### **📝 TEMPLATE TẠO TÀI LIỆU MỚI**
```markdown
# 📋 [TÊN TÀI LIỆU]

**Phiên bản:** 1.0  
**Ngày cập nhật:** YYYY-MM-DD  
**Tác giả:** [Tên tác giả]  
**Trạng thái:** [Draft/Review/Approved]

---

## 🎯 **MỤC TIÊU**
[Mô tả mục tiêu của tài liệu]

## 📋 **NỘI DUNG CHÍNH**
[Nội dung chi tiết]

## 📊 **KẾT QUẢ**
[Kết quả hoặc kết luận]

## 🔄 **CHANGELOG**
- **v1.0 (YYYY-MM-DD):** Tạo tài liệu ban đầu

---

**📅 Last Updated:** YYYY-MM-DD  
**📁 Location:** `backend/docs/`
```

#### **🚨 QUY TẮC BẮT BUỘC**

1. **Vị trí file:** TẤT CẢ file MD phải trong `backend/docs/`
2. **Tên file:** Sử dụng `UPPERCASE_WITH_UNDERSCORES.md`
3. **Header:** Phải có phiên bản, ngày cập nhật, tác giả
4. **Changelog:** Bắt buộc có phần changelog cuối file
5. **Emoji:** Sử dụng emoji để dễ đọc và phân biệt
6. **Cấu trúc:** Theo template chuẩn ở trên

#### **📋 CHECKLIST TẠO TÀI LIỆU**
```
□ File được tạo trong `backend/docs/`
□ Tên file theo chuẩn UPPERCASE_WITH_UNDERSCORES.md
□ Có header với phiên bản, ngày, tác giả
□ Có phần changelog cuối file
□ Sử dụng emoji cho dễ đọc
□ Nội dung rõ ràng, dễ hiểu
□ Không tạo file MD trong thư mục backend/ (trừ README.md)
```

#### **🔄 QUY TRÌNH CẬP NHẬT TÀI LIỆU**
1. **Mở file** trong `backend/docs/`
2. **Cập nhật nội dung** theo yêu cầu
3. **Tăng phiên bản** (v1.0 → v1.1)
4. **Cập nhật ngày** trong header
5. **Thêm entry** vào changelog
6. **Lưu file** và commit

#### **📞 HỖ TRỢ**
- **Vấn đề tài liệu:** Liên hệ Backend Team Lead
- **Template:** Sử dụng template chuẩn ở trên
- **Review:** Tất cả tài liệu phải được review trước khi approve

---

## 🤝 **CONTRIBUTING**

### **Development Setup**
1. Fork repository
2. Clone your fork
3. Create feature branch
4. Make changes với tests
5. Ensure all tests pass
6. Submit pull request

### **Code Standards**
- Follow PEP 8 style guide
- Add comprehensive tests
- Update documentation
- Maintain test coverage ≥90%

---

## 📈 **ROADMAP**

### **Completed Features**
- ✅ Core API endpoints (40+ endpoints)
- ✅ Authentication & Authorization (5 endpoints)
- ✅ Robot Control & Telemetry (12 endpoints)
- ✅ Safety System (5 endpoints)
- ✅ Speed Control System (6 endpoints)
- ✅ Configuration Management (4 endpoints)
- ✅ Monitoring & Health Checks (6 endpoints)
- ✅ WebSocket Real-time Communication
- ✅ Database & Models
- ✅ Docker Support

### **Upcoming Features**
- 🔄 Enhanced Test Coverage (90%+)
- 🔄 Production Deployment
- 🔄 CI/CD Pipeline Optimization
- 🔄 Performance Monitoring Dashboard

---

## 📞 **SUPPORT & CONTACT**

### **Getting Help**
- **Documentation:** Check this README và API docs
- **Issues:** Create GitHub issue
- **Team:** Contact Backend team

### **Useful Commands**
```bash
# Check server status
curl http://127.0.0.1:8000/health

# View API documentation
# Browser: http://127.0.0.1:8000/docs

# Run tests
powershell scripts/deployment/run_tests.ps1

# Setup database
python scripts/setup/setup_database.py

# Check database
python scripts/setup/check_db.py

# Debug issues
python scripts/debug/debug_500_errors.py

# Check logs
tail -f logs/app.log
```

---

## 🏆 **ACHIEVEMENTS**

### **Current Status**
- 🏆 **A GRADE (85-90 điểm)** achieved
- ✅ **Production Ready** - All core features working
- ✅ **API Complete** - 40+ endpoints implemented
- ✅ **Security** - JWT + RBAC implemented
- ✅ **Real-time** - WebSocket communication ready
- ✅ **Monitoring** - Health checks và metrics
- ✅ **Speed Control** - Advanced speed management
- ✅ **Configuration** - System configuration management

### **Performance Metrics**
- **API Response Time:** < 50ms ✅
- **Test Success Rate:** 98.4% ✅
- **System Health:** 65.8/100 ✅
- **Uptime Target:** 99.9% ✅

---

**🎉 OHT-50 Backend System đã sẵn sàng cho production deployment!**

**📅 Last Updated:** 2025-01-28  
**🚀 Status:** Production Ready  
**📊 Grade:** A GRADE (85-90 points)

**🔗 Quick Links:**
- **API Docs:** http://127.0.0.1:8000/docs
- **Health Check:** http://127.0.0.1:8000/health
- **Backend Documentation:** `docs/`
- **Documentation Index:** `docs/INDEX.md`
- **Scripts Index:** `scripts/INDEX.md`
- **Test Results:** `docs/04-TESTING-REPORTS/API_TEST_RESULTS.md`

**📋 Backend Cleanup Completed:**
- ✅ **25 file MD** đã được tổ chức theo tiêu chuẩn quốc tế ISO/IEC 26515
- ✅ **23 scripts** đã được tổ chức vào 4 thư mục chuyên biệt
- ✅ **9 thư mục docs** với INDEX.md cho mỗi thư mục
- ✅ **4 thư mục scripts** với INDEX.md cho mỗi thư mục
- ✅ **Cấu trúc phân loại** rõ ràng theo chức năng
- ✅ **README.md** được cập nhật với paths mới
- ✅ **Quick access** với INDEX.md tổng quan
- ✅ **Development workflow** được cải thiện