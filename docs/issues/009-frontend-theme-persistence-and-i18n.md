# FE-009: Theme persistence & quyết định i18n

## Bối cảnh
- Toggle theme chưa lưu; chuỗi UI cố định tiếng Việt, chưa có i18n.

## Vấn đề
- Trải nghiệm người dùng không ổn định; khó mở rộng đa ngôn ngữ.

## Phạm vi
- Lưu theme vào `localStorage`, khởi tạo theo system preference; tôn trọng `prefers-reduced-motion`.
- Quyết định i18n: (A) giữ tiếng Việt + docs nêu rõ, hoặc (B) tích hợp i18n (react-i18next) cho `vi`/`en`.

## Checklist công việc
- [ ] Persist theme + đọc system preference
- [ ] Respect reduced-motion
- [ ] ADR quyết định i18n (A/B)
- [ ] Nếu B: khởi tạo nền tảng i18n + 1-2 màn hình mẫu

## Acceptance Criteria
- [ ] Theme giữ nguyên giữa các lần mở app
- [ ] Có ADR i18n và thực thi tương ứng

## Liên quan
- DOC-101 (ADR-FE i18n)