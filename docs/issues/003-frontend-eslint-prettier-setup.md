# FE-003: Thiết lập ESLint + Prettier và kiểm tra trong CI

## Bối cảnh
- Repo chưa có cấu hình ESLint/Prettier cho Frontend.
- CI không chạy lint/format check cho FE.

## Vấn đề
- Thiếu chuẩn hóa code, dễ lọt lỗi, khó review.

## Phạm vi
- Thêm `eslint`, `@typescript-eslint/*`, `eslint-plugin-react`, `eslint-plugin-react-hooks`, `prettier`, `eslint-config-prettier`.
- Tạo `.eslintrc.cjs`, `.prettierrc`, và script npm.
- Thêm bước CI chạy lint.

## Checklist công việc
- [ ] Cài devDeps ESLint/Prettier
- [ ] Tạo `.eslintrc.cjs` (TS + React + hooks)
- [ ] Tạo `.prettierrc` và `.prettierignore`
- [ ] Thêm script npm: `lint`, `lint:fix`, `format`
- [ ] Cập nhật `.github/workflows/ci-cd.yml`: job FE lint trước build

## Acceptance Criteria
- [ ] `npm run lint` không báo lỗi nghiêm trọng
- [ ] CI fail nếu lint fail

## Liên quan
- FE-004 (Type-check CI), FE-005 (Tests)