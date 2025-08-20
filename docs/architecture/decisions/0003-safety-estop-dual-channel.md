# ADR-0003: Safety – Dual-Channel E‑Stop

- Trạng thái: Accepted
- Ngày: 2025-08-19

## Context
E‑Stop là yêu cầu an toàn bắt buộc; cần dừng nhanh, tin cậy, có thể kiểm chứng.

## Decision
- E‑Stop kênh kép với giám sát liên tục; ưu tiên trên mọi lệnh.
- Tích hợp interlock với drive enable và contactor nguồn.
- Ghi event/logs và đo latency E‑Stop.

## Consequences
- Hạ tầng HW/SW phức tạp hơn nhưng an toàn hơn; dễ chứng minh tuân thủ.

## Alternatives
- E‑Stop đơn kênh (không đạt yêu cầu an toàn mục tiêu).

## References
- `docs/specs/state_machine.md`
- `deploy/scripts/estop_latency_test.sh`


