# 🚀 OHT-50 Firmware

**Version:** 1.0.1 - Domain-Driven Architecture  
**Platform:** Orange Pi 5B (RK3588)  
**Standard:** ISO/IEC 12207 & ISO 9001:2015 Compliant  
**Architecture:** Domain-Driven Design (DDD)

---

## 📋 **TỔNG QUAN**

Firmware cho OHT-50 (Overhead Hoist and Transfer) Master Module - Hệ thống điều khiển robot công nghiệp với kiến trúc Domain-Driven Design và tuân thủ tiêu chuẩn ISO.

### **✨ Tính năng chính:**

| **Tính năng** | **Mô tả** | **Trạng thái** |
|---------------|-----------|----------------|
| 📡 **Multi-protocol Communication** | RS485, Ethernet, WiFi | ✅ Production |
| 🛡️ **Safety System** | E-Stop, Real-time monitoring | ✅ Production |
| 🔵 **LiDAR Integration** | 360° obstacle detection | ✅ Production |
| 🌐 **HTTP REST API** | RESTful endpoints @ port 8080 | ✅ Production |
| 📦 **Module Management** | Auto-discovery 4 slave modules | ✅ Production |
| ⚡ **Performance** | Boot < 20ms, Loop 8-15μs | ✅ Optimized |

---

## 🚀 **QUICK START**

### **1. Build Firmware**

```bash
# Tạo build directory
mkdir build && cd build

# Configure và build
cmake ..
make -j$(nproc)

# Run
./oht50_main
```

### **2. Chạy với Options**

```bash
# Dry-run mode (test không chạm hardware)
./oht50_main --dry-run

# Debug mode (verbose logging)
./oht50_main --debug

# Help
./oht50_main --help
```

---

## 📚 **TÀI LIỆU ISO CHUẨN**

Tất cả tài liệu được tổ chức theo **ISO 9001:2015 Document Control**:

### 📂 **Cấu trúc tài liệu:**

```
docs/
├── 01-QUALITY_POLICY/          # Chính sách chất lượng
│   ├── QP-001_Security_Policy.md
│   └── QP-002_License.md
│
├── 02-QUALITY_MANUAL/          # Sổ tay chất lượng
│   ├── QM-001_Project_Overview.md
│   └── QM-002_Code_Structure.md
│
├── 03-PROCEDURES/              # Thủ tục
│   ├── PR-001_Build_Procedure.md
│   ├── PR-002_Contributing_Procedure.md
│   └── PR-003_Code_Quality_Procedure.md
│
├── 04-WORK_INSTRUCTIONS/       # Hướng dẫn công việc
│   ├── WI-001_Installation_Guide.md
│   ├── WI-002_Usage_Guide.md
│   ├── WI-003_Development_Guide.md
│   └── WI-004_Troubleshooting_Guide.md
│
├── 05-FORMS_RECORDS/           # Biểu mẫu và hồ sơ
│   ├── REC-001_Change_Log.md
│   ├── REC-002_Migration_Log_v1.0.1.md
│   ├── REC-003_Domain_Driven_Migration_Summary.md
│   ├── REC-004_Cleanup_Summary.md
│   ├── REC-005_Final_Cleanup_Report.md
│   └── REC-006_Documentation_Update_Summary.md
│
└── 06-REFERENCE/               # Tài liệu tham khảo
    ├── REF-001_API_Reference.md
    ├── REF-002_Architecture_Quick_Reference.md
    ├── REF-003_Documentation_Index.md
    └── REF-004_Documentation_Navigation.md
```

### 🔗 **Truy cập nhanh:**

| **Loại tài liệu** | **Đường dẫn** | **Mục đích** |
|-------------------|---------------|-------------|
| 📘 **Bắt đầu** | `docs/04-WORK_INSTRUCTIONS/WI-001_Installation_Guide.md` | Cài đặt firmware |
| 📖 **Sử dụng** | `docs/04-WORK_INSTRUCTIONS/WI-002_Usage_Guide.md` | Hướng dẫn sử dụng |
| 🔧 **Phát triển** | `docs/04-WORK_INSTRUCTIONS/WI-003_Development_Guide.md` | Development guide |
| 🌐 **API** | `docs/06-REFERENCE/REF-001_API_Reference.md` | API documentation |
| 🐛 **Khắc phục** | `docs/04-WORK_INSTRUCTIONS/WI-004_Troubleshooting_Guide.md` | Troubleshooting |
| 📊 **Changelog** | `docs/05-FORMS_RECORDS/REC-001_Change_Log.md` | Version history |

---

## 🏗️ **KIẾN TRÚC**

### **Domain-Driven Design (5 Layers)**

