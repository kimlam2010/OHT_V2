# SECURITY GUIDE - OHT-50 Backend (v0.3)

## Mục lục
- [Auth & Token](#auth--token)
- [CORS](#cors)
- [Rate-limit](#rate-limit)
- [Secrets & ENV](#secrets--env)
- [TLS/HTTPS](#tlshttps)
- [Tham chiếu](#tham-chiếu)

---

## Auth & Token
- Khi `DEBUG=false`, hầu hết endpoint yêu cầu `Authorization: Bearer <token>`.
- Đăng nhập (Auth mock):
```bash
curl -s -X POST 'http://localhost:8000/api/v1/auth/login' \
  -H 'Content-Type: application/json' \
  -d '{"username":"admin","password":"admin"}'
# → { "token": "<hex>" }
```
- Gọi API kèm token:
```bash
curl -s -H 'Authorization: Bearer <TOKEN>' http://localhost:8000/api/v1/config/
```

## CORS
- `settings.cors_origins` (mặc định `*`) → nên cấu hình domain cụ thể ở prod.

## Rate-limit
- Chưa bật; khuyến nghị đặt tại Nginx/Ingress.

## Secrets & ENV
- `.env` chứa `SECRET_KEY`, DB URL, ... cần rotate và không commit.
- Ví dụ bật prod:
```bash
export DEBUG=false
export LOG_LEVEL=INFO
```

## TLS/HTTPS
- Tham khảo triển khai SSL trong DEPLOYMENT.md hoặc qua reverse proxy.

## Tham chiếu
- Code: `backend/app/core/security.py`, `backend/app/services/auth_service.py`
- Deploy: `docs/backend/DEPLOYMENT.md`
