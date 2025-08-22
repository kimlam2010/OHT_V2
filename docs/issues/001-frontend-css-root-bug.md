# FE-001: Sửa lỗi biến theme không áp dụng do selector `::root`

## Bối cảnh
- File: `frontend/src/styles.css`
- Hiện tại khai báo sử dụng `::root` thay vì `:root`, khiến CSS custom properties không áp dụng toàn cục.

## Vấn đề
- Biến màu sắc/typography không áp vào UI → UI lệch theme, màu mặc định Tailwind/UA.

## Phạm vi
- Sửa selector `::root` → `:root`.
- Kiểm tra nhanh các nơi sử dụng biến: nền, chữ, card, border, ring, charts.

## Checklist công việc
- [ ] Đổi `::root` thành `:root` trong `frontend/src/styles.css` (dòng định nghĩa biến theme)
- [ ] Build dev và prod kiểm tra biến áp dụng (thủ công: UI có đổi màu)
- [ ] Snapshot ảnh trước/sau (tùy chọn)

## Acceptance Criteria
- [ ] Các biến CSS hiển thị đúng trên Dashboard (nền/chữ/card/border/ring)
- [ ] Không còn màu mặc định lệch theme (so với biến trong `:root`)

## Rủi ro/Ảnh hưởng
- Thấp, thay đổi selector thuần.

## Liên quan
- Design system (tokens) – xem FE-103 (Docs)