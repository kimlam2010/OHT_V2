# CI-202: Bật ngưỡng coverage cho FE trong CI

## Bối cảnh
- CI chạy test FE nhưng không enforce coverage.

## Phạm vi
- Bật coverage trong Vitest và thêm threshold (lines ≥ 70% khởi điểm).
- Upload coverage (Codecov tùy chọn, đã dùng cho backend).

## Checklist
- [ ] Bật coverage trong cấu hình Vitest
- [ ] Cập nhật workflow FE để fail nếu dưới ngưỡng
- [ ] Upload coverage (nếu dùng Codecov)

## Acceptance Criteria
- [ ] CI fail khi coverage FE dưới ngưỡng đặt ra