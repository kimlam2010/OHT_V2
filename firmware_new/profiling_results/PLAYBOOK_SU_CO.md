# Playbook sự cố – Module Manager / RS485

## 1) Timeout hàng loạt
- Quan sát: `timeouts` tăng nhanh trong `soak_report.md` và log `[COMM_MGR]`.
- Xử lý tức thời:
  - Giảm tải bus: tăng `health_interval_ms`, giảm `scan` tạm thời.
  - Tăng `cb_base_cooldown_ms` và/hoặc `cb_fail_threshold` trong YAML, reload FW.
  - Kiểm tra dây/cổng RS485, termination 120Ω, bias.
- Điều tra nguyên nhân:
  - Bật DEBUG, ghi lại `crc_error_count`, `timeout_count`, p95/p99.
  - Dò điện áp/nhiễu nguồn, kiểm tra shielding.

## 2) CRC tăng bất thường
- Quan sát: `crc_errors` tăng, p95/p99 tăng.
- Xử lý tức thời:
  - Bật `enable_crc_check`, thay cáp, kiểm tra đầu nối.
  - Giảm tốc độ baud tạm thời, thử lại.
- Điều tra nguyên nhân:
  - Kiểm tra ESD/EMI, routing, nối đất.
  - So sánh khung lỗi theo địa chỉ để khoanh vùng module lỗi.

## 3) Flapping ONLINE↔OFFLINE
- Quan sát: `flapping` tăng, WS xuất hiện `offline`/`discovered` liên tiếp.
- Xử lý tức thời:
  - Tăng `offline_threshold_ms`, thêm jitter health-check.
  - Tăng `cb_fail_threshold` để tránh mở breaker quá sớm.
- Điều tra nguyên nhân:
  - Kiểm tra nguồn phụ tải, rung/tiếp xúc lỏng.

## 4) Memory Leak nghi ngờ
- Quan sát: `RSS kB` tăng dần trong soak.
- Xử lý tức thời:
  - Giảm tần suất WS (debounce tăng lên 1000 ms), batch tối đa.
- Điều tra nguyên nhân:
  - Rà soát cấp phát/giải phóng trong API/WS, vòng lặp polling.

## 5) Checklist sau xử lý
- Cập nhật YAML, lưu diff.
- Chạy lại soak ≥ 60’ và so sánh KPI: p95<300ms, Timeout<1%, CRC<0.5%, không flapping.
- Ghi nhận kết quả vào `profiling_results/soak_report.md` và cập nhật mục 16 (Post‑implementation).
