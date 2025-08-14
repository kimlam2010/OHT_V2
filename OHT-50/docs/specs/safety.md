# An toàn & E‑Stop

## Mục tiêu
- Đạt trạng thái an toàn có kiểm chứng trong mọi tình huống khẩn.
- Ghi nhận và chẩn đoán nguyên nhân để cải thiện độ tin cậy.

## Kiến trúc an toàn đề xuất
- Safety module RS485 để báo trạng thái; kênh phần cứng cắt mạch lực độc lập.
- Liên động:
  - Không cho `axis_drive` chạy khi `axis_door` chưa đóng hoàn toàn.
  - Không cho `axis_lift` chạy khi cửa chưa mở và chưa có permit.
  - Giới hạn tốc độ trong vùng station; cấm chuyển động khi có cảm biến kẹp/chèn kích hoạt.

## Trình tự E‑Stop (mức logic)
```mermaid
sequenceDiagram
  participant IO as E-Stop I/O (HW)
  participant SAFE as Safety Module
  participant CTRL as Control Core
  participant HAL as HAL/Drivers

  IO->>SAFE: estop_on = TRUE (HW chain)
  SAFE->>CTRL: RS485 READ_STATUS(estop=true)
  CTRL->>HAL: ramp_down_all_axes_to_zero
  HAL-->>CTRL: feedback (x,v)
  CTRL-->>SAFE: stopped_all_axes
  SAFE-->>IO: latch_estop_state (HW)
```

## Mất kết nối Center
- Nếu mất `Center` > T_timeout (TBD): giữ/quay về trạng thái an toàn; không mở cửa/không thả hàng nếu chưa có permit.

## Tiêu chí & kiểm thử
- Đo khoảng cách dừng; xác minh liên động cửa/lift.
- Tiêm lỗi: mất RS485, mất Center, vượt tốc, trigger kẹp/chèn; hệ phải về an toàn.
