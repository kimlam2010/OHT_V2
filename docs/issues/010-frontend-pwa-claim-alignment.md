# FE-010: Đồng bộ claim PWA với artefacts (manifest, SW)

## Bối cảnh
- Tài liệu nêu PWA nhưng repo không có `manifest.json` và service worker.

## Vấn đề
- Sai lệch giữa docs và code; không thể cài đặt như app.

## Phạm vi
- (A) Gỡ claim PWA khỏi docs, hoặc (B) Thêm manifest + SW (vite-plugin-pwa) và icon assets.

## Checklist công việc
- [ ] Quyết định A/B với PM
- [ ] Nếu B: thêm `vite-plugin-pwa` cấu hình basic
- [ ] Tạo `public/manifest.webmanifest`, icons
- [ ] Kiểm thử installability

## Acceptance Criteria
- [ ] Claim PWA khớp với thực tế
- [ ] Nếu bật PWA: Lighthouse PWA ≥ 90

## Liên quan
- DOC-102 (Docs cập nhật PWA)