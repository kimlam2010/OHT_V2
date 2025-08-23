# State Machine hệ thống OHT-50 (Phiên bản 1.0)

## Mục tiêu
- Định nghĩa trạng thái hệ thống và chuyển tiếp giữa các trạng thái cho Master.

## Trạng thái
- Idle: chờ lệnh, hệ an toàn OK.
- Move: di chuyển theo lệnh (profile v‑a‑j), giám sát an toàn liên tục.
- Dock: quy trình docking/charging, interlock sạc.
- Fault: lỗi có thể khôi phục; cần xác nhận/clear để quay về Idle.
- E‑Stop: khẩn cấp; dừng ngay, chỉ thoát khi xác nhận an toàn.

## Sơ đồ trạng thái (Mermaid)
```mermaid
stateDiagram-v2
  [*] --> Idle
  Idle --> Move: cmd.move & safety_ok
  Move --> Idle: cmd.stop | reached_target
  Move --> Dock: cmd.dock & station_ready
  Dock --> Idle: undock_complete
  Idle --> Fault: fault_detected
  Move --> Fault: fault_detected
  Dock --> Fault: fault_detected
  Fault --> Idle: fault_cleared & safety_ok
  Idle --> EStop: estop_pressed
  Move --> EStop: estop_pressed
  Dock --> EStop: estop_pressed
  EStop --> Idle: estop_released & safe_confirm
```

## Sự kiện & điều kiện canh gác (guards)
- cmd.move, cmd.stop, cmd.dock, undock_complete
- safety_ok, station_ready, fault_detected, fault_cleared
- estop_pressed, estop_released, safe_confirm

## Yêu cầu thời gian thực
- Phát hiện E‑Stop: tức thời; phản hồi dừng < 100ms (mục tiêu).
- Giám sát lỗi: quá dòng, quá nhiệt, mất encoder, mất truyền thông.

## Trình tự E‑Stop (rút gọn)
```mermaid
sequenceDiagram
  autonumber
  participant FE as UI
  participant MM as Master
  participant DRV as Drive
  participant PWR as Power/BMS
  FE-->>MM: Hiển thị trạng thái E‑Stop
  Note over MM: estop_pressed
  MM->>DRV: stop_immediate()
  MM->>PWR: cut_motion_enable()
  DRV-->>MM: stopped
  PWR-->>MM: motion_cut
  MM-->>FE: E‑Stop lat, logs
```

## Changelog
- 1.0: Khởi tạo tài liệu, sơ đồ trạng thái và trình tự E‑Stop.


