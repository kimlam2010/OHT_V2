# 📚 API Documentation Index

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Tác giả:** Backend Team  
**Trạng thái:** Active  

---

## 🎯 **MỤC TIÊU**
Tài liệu API cho OHT-50 Backend System, bao gồm tài liệu API, kết quả test và hướng dẫn sử dụng.

## 📋 **DANH SÁCH TÀI LIỆU**

### **📖 API Documentation**
- **[API_DOCUMENTATION.md](./API_DOCUMENTATION.md)** - Tài liệu API chi tiết
- **[api.md](./api.md)** - Tài liệu API ngắn gọn
- **[API_TEST_RESULTS.md](./API_TEST_RESULTS.md)** - Kết quả test API

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
- **Total Endpoints:** 40+
- **API Categories:** 6
- **Test Coverage:** 98.4%
- **Response Time:** < 50ms

## 🔄 **CHANGELOG**
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
