# ADR-0002: Runtime Configuration vs Firmware Boundaries

- Trạng thái: Accepted
- Ngày: 2025-08-19

## Context
Thay đổi số lượng/địa chỉ module và tham số vận hành diễn ra thường xuyên; không thể reflash FW mỗi lần.

## Decision
- Phân tách: protocol/safety core bất biến; cấu hình vận hành (v_max/a_max/j_max, IO mapping, docking, BMS...) là runtime.
- Lưu cấu hình runtime trong NVM module + quản lý bởi Master/Center (JSON/YAML versioned, safe-apply, rollback).

## Consequences
- Đổi cấu hình không cần lập trình lại; giảm downtime.
- Cần cơ chế kiểm tra schema, dry-run, audit.

## Alternatives
- Cố định cấu hình trong FW (bị loại vì thiếu linh hoạt và tăng rủi ro vận hành).

## References
- `docs/specs/interfaces.md`
- `docs/specs/state_machine.md`


