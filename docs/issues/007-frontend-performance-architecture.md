# FE-007: Tối ưu hiệu năng & kiến trúc FE

## Bối cảnh
- Router chưa code-splitting; chart real-time có nguy cơ re-render dày; chưa có error telemetry.

## Vấn đề
- Tải ban đầu lớn; drop FPS khi dữ liệu dày; khó truy vết lỗi prod.

## Phạm vi
- Dùng `React.lazy` + `Suspense` cho route lớn.
- Điều tiết render chart (requestAnimationFrame/throttle).
- Thêm error telemetry (Sentry/OpenTelemetry) – optional.

## Checklist công việc
- [ ] Code-splitting các route nặng (dashboard, chart)
- [ ] Thêm scheduler cho cập nhật chart (throttle 16–33ms)
- [ ] Thêm error boundary báo cáo telemetry (tùy chọn)

## Acceptance Criteria
- [ ] Lighthouse Performance ≥ 85 (desktop)
- [ ] FPS ổn định khi stream telemetry (manual)

## Liên quan
- CI-202 (Lighthouse in CI)