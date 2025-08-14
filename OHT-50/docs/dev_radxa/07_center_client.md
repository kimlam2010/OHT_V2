# 07) Client Center (HTTP/WS hoặc MQTT)

> Ghi chú: Giai đoạn hiện tại tập trung viết ứng dụng trên Radxa (HAL/Control/Location/Safety/Telemetry + API cục bộ). Client Center được giữ ở mức stub hoặc tắt mặc định; nội dung dưới đây dùng làm tham chiếu để sẵn sàng khi cần kích hoạt.

## WS (gợi ý)
- Kết nối: `ws://.../robot?id=<robot_id>` kèm token.
- Nhịp Heartbeat: 1–2 s.
- Gói Telemetry (10–50 Hz): bám `interfaces.md`/`center_comm.md`.
- Reconnect: exponential backoff (max 30 s), resubscribe.

## MQTT (gợi ý)
- Topics:
  - `center/<site>/robot/<id>/telemetry`
  - `center/<site>/robot/<id>/heartbeat`
  - `center/<site>/robot/<id>/events`
  - `robot/<id>/cmd` (mission/permit)
- QoS ≥1; retain messages cấu hình.

## Bảo mật
- Token trong header/param; TLS nếu có; whitelist broker.

## Xử lý permit/mission
- Lưu `mission_ctx`, validate permit trước hành động nhạy cảm (mở cửa/thả hàng).

## TBD
- Định nghĩa schema cuối cùng và auth.
