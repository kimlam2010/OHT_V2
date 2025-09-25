# RS485 Slave Simulator (Modbus RTU) – OHT-50

Mục tiêu: mô phỏng các slave module (Motor 0x01, Power 0x02, Safety 0x03, Dock 0x05) để master kết nối qua RS485 (USB to RS485) trên `/dev/ttyUSB1`.

## Cài đặt
```bash
cd /home/orangepi/Desktop/OHT_V2/tools/slave_RS485
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
```

## Chạy mô phỏng
```bash
# Thiết bị mặc định /dev/ttyUSB1, baud 115200. Có thể override bằng env.
RS485_PORT=/dev/ttyUSB1 RS485_BAUD=115200 python server.py
```

- Unit IDs: 0x01 (Motor), 0x02 (Power), 0x03 (Safety), 0x05 (Dock)
- Giá trị được cập nhật ngẫu nhiên mỗi 200ms, theo register map giống firmware.

## Gợi ý test từ master
- Đọc Holding Register từ các địa chỉ trong header tương ứng (FC03)
- Ghi Single Register để thay đổi mục tiêu như `MOTOR_POSITION_TARGET_REG` (0x0004)

Lưu ý: Đây là công cụ mô phỏng cho development/lab, KHÔNG dùng production.
