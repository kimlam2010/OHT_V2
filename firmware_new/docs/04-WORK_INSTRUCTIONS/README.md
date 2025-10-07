# 📚 04-WORK_INSTRUCTIONS (Hướng Dẫn Công Việc)

**Theo ISO 9001:2015 Section 7.5 - Work Instructions**

---

## 🎯 **MỤC ĐÍCH**

Thư mục này chứa **hướng dẫn công việc chi tiết** - step-by-step instructions cho các tác vụ cụ thể.

---

## 📄 **DANH SÁCH TÀI LIỆU**

| **Mã** | **Tên tài liệu** | **Mô tả** | **Version** |
|--------|------------------|-----------|-------------|
| **WI-001** | Installation_Guide.md | Hướng dẫn cài đặt đầy đủ (HW + SW) | 1.0.1 |
| **WI-002** | Usage_Guide.md | Hướng dẫn sử dụng firmware | 1.0.1 |
| **WI-003** | Development_Guide.md | Hướng dẫn phát triển | 1.0.1 |
| **WI-004** | Troubleshooting_Guide.md | Khắc phục 10+ sự cố | 1.0.1 |
| **WI-005** | LiDAR_Debug_Guide.md | Debug LiDAR HAL system | 1.2 |
| **WI-006** | Network_Deployment.md | Triển khai network management | 1.0 |

---

## 🔧 **WI-001: Installation Guide**

**Scope:** Cài đặt firmware lên Orange Pi 5B

**Nội dung:**
- ✅ Yêu cầu hệ thống (hardware + software)
- ✅ Chuẩn bị môi trường (OS, SSH)
- ✅ Cài đặt phụ thuộc (build tools, libraries)
- ✅ Build firmware (Debug/Release)
- ✅ Cấu hình hardware (UART1, GPIO, udev)
- ✅ Triển khai (systemd service)
- ✅ Xác minh cài đặt
- ✅ Troubleshooting (4 issues)

**Thời gian:** ~30-60 phút (lần đầu)

---

## 📖 **WI-002: Usage Guide**

**Scope:** Sử dụng firmware trong operation

**Nội dung:**
- ✅ Khởi động firmware (basic, service)
- ✅ State machine (8 states)
- ✅ Cấu hình (modules.yaml, safety_config)
- ✅ API usage (5 endpoints)
- ✅ Module management (auto-discovery)
- ✅ System monitoring (telemetry, LED)
- ✅ Emergency stop procedures
- ✅ Error handling

**Thời gian:** ~10-15 phút đọc hiểu

---

## 🛠️ **WI-003: Development Guide**

**Scope:** Development workflow cho contributors

**Nội dung:**
- ✅ IDE setup (VS Code + extensions)
- ✅ Architecture overview (5 layers)
- ✅ Development workflow
- ✅ Debugging (GDB, Valgrind)
- ✅ Testing strategy
- ✅ Performance profiling
- ✅ Best practices

**Thời gian:** ~20-30 phút setup

---

## 🐛 **WI-004: Troubleshooting Guide**

**Scope:** Khắc phục các sự cố phổ biến

**Nội dung:**
- ✅ 10 vấn đề phổ biến:
  1. Build failed
  2. /dev/ttyOHT485 not found
  3. Permission denied
  4. Port in use
  5. Module not discovered
  6. High CPU usage
  7. RS485 errors
  8. LiDAR not responding
  9. State machine stuck
  10. Memory leak
- ✅ Diagnostic tools
- ✅ Support channels

**Thời gian:** ~5-10 phút giải quyết issue

---

## ✅ **SỬ DỤNG**

Work Instructions phải được:
- ✅ Đọc trước khi thực hiện tác vụ
- ✅ Tuân thủ từng bước
- ✅ Update nếu có thay đổi process
- ✅ Review định kỳ (quarterly)

---

**Version:** 1.0  
**Last Updated:** 2025-10-07  
**Maintained By:** Operations Team

