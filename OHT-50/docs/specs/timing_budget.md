# Ngân sách thời gian vòng lặp điều khiển (Timing Budget)

Phiên bản: v1.0 (khởi tạo)

## 1) Mục tiêu
- Chu kỳ điều khiển mục tiêu: 10 ms (100 Hz) — điều chỉnh theo yêu cầu động học
- Jitter mục tiêu: ≤ 1 ms p95
- Deadline từng tác vụ rõ ràng; watchdog giám sát

## 2) Phân rã tác vụ (ví dụ)
| Tác vụ | Ngân sách (ms) | Ghi chú |
|---|---:|---|
| Read Encoders/IO | 1.0 | SPI/UART/CAN + parse |
| Safety Checks | 0.5 | Interlocks, E‑Stop, vùng cấm |
| Planner/Controller | 2.0 | PID/FF + giới hạn v/a/j |
| HAL Dispatch → Drivers | 1.0 | Gửi lệnh, hàng đợi |
| Telemetry Buffering | 0.5 | Không block vòng lặp |
| Margin | 5.0 | Dự phòng & GC |

## 3) Ưu tiên & Threading
- RT priority: Control > Safety > Drivers > Telemetry
- Pin CPU core cho Control/Safety nếu có thể
- Tránh I/O blocking trong luồng control

## 4) Watchdog & Deadline
- Soft watchdog: cảnh báo nếu vòng lặp > 12 ms; chuyển Fault nếu > 50 ms
- Hard watchdog (ngoài): reset hệ nếu treo

## 5) Đo lường
- Log thời gian thực thi mỗi tác vụ; xuất p50/p95/p99
- Dùng `clock_gettime(CLOCK_MONOTONIC)` hoặc tương đương Python/Node
