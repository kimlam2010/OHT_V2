# Trạng thái hệ thống (State Machine)

## Danh mục trạng thái
- `Idle`: chờ lệnh/mission.
- `Move`: di chuyển trên ray theo quỹ đạo S‑curve.
- `Approach`: tiếp cận station (vùng giảm tốc).
- `Align`: canh chỉnh vị trí tại station bằng cảm biến.
- `DoorOpen`: mở cửa thùng.
- `CargoOp`: thao tác thả/nâng thùng (lift).
- `DoorClose`: đóng cửa.
- `Leave`: rời station, tăng tốc ra khỏi vùng.
- `Fault`: lỗi.
- `E-Stop`: dừng khẩn.

## Sơ đồ trạng thái
```mermaid
stateDiagram-v2
  [*] --> Idle
  Idle --> Move: cmd_move/mission
  Move --> Approach: near_station
  Approach --> Align: speed<threshold & dock_align
  Align --> DoorOpen: permit.open_door
  DoorOpen --> CargoOp: door_opened
  CargoOp --> DoorClose: cargo_done
  DoorClose --> Leave: door_closed & permit.leave_station
  Leave --> Idle: clear_of_station

  Idle --> Fault: fault
  Move --> Fault: fault
  Approach --> Fault: fault
  Align --> Fault: fault
  DoorOpen --> Fault: fault
  CargoOp --> Fault: fault
  DoorClose --> Fault: fault
  Leave --> Fault: fault
  Fault --> Idle: recovered

  [*] --> EStop: estop_on
  Idle --> EStop: estop_on
  Move --> EStop: estop_on
  Approach --> EStop: estop_on
  Align --> EStop: estop_on
  DoorOpen --> EStop: estop_on
  CargoOp --> EStop: estop_on
  DoorClose --> EStop: estop_on
  Leave --> EStop: estop_on
  EStop --> Idle: estop_off & safe_to_reset
```

## Quy tắc ưu tiên & timeout
- `E-Stop` thắng mọi trạng thái.
- Timeout cho `Align`, `DoorOpen`, `CargoOp`, `DoorClose` (TBD) → vào `Fault`.
- Chỉ vào `Approach` khi có `permit.enter_station` từ Center.

## Log sự kiện bắt buộc
- Mission id, station id, thời gian từng bước, kết quả cảm biến, lỗi.

## Ghi chú cảm biến vị trí
- `Approach`: kích hoạt khi `s_on_rail` gần `s_station` (từ RFID/encoder), giảm tốc theo vùng cấu hình.
- `Align`: xác nhận `tag_id` của station; dùng encoder để tinh chỉnh đến vị trí dừng trước khi mở cửa.
