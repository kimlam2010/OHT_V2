# EMBED TEAM EXECUTION PROMPT - OHT-50 Master Module

Phiên bản: v1.0  
Ngày tạo: 2025-01-28  
Team: EMBED (Hardware & Drivers)  
Trạng thái: 🎯 SUPPORT + VALIDATION EXECUTION

---

## 🎯 Mục tiêu
- Duy trì nền tảng HW/Drivers ổn định đã hoàn tất (100%), cung cấp artefact xác nhận.
- Chuẩn hoá RS485 addressing/wiring (0x02–0x07), hướng dẫn thực địa.
- Hỗ trợ Phase 1–4 cho FW (HIL, loopback, latency) và DevOps (Startup Sequence).

---

## 📋 SPRINT ORDERS (7 ngày)

### Day 1–2: RS485 Addressing & Wiring (Field-Ready)
```bash
# TODO LIST
□ Soạn docs/EMBED/RS485_ADDRESSING_GUIDE.md
   - Dải địa chỉ: 0x02–0x07
   - Cách đặt địa chỉ (DIP/Jumper/UART cmd)
   - Quy tắc dán nhãn, tránh trùng địa chỉ
   - Wiring: termination 120Ω, bias, shield/ground
□ Chuẩn hoá checklist test nhanh (loopback/BER/latency)
□ Thêm sơ đồ đấu nối/bảng vật tư (nếu có)
□ Commit: "EMBED: RS485 addressing & wiring guide (0x02–0x07)"
```
Expected:
```bash
Artefact: docs/EMBED/RS485_ADDRESSING_GUIDE.md
```

### Day 2–3: RS485 Loopback & BER/Latency Validation
```bash
# TODO LIST
□ Chạy loopback 60 phút @115200 8N1, ghi BER + latency p95
□ Thử termination/bias để chốt khuyến nghị
□ Báo cáo: docs/reports/rs485_loopback_results.md
□ Commit: "EMBED: RS485 loopback/BER/latency report"
```
Expected:
```bash
./tools/rs485_loopback_test.sh --duration 3600 --baud 115200
# Kỳ vọng: BER ≤ 0.1%, latency p95 < 50ms, 0 timeout
```

### Day 3–4: Startup Sequence Validation (v2.0)
```bash
# TODO LIST
□ Xác minh DT overlays (uart1_46_47.dtbo) + udev /dev/ttyOHT485
□ Kiểm thứ tự systemd: network-online → oht50.service
□ Ghi marker cho bài test E-Stop latency (journalctl)
□ Báo cáo: docs/reports/startup_validation_embed.md
□ Commit: "EMBED: Startup sequence validation (DT/udev/systemd)"
```
Expected:
```bash
- /dev/ttyOHT485 tồn tại, trỏ đúng UART1
- oht50.service khởi chạy sau network-online
- Marker bài test E-Stop sẵn cho FW đo <100ms
```

### Day 4–5: GPIO/Pin Final Audit + Artefact Pack
```bash
# TODO LIST
□ Audit cuối: LED(54,35,28,29,58), E-Stop(59), Relay(131,132), UART1(46,47)
□ Lưu gpioinfo/gpioset + phiên bản kernel/board
□ Pack artefacts: artefacts/embed_hw_validation_Wx.zip
□ Commit: "EMBED: Final pin audit + artefact pack"
```
Expected:
```bash
- Log gpioinfo/gpioset có timestamp
- Artefact zip chứa logs + hướng dẫn re-run test
```

### Day 5–7: Hỗ trợ FW/QA/DevOps (On-Call)
```bash
# TODO LIST
□ SLA hỗ trợ HIL test (FW safety/comm) trong ngày
□ Góp ý chống nhiễu (EMI/ESD) nếu có spike
□ Hỗ trợ DevOps hoàn thiện Startup Sequence gating
□ Commit (nếu thay đổi): "EMBED: HIL support fixes"
```
Expected:
```bash
- Không còn blocker HW trong E2E
- Tài liệu cập nhật ngay khi có phát hiện
```

---

## ✅ Definition of Done (EMBED)
- GPIO/RS485 audited; guides đủ dùng hiện trường.
- Loopback/BER/latency report đạt ngưỡng.
- Startup Sequence validation hoàn tất; artefacts bàn giao QA/DevOps.
- Hỗ trợ HIL không còn blocker phần cứng.

---

## 🧪 Test & Acceptance
- GPIO tests: gpioinfo/gpioset đúng mapping.
- RS485 loopback 60 phút: BER ≤ 0.1%, p95 latency < 50ms.
- Startup: systemd order đúng, device nodes đúng.
- Artefacts: hướng dẫn + logs + sơ đồ đấu nối.

---

## 🚨 Escalation
- BER cao: kiểm termination/bias/shield; escalate FW Comms Lead nếu chưa đạt.
- Không tạo /dev/ttyOHT485: kiểm DT/udev; escalate DevOps.
- E-Stop marker lệch: phối hợp FW Safety Lead.

---

## 🔗 Liên kết nhanh
- GPIO_PIN_MAPPING_SPEC.md, COMMUNICATION_SPEC.md, STARTUP_SEQUENCE_SPEC.md
- ASSIGNMENTS_MATRIX.md (owners/ETA/DoD/Tests)
