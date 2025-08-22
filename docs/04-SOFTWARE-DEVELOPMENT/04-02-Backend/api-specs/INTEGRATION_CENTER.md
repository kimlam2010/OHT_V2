---
title: "Integration Center"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "Backend Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['software-development']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# INTEGRATION WITH CENTER/FW - OHT-50 Backend (v0.3)

## Mục lục
- [Tổng quan](#tổng-quan)
- [Endpoints](#endpoints)
- [Cấu hình client](#cấu-hình-client)
- [Lưu ý](#lưu-ý)
- [Tham chiếu](#tham-chiếu)

---

## Tổng quan
- BE nhận mission/permission từ Center; proxy một số dữ liệu từ FW/Center nội bộ.

## Endpoints
- Nhận mission:
```bash
curl -s -X POST 'http://localhost:8000/api/v1/center/mission' \
  -H 'Content-Type: application/json' \
  -d '{"mission_id":"m1","mission_type":"move_to","target_position":{"x":1,"y":2,"z":0,"theta":0}}'
```
- Yêu cầu permission:
```bash
curl -s -X POST 'http://localhost:8000/api/v1/center/permission' \
  -H 'Content-Type: application/json' \
  -d '{"action":"move","resource":"robot","context":{"zone":"A"}}'
```
- Proxy modules/points (FW/Center):
```bash
curl -s http://localhost:8000/api/v1/center/modules
curl -s http://localhost:8000/api/v1/center/points
```

## Cấu hình client
- `Settings.center_http_base` (mặc định `http://localhost:8081`)
- `Settings.center_http_timeout_s` (mặc định 3.0)

## Lưu ý
- Cần bổ sung auth/headers khi kết nối Center thật; xem xét retry/backoff.

## Tham chiếu
- Code: `backend/app/api/v1/center.py`, `backend/app/services/center_client.py`
