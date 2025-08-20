# HƯỚNG DẪN ĐỊA CHỈ RS485 & WIRING – OHT‑50

Phiên bản: 1.0 • Ngày: 2025-01-28 • Chủ sở hữu: EMBED Lead • Trạng thái: Ready to use

Tài liệu ngắn gọn, thực dụng để chuẩn hoá dải địa chỉ RS485 (0x02–0x07), cách đặt địa chỉ phần cứng và hướng dẫn wiring/termination/bias nhằm giảm nhiễu, phục vụ cơ chế “Dynamic + Managed”. Tham chiếu: `docs/FIRMWARE/MODULE_MANAGEMENT_SPEC.md` (mục tiêu quét 0x02–0x07) và `docs/tasks/EMBED_TASKS_RS485_ADDRESSING.md`.

---

## 1) Phạm vi & nguyên tắc
- Bus: RS485 2‑wire half‑duplex (A/B), topology dây chuỗi (daisy-chain), tránh sao (star).
- Dải địa chỉ tiêu chuẩn: 0x02–0x07 theo đặc tả quản lý module.
- Mỗi module phải có địa chỉ duy nhất; không dùng 0x00 (broadcast).
- Tốc độ danh định: 115200 bps (mặc định FW); có thể hạ xuống 38400 bps khi cáp dài/nhiễu.
- Linux device khuyến nghị: `/dev/ttyOHT485` (udev rule), fallback: `/dev/ttyS1`.

---

## 2) Kế hoạch địa chỉ (đề xuất) 0x02–0x07
Lưu ý: có thể tuỳ biến theo site, nhưng cần dán nhãn rõ ràng và cập nhật `modules.yaml`.

- 0x02: Power/IO nguồn hoặc Module I/O chung
- 0x03: Motor/X (động cơ chuyển động chính)
- 0x04: Docking/IO docking
- 0x05: IO Safety (interlock, E‑Stop interface)
- 0x06: Sensor (encoder/limit/obstacle)
- 0x07: Dự phòng/thiết bị mở rộng cùng loại

Quy tắc dán nhãn: dán tem gồm `ADDR=0xNN`, `TYPE`, `Tên/Slot`, ngày/lần hiệu chuẩn (nếu có).

---

## 3) Cách đặt địa chỉ phần cứng

Tùy model module, có 1 trong 3 cách: DIP switch, Jumper chọn sẵn, hoặc lệnh UART/Modbus để ghi ID.

### 3.1 DIP switch (khuyến nghị khi có)
- Mặc định: DIP “ON” = logic 1 (một số hãng ngược lại, cần kiểm tra silkscreen/datasheet).
- Dùng 3 bit thấp để biểu diễn các giá trị 0x02–0x07.

| Địa chỉ | Nhị phân (b2 b1 b0) | Gợi ý DIP (b2 b1 b0) |
|--------:|---------------------:|----------------------:|
| 0x02    | 010                  | ON OFF ON             |
| 0x03    | 011                  | ON ON  ON             |
| 0x04    | 100                  | OFF ON OFF            |
| 0x05    | 101                  | OFF ON ON             |
| 0x06    | 110                  | ON  ON OFF            |
| 0x07    | 111                  | ON  ON ON             |

Ghi chú: Nếu bo mạch định nghĩa thêm offset cơ sở (ví dụ 0x00/0x01 reserved), hãy cộng/trừ theo tài liệu hãng.

### 3.2 Jumper selector
- Một số module có jumper chọn sẵn địa chỉ (in trực tiếp trên PCB). Hãy đối chiếu bảng silkscreen để gán 0x02–0x07 tương ứng.
- Khoá jumper sau khi chọn, dán nhãn địa chỉ lên vỏ.

### 3.3 Cấu hình qua lệnh UART/Modbus
- Dùng địa chỉ hiện tại (hoặc địa chỉ mặc định của nhà sản xuất) để ghi thanh ghi “Device ID”.
- Quy trình mẫu (giả định thanh ghi 0x0001 là Device ID, chấp nhận broadcast chỉ với lệnh write):
  1) Kết nối 1 module duy nhất lên bus (tránh trùng địa chỉ khi broadcast).
  2) Gửi lệnh Modbus Function 0x06 (Write Single Register) tới broadcast 0x00 hoặc địa chỉ hiện tại, ghi giá trị mới ∈ {0x02..0x07} vào addr 0x0001.
  3) Ngắt nguồn 3–5 giây và cấp lại điện để module nạp ID mới (tuỳ hãng có thể không cần power‑cycle).
- Sau khi đặt, dán nhãn địa chỉ và cập nhật bảng theo dõi.

---

## 4) Wiring, termination & bias (giảm nhiễu)

### 4.1 Topology & đấu dây
- Dây chuỗi (daisy‑chain) từ Master → M1 → M2 → … → Mn; không rẽ nhánh sao.
- Cặp xoắn (twisted pair) cho A/B, có shield. Giữ cặp A với A, B với B (tùy ký hiệu: D−/D+ hoặc A/B). Nếu đọc sai cực, truyền thông có thể chập chờn → đảo lại.
- Kết nối tham chiếu đất (SGND) giữa các node về một điểm chung (single‑point), tránh vòng lặp đất.

### 4.2 Termination (kết thúc đường truyền)
- Gắn điện trở 120Ω tại HAI đầu xa nhất của đường bus (chỉ 2 điểm). Không gắn termination ở giữa.
- Với dây ngắn (< 20 m, 115200 bps), có thể tắt 1 đầu nếu thực nghiệm cho thấy ổn định hơn; mặc định vẫn khuyến nghị đủ 2 đầu.

