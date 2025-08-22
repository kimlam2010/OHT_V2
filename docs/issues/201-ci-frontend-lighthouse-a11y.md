# CI-201: Thêm Lighthouse Performance & A11y audit cho Frontend trong CI

## Bối cảnh
- CI chưa đo lường hiệu năng/a11y cho FE.

## Phạm vi
- Dùng `lhci` (Lighthouse CI) hoặc `treosh/lighthouse-ci-action` để chạy trên build preview.
- Target desktop (hoặc 2 cấu hình desktop/mobile)
- Báo cáo Performance/Accessibility/Best Practices/SEO.

## Checklist
- [ ] Cài LHCI devDep trong FE hoặc dùng action
- [ ] Job CI build FE, serve tĩnh, chạy Lighthouse
- [ ] Lưu artefact reports
- [ ] Fail nếu Perf < 85 hoặc A11y < 90 (ngưỡng tạm)

## Acceptance Criteria
- [ ] CI tạo báo cáo Lighthouse và enforce ngưỡng