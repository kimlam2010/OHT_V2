# Đặc tả điều khiển (Control)

## Phạm vi
- Robot nhà hàng chạy trên ray, gồm 3 trục điều khiển chính: `axis_drive`, `axis_lift`, `axis_door`.
- Planner S‑curve cho `axis_drive`; sequencer điều khiển `axis_lift`/`axis_door` khi vào station.
- Hỗ trợ DC/Stepper qua RS485; tương tác với Center (mission, permit).

## Đơn vị & ký hiệu
- Đơn vị SI: m (met), s (giây), m/s (vận tốc), m/s² (gia tốc), m/s³ (jerk), V, mA.
- Ký hiệu dùng chung: `v_max`, `a_max`, `j_max`, `dt`, `s_on_rail`.

## Mục tiêu kỹ thuật
- `axis_drive`: không vượt `v_max/a_max/j_max`, dừng ±1–2 mm.
- `axis_lift`/`axis_door`: tuân thủ giới hạn tốc/biên hành trình, liên động an toàn.
- Vùng giảm tốc trước station; không chạy khi cửa mở (liên động).

## Mô hình & thông số (TBD)
- `axis_drive`: `{v_max, a_max, j_max, dt, rail_length}`.
- `axis_lift`: `{speed, accel?, stroke, sensors: {top, bottom, load}}`.
- `axis_door`: `{open_time, close_time, limitsw: {open, close}}`.
- Cảm biến: dock_align, obstacle, cargo_present, door_state, load_cell.

## Planner S‑curve (axis_drive)
- Pha jerk giới hạn → tăng tốc hằng → chỉnh jerk → cruising → đối xứng giảm tốc.
- Nội suy rời rạc theo `dt` sinh setpoint {x,v,a}.

## Sequencer tại station (door/lift)
- Điều kiện vào: `permit.enter_station == true` từ Center + tốc độ dưới ngưỡng.
- Trình tự mẫu:
  1) Align bằng `dock_align` + giảm tốc về 0.
  2) Chờ `permit.open_door` → mở `axis_door` đến `limitsw.open`.
  3) Thực hiện `drop/lift` qua `axis_lift` theo cấu hình `stroke`.
  4) Đóng `axis_door` đến `limitsw.close`.
  5) Chờ `permit.leave_station` → rời station.

## Mapping planner → HAL
- `axis_drive`: `set_velocity` hoặc `set_position` (nếu driver hỗ trợ vị trí ổn định).
- `axis_lift`: `set_position` theo hành trình hoặc `set_velocity` + cảm biến biên.
- `axis_door`: `open/close` tốc độ cố định, xác nhận bằng limit switch.

## Sự kiện/Trạng thái
- Sự kiện: `cmd_move(target)`, `cmd_station(station_id)`, `permit_*`, `sensor_*`, `fault_*`, `estop_*`.
- Trạng thái: xem `state_machine.md`.

## Tiêu chí chấp nhận
- Dừng ±2 mm tại station, cửa/lift hoạt động đúng trình tự và liên động.
- Không vi phạm giới hạn v/a/j và biên hành trình; phản ứng E‑Stop đúng.

## Ước lượng vị trí `s_on_rail` (RFID + encoder)
- Nguồn đo: `tag_id` (tuyệt đối theo bảng hiệu chuẩn) + `encoder_count` (tương đối).
- Thuật toán gợi ý:
  - Dead‑reckoning bằng encoder giữa hai lần gặp tag.
  - Khi gặp tag: hiệu chỉnh `s_on_rail = s_tag + delta_offset` (offset nhỏ nếu cần), reset drift.
  - Lọc/giới hạn: loại bỏ tag trùng/ảo bằng ngưỡng thời gian và dịch chuyển tối thiểu.
- Ứng dụng:
  - Planner đặt vùng giảm tốc trước `s_station` dựa trên `s_on_rail`.
  - Align dùng `tag_id` tại station + encoder để bù sai số đến vị trí dừng.
