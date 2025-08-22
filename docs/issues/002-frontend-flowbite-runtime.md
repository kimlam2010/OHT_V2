# FE-002: Bổ sung Flowbite runtime vào SPA

## Bối cảnh
- Tailwind đã bật plugin: `tailwind.config.js` → `plugins: [require('flowbite/plugin')]`
- Chưa import JS runtime của Flowbite trong ứng dụng React.

## Vấn đề
- Các component cần JS (dropdown, modal, tooltip, collapse...) có thể không hoạt động.

## Phạm vi
- Import `'flowbite'` vào entry `frontend/src/main.tsx`.
- Nếu không dùng component tương tác → xác nhận và cập nhật docs.

## Checklist công việc
- [ ] Thêm `import 'flowbite'` vào đầu `frontend/src/main.tsx`
- [ ] Kiểm tra một component mẫu (tooltip/dropdown) hoạt động
- [ ] Cập nhật ghi chú trong `frontend/README.md`

## Acceptance Criteria
- [ ] Ít nhất một component Flowbite tương tác hoạt động trong dev
- [ ] Không có lỗi bundling liên quan Flowbite

## Rủi ro/Ảnh hưởng
- Thấp; tăng nhẹ bundle nếu dùng rộng.

## Liên quan
- FE-101 (README) cập nhật hướng dẫn