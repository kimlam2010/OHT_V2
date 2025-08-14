# RS485/CAN Transceiver: Wiring & Cấu hình

Phiên bản: v1.0 (khởi tạo)

## 1) RS485
- Topology: bus 2 dây A/B, half-duplex.
- Termination: 120Ω ở hai đầu tuyến.
- Bias: kéo lên/kéo xuống để xác định trạng thái idle (ví dụ 680Ω–4.7kΩ tùy chiều dài, thử nghiệm để tối ưu SNR).
- Dây: cặp xoắn; che chắn nếu > 5 m.
- Tốc độ: 9,6 kbps → 1 Mbps (đề xuất 115200/250k tuỳ driver).
- Frame: 8N1; khoảng lặng giữa khung theo Modbus RTU: ~3.5 char time.

### Test nhanh
```bash
# RS485 dongle ở /dev/ttyOHT485 (udev đã map)
stty -F /dev/ttyOHT485 115200 -echo -onlcr -ocrnl
cat /dev/ttyOHT485 | hexdump -C | cat
```

## 2) CAN (SocketCAN)
- Topology: bus CANH/CANL, termination 120Ω hai đầu.
- Bitrate: 250k/500k/1M (chọn theo driver và chiều dài bus). CAN-FD nếu hỗ trợ.
- Frame: Standard 11-bit hoặc Extended 29-bit.

### Bật interface
```bash
sudo ip link set can0 up type can bitrate 500000
candump can0 | cat
```

## 3) IC gợi ý (TBD)
- RS485: MAX485, ADM485, THVD2450.
- CAN: SN65HVD230/231, MCP2551 (CAN 2.0), hoặc CAN-FD tương thích.

## 4) EMC/ESD
- TVS diode cho đường A/B và CANH/CANL.
- Capa nhỏ (47–100 pF) chống nhiễu cao tần nếu cần; đánh đổi biên độ.

## 5) Checklist triển khai
- [ ] Termination đúng 120Ω hai đầu tuyến.
- [ ] Bias đúng và chỉ đặt tại một vị trí trung tâm (hoặc thiết bị master).
- [ ] Dây cặp xoắn; kiểm tra trở kháng.
- [ ] Bitrate thống nhất toàn hệ.
- [ ] Nhật ký lỗi giao tiếp trong dịch vụ (timeout/retry/error counters).
