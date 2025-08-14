# Quy tắc Cursor Dev Radxa – OHT-50 (Lập trình)

## 1) Phạm vi & mục tiêu
- Phạm vi: hiện thực lớp HAL RS485, vòng điều khiển 1D, hợp nhất vị trí, an toàn phần mềm, logging/telemetry và API cục bộ trên Radxa S0; client Center chỉ stub (tuỳ chọn) ở giai đoạn này.
- Mục tiêu giai đoạn đầu (Sprint 0): khung chạy được với `hal_mock`, telemetry + API cục bộ (`GET /status`, `POST /command`, `WS /telemetry`), test cơ bản.

## 2) Kết luận về mức độ đủ của tài liệu hiện tại
- Đủ để khởi động lập trình với khung mã, mô phỏng và client Center ở mức cơ bản.
- Chưa đủ để nối thiết bị thật do còn nhiều `TBD`. Cần mặc định tạm để không chặn tiến độ.

## 3) Thông số bắt buộc còn thiếu cần chốt (TBD)
- RS485: CRC chuẩn (CCITT hay Modbus), chân GPIO DE/RE cụ thể, timeout đọc cuối cùng, backoff giữa lần gửi.
- Ánh xạ địa chỉ module khớp thực tế với `module_map.md` (xác nhận lần cuối).
- Trục `axis_drive`: `v_max`, `a_max`, `j_max`, `dt` xác nhận; chiều dài ray; giới hạn vùng giảm tốc.
- Encoder: `encoder_ppr`, bán kính bánh `wheel_radius_m`, tỉ số truyền (gear).
- Location: bảng tag cuối cùng (đủ tất cả station), chiến lược hiệu chỉnh trượt.
- Center: `mode` (ws|mqtt), `url`, `auth_token`, schema gói lệnh/telemetry (bám `center_comm.md`).
- An toàn: trình tự E‑Stop phần mềm khớp phần cứng, timeout các bước `Align/Door/Cargo`.
- Bảo mật: dùng TLS hay không; khoá CA/client; whitelist broker/host.

## 4) Chiến lược khi thiếu thông số
- Dùng giá trị mặc định an toàn trong `config/system.yaml` và đánh dấu `# TBD_default`.
- Cung cấp mock HAL để chạy unit/integration mà không cần phần cứng.
- Bao toàn bộ thông số vào cấu hình để thay đổi mà không sửa mã.

## 5) Cấu trúc thư mục/lớp (khởi tạo)
- `control/`:
  - `profiles/` S‑curve/Trapezoid.
  - `planner.py` nhận mục tiêu và giới hạn, phát setpoint {x,v,a} theo `dt`.
  - `state_machine.py` Idle/Move/Dock/Fault/E‑Stop.
- `drivers/`:
  - `bus_rs485.py` (frame, CRC, timeout, retry).
  - `hal.py` (MotorInterface, IOInterface, SafetyInterface, LocationInterface) + `hal_mock.py`.
- `services/`:
  - `local_api/` `api.py` (FastAPI): `GET /health`, `GET /status`, `POST /command`, `WS /telemetry`.
  - `center_client/` stub (tắt mặc định giai đoạn này).
- `sim/`:
  - `rail_1d.py` (mô phỏng đơn giản quán tính + ma sát).
- `tests/`:
  - Unit: CRC16, parser frame, profile biên, state machine đường đi hạnh phúc & lỗi.
  - Integration: HAL giả lập + planner + state machine + client Center cục bộ.
- `docs/`:
  - Cập nhật sơ đồ Mermaid khi đổi kiến trúc hoặc state.

## 6) Chuẩn công nghệ & chất lượng
- Python 3.11, `venv` theo `01_overview_setup.md`.
- Async: control/IO tách luồng; `pyserial-asyncio` cho RS485; `libgpiod` cho DE/RE; `uvloop` (tùy chọn) để giảm jitter.
- Local API: FastAPI + uvicorn; WS qua FastAPI WebSocket.
- Lint/format: `ruff` + `black` (mặc định), không lỗi linter.
- Test: `pytest` + `pytest-asyncio` cho phần bất đồng bộ.
- Logging: JSON dòng, rotate theo dung lượng; tối thiểu gồm `timestamp,state,events,commands,faults,bus_rtt_ms,retry_count`.