### 4.3 Bias (điện áp kéo về mức idle)
- Cần có bộ điện trở kéo (fail‑safe bias) để giữ đường truyền ở mức “idle” khi không có thiết bị truyền, tránh nhiễu tạo khung giả.
- Vị trí: ưu tiên đặt tại Master (một điểm duy nhất trên bus).
- Giá trị tham khảo: Rpull‑up ≈ 680Ω–1k lên Vcc cho D+, Rpull‑down ≈ 680Ω–1k xuống GND cho D− (hoặc theo khuyến cáo của transceiver). Mục tiêu tạo chênh lệch tĩnh ≥ 200 mV ở trạng thái rỗi.

### 4.4 Chiều dài & tốc độ
- 115200 bps: khuyến nghị ≤ 100 m với cáp công nghiệp chất lượng tốt. Nếu dài hơn, hạ baud xuống 38400 bps hoặc 19200 bps.
- Tránh chạy song song gần dây nguồn/motor công suất; nếu bắt buộc, tách khoảng cách và dùng shield nối đất một đầu.

### 4.5 EMI/ESD
- Dùng cáp shield, đầu shield nối chassis ground tại một điểm (gần Master).
- Đảm bảo có bảo vệ ESD/TVS theo khuyến cáo của transceiver; bố trí dây gọn, bán kính uốn vừa phải.

```mermaid
flowchart LR
  M[Master\n(termination+bias)] --- A1((M1)) --- A2((M2)) --- A3((M3))
  A3 --- A4((M4))
  M:::end <--> A4:::end
  classDef end stroke:#333,stroke-width:1px;
```

---

## 5) Checklist trước khi cấp nguồn
- [ ] Xác nhận địa chỉ từng module ∈ 0x02–0x07, không trùng nhau
- [ ] Dán nhãn `ADDR=0xNN` trên vỏ module
- [ ] Topology dây chuỗi; không có nhánh sao
- [ ] Termination 120Ω chỉ tại hai đầu bus
- [ ] Bias chỉ tại một điểm (Master)
- [ ] Kết nối SGND giữa các node (single‑point)
- [ ] Cáp xoắn, shield đúng cách; tránh chạy sát dây công suất

---

## 6) Kiểm thử nhanh (BER/latency) & lệnh mẫu

### 6.1 Discover/ping (CLI)
```bash
# Liệt kê nhanh theo phần mềm mô phỏng hiện có
python3 tools/module_cli.py discover
python3 tools/module_cli.py list
python3 tools/module_cli.py ping --addr 0x02
```

### 6.2 Kiểm thử vòng lặp (loopback) – nếu có tool
```bash
# ví dụ chạy liên tục 60s và thống kê lỗi (tùy tool sẵn có)
./tools/rs485_loopback_test.sh --duration 60 --baud 115200
```

Gợi ý đo: đếm số khung lỗi/CRC/timeout, ước tính BER ~ (bit lỗi/bit truyền). Nếu BER cao, thử:
- Giảm baud rate (38400/19200)
- Kiểm tra termination/bias
- Kiểm tra đảo cực A/B
- Kiểm tra topology (loại bỏ nhánh sao)
- Kiểm tra nối đất và khoảng cách tới dây công suất

---

## 7) Sự cố thường gặp & cách xử lý
- Trùng địa chỉ: scan thấy 1 hoặc nhấp nháy không ổn định → tháo từng nhánh để cô lập, đặt lại ID, dán nhãn.
- Đảo cực A/B: thiết bị “chập chờn”, nhiều CRC fail → đảo A↔B trên toàn tuyến (đồng nhất ký hiệu D+/D−).
- Thiếu bias: bus nhiễu khi rỗi, nhận khung giả → thêm/kiểm tra bộ điện trở bias tại Master.
- Termination sai: lắp 3–4 điện trở dọc tuyến → phản xạ, timeout → chỉ giữ 2 đầu bus.
- Topology sao: phản xạ trên nhánh → chuyển về dây chuỗi, dùng stub ngắn nhất có thể.
- Cáp/đường đi: chạy gần motor/power → tăng khoảng cách, dùng shield tốt, nối đất một đầu.

---

## 8) Tương thích với phần mềm quản lý module
- Đội FW quét địa chỉ 0x02–0x07 khi khởi động (xem `MODULE_MANAGEMENT_SPEC.md`).
- Tham số timeout/debounce: retry=3; backoff 50→100→200 ms; OFFLINE sau 2 lần miss liên tiếp.
- API/CLI (Phase 1):
  - `module_cli discover|list|ping --addr 0xNN`
  - Backend (mở rộng): `/api/v1/modules`, `/api/v1/modules/scan`, `/api/v1/modules/{addr}`

---

## 9) Tài liệu hoá & báo cáo
- Cập nhật `modules.yaml` (addr, type, name/labels) sau khi gán địa chỉ.
- Đính kèm ảnh wiring/termination/bias thực tế (nếu có) trong PR.
- Báo cáo kết quả scan ổn định 0x02–0x07 cho FW (log/ảnh màn hình CLI).

---

## 10) Phụ lục: Bảng quyết định nhanh
| Tình huống | Hành động nhanh |
|---|---|
| Mất gói/CRC cao | Giảm baud → kiểm bias/termination → kiểm A/B → kiểm topology |
| Không thấy module | Kiểm trùng địa chỉ → đo dây → cấp nguồn module → ping riêng lẻ |
| Jitter trạng thái | Cải thiện nối đất, tách khỏi dây công suất, thêm ferrite/shield |



