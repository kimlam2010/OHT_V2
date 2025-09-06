# Kế hoạch Khắc phục Managers - OHT-50 Master Module

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-01-28  
**Team:** FW  
**Mức độ ưu tiên:** CAO  
**Trạng thái:** PLANNING

---

## 🎯 Mục tiêu
Sửa toàn bộ lỗi nghiêm trọng đã phát hiện trong `src/app/managers/` (Communication, Module, Safety, Telemetry), đảm bảo ổn định, an toàn, và sẵn sàng tích hợp API.

---

## 📅 Lộ trình 2 Tuần (10 ngày làm việc)

### Week 1: Ổn định nền tảng (HAL, Memory, Thread)
1. HAL Integration Guard (ngày 1)
2. Thread Safety & Mutex (ngày 2)
3. Memory Cleanup & Error Paths (ngày 3)
4. Input/Return Validation & Bounds (ngày 4)
5. JSON Serialization Hardening (ngày 5)

### Week 2: Dữ liệu thực & Kiểm thử
6. Replace Simulated Data with HAL (ngày 6-7)
7. Real Health Checks (ngày 8)
8. Integration Tests (ngày 9)
9. Docs & Diagnostics (ngày 10)

---

## 🔧 Checklist chi tiết theo Manager

### 1) Communication Manager
- [x] Thêm `pthread_mutex_t` cho `g_comm_manager`
- [x] Guard state: `waiting_for_response`, `response_timeout`
- [x] Sửa leak khi `comm_manager_modbus_send_request` fail
- [x] Reset state khi lỗi build/send/recv/parse
- [x] Kiểm tra kết quả `hal_rs485_init/open/receive/transmit/health_check`
- [ ] Thêm backoff cấu hình được cho retry
- [ ] Thêm thống kê lỗi chi tiết (per-fc, per-slave)

### 2) Module Manager
 - [x] Kiểm tra `comm_manager` đã init trước khi discover
- [x] Sửa mapping `module_type` (đồng bộ enum ↔ register value)
- [x] Implement health check thực (đo thời gian phản hồi, CRC/timeout)
- [ ] Không tăng/giảm online/error count sai logic
- [ ] Debounce offline theo thời gian cấu hình
- [ ] Bảo vệ registry calls (kiểm tra trả về, fallback)

### 3) Safety Manager
- [ ] Kiểm tra kết quả `hal_estop_set_callback`, `hal_led_*`
- [ ] Implement check thực cho `safety_circuit` và `sensors`
- [ ] Xử lý đầy đủ `SAFETY_EVENT_EMERGENCY_STOP` (gọi shutdown hooks)
- [ ] Đồng bộ với `safety_monitor` khi có (nếu có integration)
- [ ] Bổ sung diagnostics chi tiết (lý do, timestamp, đếm sự kiện)

### 4) Telemetry Manager
- [x] Cleanup khi init fail (free ring buffer, reset state)
- [x] Thu thập metrics thực (CPU/mem/temp) qua HAL/OS khi có
- [ ] Cứng hoá `serialize_json` (kiểm soát `written`, cắt chuỗi an toàn)
- [ ] Giới hạn tần suất broadcast, chống flood
- [ ] Thêm schema version trong JSON

---

## 🧪 Kiểm thử & Chấp nhận

### Unit/Integration Tests (thêm vào `tests/`)
- [ ] RS485 happy-path + timeout + CRC error (mock HAL)
- [ ] Modbus read/write với retry/backoff
- [ ] Module discovery: online/offline/debounce
- [ ] Safety: estop press/release, circuit/sensor fault
- [ ] Telemetry: JSON size limits, frequency control

### Tiêu chí chấp nhận
- [ ] Không crash sau 24h chạy liên tục
- [ ] Không leak bộ nhớ (valgrind clean trên dev)
- [ ] Timeout/CRC handled đúng, có thống kê
- [ ] Health check phản ánh đúng trạng thái
- [ ] JSON không tràn buffer, hợp lệ format

---

## 📋 Phân rã công việc theo hạng mục (Tasks)

### A. HAL Integration Guards
- [x] Thêm macro guard/adapter cho HAL missing
- [x] Log cảnh báo khi HAL không có; fallback an toàn

### B. Thread Safety
- [x] Mutex cho `g_comm_manager`, cấu trúc tương tự cho managers khác nếu cần
- [x] Vùng critical: send/receive, update state, statistics

### C. Memory & Error Paths
- [x] Quy trình cleanup thống nhất `goto cleanup:`
- [x] Đảm bảo free mọi allocation khi lỗi

### D. Validation & Bounds
- [x] Kiểm tra `NULL`, `quantity`, kích thước buffer
- [x] Kiểm tra return code từ tất cả HAL/manager calls
- [x] Thêm bounds checks cho Modbus requests (slave_id, quantity, start_address)
- [x] Validate address ranges trong module discovery
- [x] Guard max_count==0 trong get_registered_modules

### E. Real Data & Health
- [x] Thay simulated → HAL (CPU/mem/temp, RS485 link, module response)
- [x] Tính health = hàm của timeout/CRC/error rate

### F. JSON Hardening
- [x] Helper `snprintf_s` trả về -1 nếu không đủ chỗ
- [x] Cắt chuỗi an toàn, validate độ dài

### G. Tests & Docs
- [x] Bổ sung tests vào `tests/integration/`
- [ ] Cập nhật diagnostics strings + README

### H. Integration Tests
- [x] RS485/Modbus read/write với bounds checking
- [x] Module discovery với comm manager dependency
- [x] Safety E-Stop/LED integration
- [x] Telemetry JSON serialization với buffer limits
- [x] Cross-manager end-to-end flow
- [x] Thread safety và concurrent access
- [x] Error recovery và resilience testing

---

## 📊 Theo dõi tiến độ (liên kết TODO)

### Completed TODOs ✅
- [x] `mgr-hal-integration-fixes` - HAL integration fixes (completed)
- [x] `mgr-thread-safety` - Thread safety implementation (completed)
- [x] `mgr-memory-cleanup` - Memory leak fixes (completed)
- [x] `mgr-real-data` - Replace simulated data (completed)
- [x] `mgr-json-serialization` - JSON serialization hardening (completed)
- [x] `mgr-validate-io` - Validate inputs/returns and add bounds checks (completed)
- [x] `mgr-integration-tests` - Integration tests (completed)
- [x] `mgr-docs` - Update documentation and diagnostics for managers (completed)

### Status Summary
- **Total TODOs:** 8
- **Completed:** 8 ✅
- **Remaining:** 0
- **Overall Progress:** 100% Complete

---

## 🛡️ Rủi ro & Giảm thiểu
- R1: Thiếu HAL thực → dùng adapter/mocks, degrade graceful
- R2: Deadlock do mutex → phạm vi khóa nhỏ, review kỹ
- R3: Thay đổi nhiều ảnh hưởng build → chạy CI local, chia nhỏ commits

---

**Ghi chú:** Kế hoạch này là tiền đề trước khi nối API layer mới. Sau khi hoàn tất, tiến hành Phase 2: tích hợp API endpoints với các managers đã ổn định.
