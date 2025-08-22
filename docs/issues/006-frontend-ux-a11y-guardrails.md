# FE-006: UX/A11y & Guardrails cho thao tác nguy hiểm

## Bối cảnh
- Điều khiển E‑Stop/Reset hiện dùng `window.confirm`.
- Một số tiêu chí a11y cơ bản chưa có: skip link, focus-visible, landmark roles, live region.

## Vấn đề
- Rủi ro thao tác nguy hiểm không đủ guard; Khả năng truy cập chưa đạt WCAG 2.1 mức A/AA cơ bản.

## Phạm vi
- Thêm cơ chế xác nhận nâng cao cho hành động nguy hiểm (hold-to-confirm / require typing / role gating / disabled by state).
- Bổ sung skip link, `:focus-visible` styles, landmark roles (`header/main/nav`), aria-live cho trạng thái WS.

## Checklist công việc
- [ ] Tạo component ConfirmDanger (hold-to-confirm hoặc typed confirm)
- [ ] Áp dụng cho E‑Stop/Reset Fault
- [ ] Thêm skip link đầu trang → `#main-content`
- [ ] Bổ sung focus-visible rõ cho nút/link
- [ ] Landmark roles & aria-labels đầy đủ
- [ ] `aria-live="polite"` cho thông báo/WS state

## Acceptance Criteria
- [ ] Hành động nguy hiểm yêu cầu xác nhận nâng cao, không chỉ `confirm()`
- [ ] Page đạt checklist WCAG cơ bản (nêu trong docs)

## Liên quan
- DOC-104 (WCAG checklist)