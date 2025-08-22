# FE-008: Bảo mật FE static server & WebSocket auth

## Bối cảnh
- `frontend/server.js` không set các security headers; WS kết nối không đính kèm token.

## Vấn đề
- Giảm bảo mật clickjacking/XSS; WS bị từ chối nếu backend yêu cầu auth.

## Phạm vi
- Thêm header cơ bản: `X-Content-Type-Options: nosniff`, `Referrer-Policy: no-referrer`, `X-Frame-Options: DENY` (hoặc CSP nếu có scope).
- Xem xét CSP ở môi trường deploy (có thể đặt ở reverse proxy nếu dùng).
- Bổ sung cơ chế WS auth (token via query param hoặc subprotocol – tùy backend).

## Checklist công việc
- [ ] Thêm middleware set headers trong `server.js`
- [ ] Tài liệu CSP (nếu áp dụng qua proxy)
- [ ] Kết nối WS kèm token nếu backend yêu cầu

## Acceptance Criteria
- [ ] Header cơ bản xuất hiện khi serve prod
- [ ] WS kết nối thành công vào endpoint bảo vệ (nếu bật)

## Liên quan
- DOC-106 (Security guide)