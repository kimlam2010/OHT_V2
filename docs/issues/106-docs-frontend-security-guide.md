# DOC-106: Hướng dẫn bảo mật FE (headers, token, WS)

## Nội dung
- Header bảo mật khi serve tĩnh: XCTO, XFO, Referrer-Policy, CSP (nếu có)
- Quản lý token (storage, truyền qua header, tránh query nếu không cần)
- WS auth (subprotocol/query, expiry/refresh)

## Checklist
- [ ] Tài liệu header tối thiểu và nơi áp dụng (server/proxy)
- [ ] Hướng dẫn thêm auth cho WS
- [ ] Liên kết đến FE-008

## Acceptance Criteria
- [ ] Có hướng dẫn rõ ràng để team triển khai an toàn