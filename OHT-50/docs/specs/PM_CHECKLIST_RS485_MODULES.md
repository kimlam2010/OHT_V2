# Phiếu thu thập thông tin PM — RS485 Modules

Mục tiêu: Thu thập thông số/kịch bản tích hợp cho từng module RS485/CAN để hoàn thiện config, driver và telemetry.

## 1) Chung cho hệ thống bus
- Topology & chiều dài bus:
- Cáp & đầu nối:
- Termination 120Ω vị trí:
- Bias (giá trị, vị trí):
- Baud (RS485), Parity/Stop bits:
- Heartbeat/Keepalive (chu kỳ):
- Timeout (ms), Retry (số lần):

## 2) Ánh xạ địa chỉ
- Safety (0x10 đề xuất):
- DIO Upper (0x11):
- AIO Upper (0x12):
- DC Driver #1 axis_drive (0x13) / Stepper #1 (0x14):
- DC Driver #2 axis_lift (0x15) / Stepper #2 (0x16):
- DIO Cargo (0x17):
- AIO Cargo (0x18):
- DC Driver #3 axis_door (0x19):
- Location RFID+Encoder (0x1A):
- Khác (liệt kê thêm nếu có):

## 3) DC Motor Driver / Stepper Driver
- Hãng/Model:
- Giao thức: Modbus RTU/CANopen/khác:
- Tham số setpoint: velocity/position/torque (đơn vị, giới hạn):
- Feedback: pos/vel/current/temp/fault:
- Encoder tích hợp? loại/ppr:
- Tần số cập nhật mong muốn (setpoint/feedback):
- Điều kiện an toàn & interlock:

## 4) Location Module (RFID + Encoder)
- RFID: chuẩn tag, tần số, khoảng cách đọc, thời gian đáp ứng:
- Dữ liệu tag: có `rssi` không:
- Encoder: loại, ppr, có kênh Z:
- Timestamp: tại CPU hay module:
- Yêu cầu Telemetry: trường cần có (vd `tag_id`, `enc.count`, `s_on_rail`):

## 5) Safety Module
- Kênh vào/ra an toàn (số lượng, loại):
- Latch/Reset điều kiện:
- Thời gian đáp ứng:

## 6) DIO/AIO Modules
- DIO: số kênh In/Out, loại NPN/PNP, debounce, polarity:
- AIO: dải 0–10V hoặc 4–20mA, độ phân giải, tần số mẫu:
- Hiệu chuẩn AI/AO: offset/scale, quy trình:

## 7) LiDAR (nếu có)
- Hãng/Model:
- Giao tiếp: RS485/UART/CAN/Ethernet:
- Tốc độ quét, FOV, phạm vi đo:
- Định dạng dữ liệu:
- Tần số stream/giảm tần mong muốn:

## 8) Telemetry & Logging
- Tần số WS (Hz):
- Trường telemetry bổ sung (ngoài schema chuẩn):
- Sự kiện quan trọng cần log:

## 9) Khác
- Yêu cầu đặc biệt về EMC/ESD/nguồn:
- Firmware version các module:
- Tài liệu/đường dẫn datasheet:
