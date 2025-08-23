# udev rules — Alias cố định cho RS485 trên Orange Pi 5B (v1.0)

Mục tiêu: Tạo alias ổn định `/dev/ttyOHT485` trỏ tới cổng RS485 chính (UART1 → `ttyS1`), tránh phụ thuộc tên thiết bị kernel.

## 1) Nội dung rule

```udev
# /etc/udev/rules.d/99-ttyOHT485.rules
KERNEL=="ttyS1", SYMLINK+="ttyOHT485", TAG+="systemd"
```

Lưu ý: Nếu RS485 không phải `ttyS1`, điều chỉnh `KERNEL=="ttySx"` tương ứng.

## 2) Cài đặt & nạp lại udev

```bash
echo 'KERNEL=="ttyS1", SYMLINK+="ttyOHT485", TAG+="systemd"' | sudo tee /etc/udev/rules.d/99-ttyOHT485.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
ls -l /dev/ttyOHT485 | cat
```

## 3) Xác nhận sau reboot

```bash
sudo reboot
# đăng nhập lại rồi kiểm tra
ls -l /dev/ttyOHT485 | cat
```

## 4) Tích hợp dịch vụ/ứng dụng

- Ứng dụng/HAL nên ưu tiên mở `/dev/ttyOHT485`; fallback `/dev/ttyS1` nếu alias chưa có.
- Ghi log cảnh báo khi fallback để dễ chẩn đoán.


