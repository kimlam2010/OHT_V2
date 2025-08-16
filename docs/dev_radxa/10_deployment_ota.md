# 10) Triển khai dịch vụ & OTA

## systemd services
- `oht50.service` (HAL + Control + API local)
- `oht50-center.service` (Center client)

Ví dụ unit:
```ini
[Unit]
Description=OHT50 Main Service
After=network-online.target

[Service]
User=oht
WorkingDirectory=/opt/oht50
Environment="VIRTUAL_ENV=/opt/oht50/venv"
Environment="PATH=/opt/oht50/venv/bin:/usr/bin"
ExecStart=/opt/oht50/venv/bin/python -m oht50.main --config /opt/oht50/config/system.yaml
Restart=always
RestartSec=3

[Install]
WantedBy=multi-user.target
```

## OTA & rollback (gợi ý)
- Tải gói phiên bản (zip) → giải nén `/opt/oht50/releases/<ver>` → symlink `current`.
- Kiểm tra health → nếu fail: rollback symlink về phiên bản trước.

## Bảo mật
- Phân quyền thư mục, token Center, khoá TLS (nếu có).

## TBD
- Chi tiết quy trình build/gói và lịch bảo trì.
