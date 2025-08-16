# OTA: Ký & Kiểm toàn vẹn

Phiên bản: v1.0 (khởi tạo)

## 1) Mục tiêu
- Đảm bảo gói OTA hợp lệ, không bị chỉnh sửa; có thể rollback an toàn.

## 2) Khóa & chữ ký
- Sinh cặp khóa (offline): `oht-ota-{env}.pem`
- Ký gói `.tar.zst` chứa: app, config, manifest
- Manifest chứa: phiên bản, sha256 từng file, timestamp, chữ ký

## 3) Quy trình
1) Build artefact → tạo manifest → ký
2) Tải lên server phát hành an toàn (HTTPS)
3) Thiết bị tải về, kiểm tra sha256 + chữ ký
4) Áp dụng vào staging dir → smoke test → switch atomically
5) Nếu lỗi → rollback bản trước

## 4) Công cụ
- `openssl dgst -sha256 -sign key.pem -out manifest.sig manifest.json`
- `openssl dgst -sha256 -verify pub.pem -signature manifest.sig manifest.json`

## 5) Chính sách quay lui
- Giữ ít nhất 2 phiên bản trước
- Không xoá bản đang chạy cho đến khi bản mới ổn định ≥ T phút