```
┌─────────────────────────────────────────────┐
│    📱 API LAYER (HTTP REST @ port 8080)     │
└─────────────────┬───────────────────────────┘
                  ▼
┌─────────────────────────────────────────────┐
│  🔐 APPLICATION LAYER (Orchestration)       │
│     - Safety Orchestrator                   │
└─────────────────┬───────────────────────────┘
                  ▼
┌─────────────────────────────────────────────┐
│  🏭 DOMAIN LAYER (Business Logic)           │
│     - Module Management, Power, Motion      │
│     - Safety Module, Dock                   │
└─────────────────┬───────────────────────────┘
                  ▼
┌─────────────────────────────────────────────┐
│  🔌 INFRASTRUCTURE LAYER (Technical)        │
│     - Communication, Network, Telemetry     │
└─────────────────┬───────────────────────────┘
                  ▼
┌─────────────────────────────────────────────┐
│  🎛️ CORE LAYER (Business Core)             │
│     - State Management, Safety, Control     │
└─────────────────┬───────────────────────────┘
                  ▼
┌─────────────────────────────────────────────┐
│  🔩 HAL LAYER (Hardware Abstraction)        │
│     - RS485, GPIO, LED, E-Stop, LiDAR       │
└─────────────────────────────────────────────┘
```

Chi tiết: `docs/06-REFERENCE/REF-002_Architecture_Quick_Reference.md`

---

## 📊 **THÔNG SỐ KỸ THUẬT**

| **Metric** | **Value** | **Note** |
|------------|-----------|----------|
| 📏 **Code Size** | 58,265 lines | C11 Standard |
| 📄 **Files** | 70 files (C/H) | Domain-Driven |
| 🧪 **Tests** | 47 test files | 8 categories |
| 📚 **Documentation** | 22 files | ISO compliant |
| ⚡ **Boot Time** | < 20ms | Optimized |
| 🔄 **Loop Time** | 8-15μs average | Adaptive |
| 🛡️ **Safety Response** | < 100ms | E-Stop |

---

## 🎯 **PLATFORM**

### **Hardware**

| **Component** | **Specification** |
|---------------|-------------------|
| 🖥️ Board | Orange Pi 5B (RK3588) |
| 💾 RAM | 4GB+ khuyến nghị |
| 💽 Storage | 16GB+ SD Card |
| 📡 RS485 | `/dev/ttyOHT485` @ 115200 |
| 🔌 GPIO | GPIO1_D3, GPIO1_D2 |
| 💡 LED | 5x status LEDs |
| 🌐 Network | Ethernet + WiFi |

### **Software**

| **Component** | **Version** |
|---------------|-------------|
| 🐧 OS | Ubuntu 20.04+/Armbian |
| 🔧 Compiler | GCC 9.0+ (C11) |
| 🏗️ Build | CMake 3.16+ |
| 🔐 Security | OpenSSL 1.1.1+ |

---

## 📖 **HƯỚNG DẪN CHI TIẾT**

### **Cho người dùng mới:**

1. 📘 **Installation** → `docs/04-WORK_INSTRUCTIONS/WI-001_Installation_Guide.md`
2. 📖 **Usage** → `docs/04-WORK_INSTRUCTIONS/WI-002_Usage_Guide.md`
3. 🐛 **Troubleshooting** → `docs/04-WORK_INSTRUCTIONS/WI-004_Troubleshooting_Guide.md`

### **Cho developers:**

1. 🔧 **Development** → `docs/04-WORK_INSTRUCTIONS/WI-003_Development_Guide.md`
2. 🏗️ **Build** → `docs/03-PROCEDURES/PR-001_Build_Procedure.md`
3. 🤝 **Contributing** → `docs/03-PROCEDURES/PR-002_Contributing_Procedure.md`
4. 🌐 **API** → `docs/06-REFERENCE/REF-001_API_Reference.md`

### **Cho PM/QA:**

1. 📊 **Changelog** → `docs/05-FORMS_RECORDS/REC-001_Change_Log.md`
2. 🏗️ **Architecture** → `docs/06-REFERENCE/REF-002_Architecture_Quick_Reference.md`
3. 📝 **Code Structure** → `docs/02-QUALITY_MANUAL/QM-002_Code_Structure.md`

---

## ⚖️ **COMPLIANCE**

### **Standards**

| **Standard** | **Compliance** | **Status** |
|--------------|----------------|------------|
| 📋 **ISO/IEC 12207** | Software lifecycle | ✅ Compliant |
| 📋 **ISO 9001:2015** | Quality management | ✅ Compliant |
| 🔒 **MISRA C:2012** | C coding standard | ⚠️ Partial |
| 🛡️ **IEC 61508** | Functional safety | ⏳ Planned |

---

## 🔐 **SECURITY & LICENSE**

- **License:** MIT License
- **Security:** See `docs/01-QUALITY_POLICY/QP-001_Security_Policy.md`
- **Third-party:** Unity (MIT), OpenSSL (Apache 2.0)

---

## 📞 **CONTACT**

| **Purpose** | **Contact** |
|-------------|-------------|
| 🐛 **Bug Reports** | GitHub Issues |
| 💡 **Feature Requests** | GitHub Discussions |
| 🔐 **Security** | security@oht50.com |
| 📧 **Support** | support@oht50.com |
| 💬 **Slack** | #oht50-firmware |

---

## 🎉 **VERSION HISTORY**

**Current:** v1.0.1 (2025-10-07)  
**Previous:** v1.0.0 (2025-10-05)  
**Changelog:** `docs/05-FORMS_RECORDS/REC-001_Change_Log.md`

---

**🏆 Status:** ✅ **PRODUCTION READY**  
**📅 Last Updated:** 2025-10-07  
**👥 Maintained By:** OHT-50 Firmware Team

