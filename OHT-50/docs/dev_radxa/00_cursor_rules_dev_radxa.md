# Quy tắc Cursor Dev Embedded – OHT-50 (v2.0)

## 1) Phạm vi & mục tiêu
- **Platform**: Orange Pi 5B (RK3588) thay vì Radxa S0
- **Công cụ chính**: `opwiring` (Orange Pi Wiring Tool)
- **Phạm vi**: hiện thực lớp HAL RS485, vòng điều khiển 1D, hợp nhất vị trí, an toàn phần mềm, logging/telemetry và API cục bộ
- **Mục tiêu giai đoạn đầu (Sprint 0)**: khung chạy được với `hal_mock`, telemetry + API cục bộ (`GET /status`, `POST /command`, `WS /telemetry`), test cơ bản với opwiring

## 2) Kết luận về mức độ đủ của tài liệu hiện tại
- ✅ **Đủ để khởi động**: tài liệu Orange Pi 5B + opwiring đã cập nhật
- ✅ **Hardware mapping**: UART1 cho RS485, GPIO1_D3/D2 cho DE/RE
- ✅ **Test tools**: script `test_rs485.sh` với opwiring
- ⚠️ **Cần wiring**: RS485 transceiver + module thật để test end-to-end

## 3) Thông số bắt buộc còn thiếu cần chốt (TBD)
- **RS485**: CRC chuẩn (CCITT hay Modbus), timeout đọc cuối cùng, backoff giữa lần gửi
- **Ánh xạ địa chỉ module**: khớp thực tế với `module_map.md` (xác nhận lần cuối)
- **Trục `axis_drive`**: `v_max`, `a_max`, `j_max`, `dt` xác nhận; chiều dài ray; giới hạn vùng giảm tốc
- **Encoder**: `encoder_ppr`, bán kính bánh `wheel_radius_m`, tỉ số truyền (gear)
- **Location**: bảng tag cuối cùng (đủ tất cả station), chiến lược hiệu chỉnh trượt
- **Center**: `mode` (ws|mqtt), `url`, `auth_token`, schema gói lệnh/telemetry (bám `center_comm.md`)
- **An toàn**: trình tự E‑Stop phần mềm khớp phần cứng, timeout các bước `Align/Door/Cargo`
- **Bảo mật**: dùng TLS hay không; khoá CA/client; whitelist broker/host

## 4) Chiến lược khi thiếu thông số
- Dùng giá trị mặc định an toàn trong `config/system.yaml` và đánh dấu `# TBD_default`
- Cung cấp mock HAL để chạy unit/integration mà không cần phần cứng
- Bao toàn bộ thông số vào cấu hình để thay đổi mà không sửa mã
- **Test với opwiring**: UART1, GPIO, RS485 protocol trước khi tích hợp

## 5) Cấu trúc thư mục/lớp (khởi tạo)
- `control/`:
  - `profiles/` S‑curve/Trapezoid
  - `planner.py` nhận mục tiêu và giới hạn, phát setpoint {x,v,a} theo `dt`
  - `state_machine.py` Idle/Move/Dock/Fault/E‑Stop
- `drivers/`:
  - `bus_rs485.py` (frame, CRC, timeout, retry) + test với opwiring
  - `hal.py` (MotorInterface, IOInterface, SafetyInterface, LocationInterface) + `hal_mock.py`
- `services/`:
  - `local_api/` `api.py` (FastAPI): `GET /health`, `GET /status`, `POST /command`, `WS /telemetry`
  - `center_client/` stub (tắt mặc định giai đoạn này)
- `sim/`:
  - `rail_1d.py` (mô phỏng đơn giản quán tính + ma sát)
- `tests/`:
  - Unit: CRC16, parser frame, profile biên, state machine đường đi hạnh phúc & lỗi
  - Integration: HAL giả lập + planner + state machine + client Center cục bộ
  - **Hardware test**: script test với opwiring
- `docs/`:
  - Cập nhật sơ đồ Mermaid khi đổi kiến trúc hoặc state
  - **Hardware docs**: wiring diagram, pin mapping

## 6) Chuẩn công nghệ & chất lượng
- **Platform**: Orange Pi 5B (RK3588), linux 6.1.43-rk3588
- **Tools**: `opwiring` cho UART/GPIO, `pyserial` cho RS485
- Python 3.11, `venv` theo `01_overview_setup.md`
- Async: control/IO tách luồng; `pyserial-asyncio` cho RS485; `opwiring` cho DE/RE; `uvloop` (tùy chọn) để giảm jitter
- Local API: FastAPI + uvicorn; WS qua FastAPI WebSocket
- Lint/format: `ruff` + `black` (mặc định), không lỗi linter
- Test: `pytest` + `pytest-asyncio` cho phần bất đồng bộ
- Logging: JSON dòng, rotate theo dung lượng; tối thiểu gồm `timestamp,state,events,commands,faults,bus_rtt_ms,retry_count`

## 7) Hợp đồng API tối thiểu giai đoạn đầu
- `services.local_api`:
  - HTTP: `GET /health`, `GET /status`, `POST /command`
  - WS: `/telemetry` 10–50 Hz, cơ chế backpressure đơn giản
