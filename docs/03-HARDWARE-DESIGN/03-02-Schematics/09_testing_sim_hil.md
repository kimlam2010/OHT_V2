---
title: "09 Testing Sim Hil"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "Hardware Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['hardware-design', 'testing', 'validation']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# 09) Kiểm thử (Unit/Integration/Sim/HIL)

## Unit
- CRC16, parser frame, planner S‑curve biên, state machine đường đi hạnh phúc & lỗi.

## Integration
- HAL RS485 giả lập → lệnh/feedback cho motor/io/safety/location.
- Center client cục bộ (mock WS/MQTT).

## Simulation
- `sim/rail_1d`: chạy profile đến station; đo sai số dừng.

## HIL (Hardware-in-the-Loop)
- Nối tối thiểu 1 driver, 1 DIO, Safety; đo timeout/retry và khoảng cách dừng.

## Tiêu chí chấp nhận
- Tất cả test xanh; không vi phạm giới hạn; E‑Stop đúng trình tự.
