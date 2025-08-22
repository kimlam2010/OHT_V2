---
title: "Openapi Review"
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

# OpenAPI Review & CI Docs-Check

## Mục tiêu
- Đảm bảo /docs (OpenAPI) khớp hành vi thực tế và tài liệu.

## Quy trình review
1. Liệt kê endpoints thực tế trong code (`backend/app/api/v1/*`).
2. So sánh response_model/response envelopes với tài liệu (CONFIG_API.md, TELEMETRY_API.md, WEBSOCKET_EVENTS.md).
3. Bổ sung examples trong pydantic models (json_schema_extra) nếu thiếu.
4. Kiểm thử nhanh bằng `docs/backend/curl_suite.sh`.

## CI “docs-check” (đề xuất)
- Link checker: kiểm tra liên kết chéo trong `docs/**.md` còn hợp lệ.
- OpenAPI diff: snapshot OpenAPI JSON và so sánh thay đổi (fail nếu chưa cập nhật docs khi thêm/breaking).
- Lint markdown: MD lints (headings, dead anchors).

## Artefacts
- `docs/backend/postman_collection.json`
- `docs/backend/curl_suite.sh`
