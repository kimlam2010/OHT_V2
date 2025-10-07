# 📚 API Documentation Index

**Phiên bản:** 2.0  
**Ngày cập nhật:** 2025-01-28  
**Tác giả:** Backend Team  
**Trạng thái:** Active  

---

## 🎯 **MỤC TIÊU**
Tài liệu API duy nhất cho OHT-50 Backend System với **100+ endpoints** được test và verify.

## 📋 **TÀI LIỆU CHÍNH**

### **📖 Complete API Documentation**
- **[COMPLETE_API_DOCUMENTATION.md](./COMPLETE_API_DOCUMENTATION.md)** - **Tài liệu API duy nhất và đầy đủ**
  - ✅ **100+ API endpoints** được test thực tế
  - ✅ **8 nhóm API chính** với examples chi tiết
  - ✅ **3 WebSocket endpoints** với message formats
  - ✅ **Complete testing examples** (Bash, Python, cURL)
  - ✅ **Error handling** và rate limiting
  - ✅ **Performance targets** và metrics

## 🔗 **LIÊN KẾT NHANH**

### **API Endpoints**
- **Authentication:** `/api/v1/auth/*`
- **Robot Control:** `/api/v1/robot/*`
- **Telemetry:** `/api/v1/telemetry/*`
- **Safety:** `/api/v1/safety/*`
- **Monitoring:** `/api/v1/monitoring/*`

### **WebSocket**
- **Telemetry Stream:** `ws://127.0.0.1:8000/ws/telemetry`
- **Status Updates:** `ws://127.0.0.1:8000/ws/status`

## 📊 **THỐNG KÊ**
- **Total Endpoints:** 100+
- **API Categories:** 8
- **WebSocket Endpoints:** 3
- **Test Coverage:** 100% (verified)
- **Response Time:** < 50ms

## 🔄 **CHANGELOG**
- **v2.0 (2025-01-28):** Tạo tài liệu API duy nhất với 100+ endpoints
- **v1.0 (2025-01-28):** Tạo index file ban đầu

---

**📅 Last Updated:** 2025-01-28  
**📁 Location:** `backend/docs/01-API-DOCUMENTATION/`  
**🔗 Related:** [Backend README](../../README.md)

## Firmware Health & Metrics

### Endpoint: GET /api/v1/firmware/health
- Mô tả: Trả về tình trạng kết nối firmware, trạng thái circuit breaker, last heartbeat
- Yêu cầu: Auth
- Performance: < 50ms

### Endpoint: GET /api/v1/firmware/metrics
- Mô tả: Xuất metrics ở định dạng Prometheus cho firmware integration
- Yêu cầu: Auth (hoặc giới hạn IP nội bộ)
- Performance: < 50ms
