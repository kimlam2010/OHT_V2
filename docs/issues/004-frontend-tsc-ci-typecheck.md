# FE-004: Bổ sung type-check (`tsc --noEmit`) vào CI

## Bối cảnh
- CI hiện chỉ build + test; không có bước type-check riêng.

## Vấn đề
- Lỗi kiểu có thể không phá build nhưng gây runtime issues.

## Phạm vi
- Thêm script `typecheck`: `tsc --noEmit`
- Thêm job/step trong CI chạy `npm run typecheck` trước build.

## Checklist công việc
- [ ] Thêm script `typecheck` vào `frontend/package.json`
- [ ] Cập nhật workflow `.github/workflows/ci-cd.yml` để chạy `typecheck`
- [ ] Đảm bảo fail PR nếu type lỗi

## Acceptance Criteria
- [ ] CI chạy type-check cho FE
- [ ] PR fail khi có lỗi kiểu

## Liên quan
- FE-003 (Lint/Prettier)