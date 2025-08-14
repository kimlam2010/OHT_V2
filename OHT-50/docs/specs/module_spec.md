# Đặc tả module (cấu tạo & khả năng)

Mục tiêu: mô tả khả năng phần cứng/điện và giao thức của từng module để cấu hình/chọn chế độ vận hành.

## Digital IO Module (8 In, 8 Out)
- Input: 8 kênh, loại NPN/PNP (TBD), điện áp danh định 24 VDC (TBD), cách ly (TBD), debounce (mặc định 5–20 ms).
- Output: 8 kênh, PNP/NPN (TBD), dòng tối đa/kênh (TBD), bảo vệ quá dòng/quá nhiệt (TBD).
- Chức năng: đọc `READ_DI`, ghi `WRITE_DO`, cấu hình `CONFIG_MODE` (debounce, polarity).
- Chẩn đoán: trạng thái kênh, lỗi ngắn mạch, quá áp (nếu hỗ trợ).

## Analog IO Module (AI/AO)
- AI: số kênh (TBD), dải 0–10 V hoặc 4–20 mA (TBD), độ phân giải 12–16 bit (TBD), tần số mẫu (TBD), lọc số IIR (TBD).
- AO: số kênh (TBD), dải 0–10 V hoặc 4–20 mA (TBD), độ phân giải (TBD).
- Chức năng: `READ_AI[n]`, `WRITE_AO[n]`, `CONFIG_RANGE`.
- Hiệu chuẩn: offset/scale (TBD quy trình).

## Safety Module
- Ngõ vào: E‑Stop chain, interlock cửa/thang, cảm biến kẹp/chèn (TBD số kênh).
- Ngõ ra an toàn: enable/contactor (HW), latch trạng thái.
- RS485: `READ_STATUS`, `RESET_SAFETY` (đòi hỏi điều kiện an toàn thoả mãn).
- Thời gian đáp ứng: tức thời trên kênh HW; RS485 chỉ để giám sát/latch.

## DC Motor Driver (2 kênh)
- Điện áp/công suất: (TBD) VDC, dòng tối đa/kênh (TBD), bảo vệ quá dòng/nhiệt.
- Chế độ: velocity/torque/position (tùy driver, TBD). Hỗ trợ encoder? (TBD type, ppr).
- Lệnh: `ENABLE/DISABLE`, `SET_VEL`, `SET_POS`, `SET_TORQUE`, `STOP_RAMP`, `READ_FB {pos, vel, i, temp, fault}`.
- Tần số cập nhật đề nghị: 50–100 Hz setpoint; feedback ≥ 50 Hz.

## Stepper Motor Driver (2 kênh)
- Dòng pha tối đa (TBD), vi bước (microstep) (TBD phạm vi), đóng/vòng lặp kín (encoder tùy chọn).
- Lệnh: `ENABLE/DISABLE`, `SET_POS`, `SET_VEL`, `HOME`, `STOP_RAMP`, `READ_FB {pos, fault}`.
- Giới hạn: ramp tốc/accel (TBD), limit switch hỗ trợ qua DI.

## Location Module (RFID + Encoder)
- RFID: tần số/chuẩn tag (TBD), khoảng cách đọc (TBD), thời gian đáp ứng (TBD), chống nhiễu.
- Encoder: loại incremental (TBD), phân giải (ppr), có kênh index (Z) hay không.
- Lệnh RS485 (gợi ý):
  - RFID: `READ_TAG_ID`, `SUBSCRIBE_TAG` (sự kiện `{t, tag_id, rssi?}`), `CLEAR_TAG_EVENT`.
  - ENCODER: `READ_COUNT`, `RESET_COUNT`, `READ_VEL`, `READ_STATUS`.
- Tốc độ cập nhật: tag theo sự kiện; encoder ≥ 100 Hz đọc/ước lượng vận tốc.
- Đồng bộ thời gian: dấu thời gian `t` từ CPU master, hoặc timestamp tại module (TBD).
- Ứng dụng: suy ra `s_on_rail` bằng nội suy encoder giữa các tag; hiệu chỉnh trôi khi gặp tag.

## Thông số môi trường & cơ khí (chung)
- Nhiệt độ vận hành (TBD), rung/va (TBD), chuẩn connector (TBD), tiêu chuẩn EMC (TBD).

## Ghi chú
- Các `TBD` sẽ được chốt theo datasheet module thực tế. Mapping địa chỉ xem `module_map.md`.
