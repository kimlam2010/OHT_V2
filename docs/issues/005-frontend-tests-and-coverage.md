# FE-005: Thêm test (Vitest + RTL) và ngưỡng coverage trong CI

## Bối cảnh
- Có Vitest/RTL nhưng chưa có file test trong `frontend/src`.
- CI chạy `npm test` nhưng thực tế 0 test.

## Vấn đề
- Không kiểm soát hồi quy; claim docs về test/coverage chưa đúng.

## Phạm vi
- Tạo tối thiểu 6–10 bài test: routing/guard auth, http client, WS backoff, control confirm, chart render (smoke), theme toggle.
- Cấu hình coverage và ngưỡng trong CI (ví dụ lines ≥ 70% khởi điểm).

## Checklist công việc
- [ ] Thêm test cho `Guard` và routing
- [ ] Test `apiFetchJson` (error/success)
- [ ] Test WS backoff (mock timers)
- [ ] Test confirm nguy hiểm (E‑Stop) hiển thị prompt
- [ ] Smoke test `Fe06Chart` render
- [ ] Test theme toggle lưu/persist
- [ ] Bật coverage và upload (tuỳ chọn Codecov)
- [ ] CI fail nếu dưới ngưỡng (>=70%)

## Acceptance Criteria
- [ ] Số test ≥ 6, chạy pass trong CI
- [ ] Coverage tổng ≥ 70%

## Liên quan
- FE-201 (CI coverage threshold – nếu tách riêng)