## 7) Hợp đồng API tối thiểu giai đoạn đầu
- `services.local_api`:
  - HTTP: `GET /health`, `GET /status`, `POST /command`.
  - WS: `/telemetry` 10–50 Hz, cơ chế backpressure đơn giản.
- RS485 frame: `[0xAA][ADDR][CMD][LEN][PAYLOAD...][CRC16]`; mặc định CRC16-CCITT (có thể đổi toàn cục khi chốt).
- `drivers.hal.MotorInterface`:
  - `enable(addr)`, `disable(addr)`, `set_velocity(addr,v)`, `set_position(addr,x)`, `stop_ramp(addr,limit)`, `read_feedback(addr)->{pos,vel,fault}`.
- `drivers.hal.LocationInterface`:
  - `read_tag_event(addr)->{t,tag_id}?`, `read_encoder(addr)->{count,vel}`, `reset_encoder(addr)`.
- `control.planner` đầu ra tick `{x,v,a}`; không vi phạm giới hạn; hỗ trợ dừng ±1–2 mm mặc định.
- `services.center_client` (tuỳ chọn): WS/MQTT theo `07_center_client.md`.

## 8) Quy trình làm việc trong Cursor
1) Khảo sát nhanh bối cảnh → xác định thiếu số liệu → đặt mặc định tạm.
2) Thiết kế ngắn gọn (kèm sơ đồ Mermaid nếu cần) → thực thi trực tiếp file liên quan.
3) Viết test trước với mock; chạy test; sửa lỗi đến xanh.
4) Kiểm tra linter; sửa đến sạch lỗi.
5) Cập nhật tài liệu trong `docs/dev_radxa/*` đúng mục.

## 9) Tiêu chí hoàn thành Sprint 0
- Chạy được vòng điều khiển ảo với `hal_mock` và xuất telemetry tối thiểu qua WS cục bộ.
- API cục bộ hoạt động: `GET /health`, `GET /status`, `POST /command`, `WS /telemetry` stream.
- Unit/integration xanh; không lỗi linter; cấu hình qua `config/system.yaml`.
- Sơ đồ Mermaid kiến trúc + trạng thái được thêm vào `docs/`.

## 10) Ưu tiên triển khai ngay (thứ tự)
1) `drivers/bus_rs485.py` + CRC16 (kèm test) và `drivers/hal_mock.py`.
2) `control/profiles/` + `planner.py` (Trapezoid trước, S‑curve sau) + test biên.
3) `control/state_machine.py` 5 trạng thái cơ bản + test đường đi hạnh phúc & E‑Stop.
4) `services/local_api/api.py` (health/status/command/ws telemetry).
5) `sim/rail_1d.py` đơn giản để kiểm tra dừng ±1–2 mm.
6) `services/center_client/ws_client.py` stub (tắt mặc định).

## 11) Checklist cấu hình mẫu (`config/system.yaml`)
- `rs485`: `port`, `baud`, `parity`, `stop_bits`, `de_re_gpio`, `retry`, `timeout_ms`.
- `modules`: địa chỉ theo `module_map.md`.
- `axes.axis_drive`: `type`, `driver_addr`, `encoder`, `limits{v_max,a_max,j_max}`, `dt`, `rail_length`.
- `location`: `tag_table`, `encoder_ppr`, `wheel_radius_m`.
- `center`: `mode`, `url`, `auth_token`, `heartbeat_s`.
- `telemetry`: `rate_hz`, `fields`.

## 12) Rủi ro & biện pháp
- Sai CRC/timeout bus: đo `bus_rtt_ms`, tăng `timeout_ms`, giảm baud nếu cần.
- Drift vị trí do encoder: hiệu chỉnh theo tag, giới hạn bước hiệu chỉnh mỗi tick.
- Mất mạng: backoff + hàng đợi gửi lại; không chặn vòng điều khiển.
- E‑Stop: luôn ưu tiên ngắt lệnh và ramp xuống 0 ngay trong state machine.

## 13) Ghi chú cuối
- Không tạo file tóm tắt thừa; cập nhật trực tiếp tài liệu trong `docs/`.
- Khi chốt thông số thật, thay thế các `TBD_default` trong cấu hình và cập nhật test tương ứng.