- **RS485 frame**: `[0xAA][ADDR][CMD][LEN][PAYLOAD...][CRC16]`; mặc định CRC16-CCITT (có thể đổi toàn cục khi chốt)
- **UART mapping**: UART1 (GPIO0_A2/A3) cho RS485, GPIO1_D3 cho DE/RE
- `drivers.hal.MotorInterface`:
  - `enable(addr)`, `disable(addr)`, `set_velocity(addr,v)`, `set_position(addr,x)`, `stop_ramp(addr,limit)`, `read_feedback(addr)->{pos,vel,fault}`
- `drivers.hal.LocationInterface`:
  - `read_tag_event(addr)->{t,tag_id}?`, `read_encoder(addr)->{count,vel}`, `reset_encoder(addr)`
- `control.planner` đầu ra tick `{x,v,a}`; không vi phạm giới hạn; hỗ trợ dừng ±1–2 mm mặc định
- `services.center_client` (tuỳ chọn): WS/MQTT theo `07_center_client.md`

## 8) Quy trình làm việc trong Cursor
1) **Hardware setup**: Cài opwiring → Test UART1/GPIO → Wiring RS485 → Test protocol
2) Khảo sát nhanh bối cảnh → xác định thiếu số liệu → đặt mặc định tạm
3) Thiết kế ngắn gọn (kèm sơ đồ Mermaid nếu cần) → thực thi trực tiếp file liên quan
4) Viết test trước với mock; chạy test; sửa lỗi đến xanh
5) **Hardware test**: Test với opwiring trước khi tích hợp
6) Kiểm tra linter; sửa đến sạch lỗi
7) Cập nhật tài liệu trong `docs/dev_radxa/*` đúng mục

## 9) Tiêu chí hoàn thành Sprint 0
- **Hardware**: UART1 hoạt động, GPIO DE/RE test OK, RS485 protocol test OK
- Chạy được vòng điều khiển ảo với `hal_mock` và xuất telemetry tối thiểu qua WS cục bộ
- API cục bộ hoạt động: `GET /health`, `GET /status`, `POST /command`, `WS /telemetry` stream
- Unit/integration xanh; không lỗi linter; cấu hình qua `config/system.yaml`
- Sơ đồ Mermaid kiến trúc + trạng thái được thêm vào `docs/`

## 10) Ưu tiên triển khai ngay (thứ tự)
1) **Hardware bring-up**: Cài opwiring → Test UART1/GPIO → Wiring RS485 → Test protocol
2) `drivers/bus_rs485.py` + CRC16 (kèm test) và `drivers/hal_mock.py`
3) `control/profiles/` + `planner.py` (Trapezoid trước, S‑curve sau) + test biên
4) `control/state_machine.py` 5 trạng thái cơ bản + test đường đi hạnh phúc & E‑Stop
5) `services/local_api/api.py` (health/status/command/ws telemetry)
6) `sim/rail_1d.py` đơn giản để kiểm tra dừng ±1–2 mm
7) `services/center_client/ws_client.py` stub (tắt mặc định)

## 11) Checklist cấu hình mẫu (`config/system.yaml`)
- `rs485`: `port: /dev/ttyS1`, `baud: 115200`, `parity: none`, `stop_bits: 1`, `de_re_gpio: GPIO1_D3`, `retry: 2`, `timeout_ms: 30`
- `modules`: địa chỉ theo `module_map.md`
- `axes.axis_drive`: `type: dc`, `driver_addr: 0x13`, `encoder: external`, `limits{v_max,a_max,j_max}`, `dt: 0.02`, `rail_length`
- `location`: `tag_table`, `encoder_ppr`, `wheel_radius_m`
- `center`: `mode: ws`, `url: ws://center.local/robot`, `auth_token: TBD`, `heartbeat_s: 2`
- `telemetry`: `rate_hz: 20`, `fields: [s_on_rail, tag_id, encoder_count, axes, safety, io]`

## 12) Rủi ro & biện pháp
- **Hardware issues**: Test với opwiring trước, wiring diagram rõ ràng, termination/bias đúng
- Sai CRC/timeout bus: đo `bus_rtt_ms`, tăng `timeout_ms`, giảm baud nếu cần
- Drift vị trí do encoder: hiệu chỉnh theo tag, giới hạn bước hiệu chỉnh mỗi tick
- Mất mạng: backoff + hàng đợi gửi lại; không chặn vòng điều khiển
- E‑Stop: luôn ưu tiên ngắt lệnh và ramp xuống 0 ngay trong state machine

## 13) Hardware Test Checklist
- [ ] Cài đặt opwiring: `sudo apt install opwiring`
- [ ] Test UART1: `opwiring uart enable 1`, `opwiring uart status 1`
- [ ] Test GPIO: `opwiring gpio set 1 3 1`, `opwiring gpio set 1 3 0`
- [ ] Wiring RS485: UART1 TX/RX → RS485 transceiver → module
- [ ] Test protocol: PING, GET_INFO với module thật
- [ ] Termination 120Ω hai đầu, bias resistor
- [ ] Test loopback: TX → RX

## 14) Ghi chú cuối
- **Platform**: Orange Pi 5B (không phải Radxa S0)
- **Tools**: opwiring cho UART/GPIO test
- **UART mapping**: UART1 cho RS485 (không phải UART0)
- Không tạo file tóm tắt thừa; cập nhật trực tiếp tài liệu trong `docs/`
- Khi chốt thông số thật, thay thế các `TBD_default` trong cấu hình và cập nhật test tương ứng
- **Hardware first**: Test với opwiring trước khi tích hợp software
