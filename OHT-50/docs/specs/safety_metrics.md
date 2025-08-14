# Tiêu chí an toàn đo được

Phiên bản: v1.0 (khởi tạo)

## 1) Khoảng cách dừng
- Đo từ thời điểm kích hoạt E‑Stop đến vị trí dừng hẳn
- Mục tiêu: < TBD mm ở vận tốc khai thác

## 2) Thời gian phản ứng E‑Stop
- Từ kích hoạt đến cắt lực kéo + state = E‑Stop
- Mục tiêu: < TBD ms

## 3) Giám sát vùng tốc độ/cấm
- Không vượt `vmax_zone`; không xâm nhập vùng cấm

## 4) Độ tin cậy phát hiện lỗi
- Phát hiện lỗi encoder/driver/comm trong < 100 ms

## 5) Ghi nhận & truy vết
- Lưu 100 sự kiện an toàn gần nhất, kèm timestamp và nguyên nhân
