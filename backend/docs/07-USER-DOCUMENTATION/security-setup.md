# 🔒 OHT-50 Backend Security Setup Guide

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Loại tài liệu:** User Documentation  
**Mức độ bảo mật:** Confidential  

---

## 🎯 **TỔNG QUAN**

OHT-50 Backend yêu cầu cấu hình JWT secret an toàn cho production deployment. Tài liệu này hướng dẫn cách cấu hình bảo mật JWT đúng cách.

## 🔐 **YÊU CẦU BẢO MẬT**

### **Môi trường Production**
- **JWT_SECRET BẮT BUỘC** - Ứng dụng sẽ không khởi động được nếu thiếu
- Phải là chuỗi ngẫu nhiên mã hóa an toàn
- Tối thiểu 32 ký tự (256 bits) được khuyến nghị
- Phải là duy nhất cho mỗi môi trường (dev/staging/prod)

### **Môi trường Testing**
- JWT_SECRET là tùy chọn khi `TESTING=true`
- Secret dự phòng được sử dụng tự động cho testing
- Không có validation bảo mật trong chế độ testing

## ⚙️ **HƯỚNG DẪN THIẾT LẬP**

### **1. Tạo JWT Secret**

Sử dụng script được cung cấp để tạo JWT secret an toàn:

```bash
python scripts/generate_jwt_secret.py
```

Script này sẽ tạo ra một secret ngẫu nhiên 256-bit an toàn.

### **2. Cấu hình Environment**

Thêm secret đã tạo vào file `.env`:

```bash
# Security - BẮT BUỘC CHO PRODUCTION
JWT_SECRET=your-generated-secret-here
JWT_ALGORITHM=HS256
JWT_EXPIRY=3600

# Testing (tự động thiết lập trong quá trình test)
TESTING=false
```

### **3. Cấu hình theo Môi trường**

**Development:**
```bash
JWT_SECRET=dev-secret-here
TESTING=false
```

**Staging:**
```bash
JWT_SECRET=staging-secret-here
TESTING=false
```

**Production:**
```bash
JWT_SECRET=production-secret-here
TESTING=false
```

## 🛡️ **BEST PRACTICES BẢO MẬT**

### **1. Quản lý Secret**
- **Không bao giờ commit secret vào version control**
- Sử dụng hệ thống quản lý biến môi trường (AWS Secrets Manager, Azure Key Vault, etc.)
- Xoay secret định kỳ (khuyến nghị mỗi 90 ngày)
- Sử dụng secret khác nhau cho mỗi môi trường

### **2. Tạo Secret**
- Sử dụng trình tạo ngẫu nhiên mã hóa an toàn
- Tối thiểu 32 ký tự (256 bits)
- Bao gồm chữ cái, số và ký tự đặc biệt
- Tránh từ điển hoặc mẫu dự đoán được

### **3. Bảo mật Deployment**
- Thiết lập JWT_SECRET qua biến môi trường trong production
- Sử dụng phương pháp injection secret an toàn
- Giám sát việc tiết lộ secret trong logs
- Triển khai quy trình xoay secret

## ✅ **VALIDATION**

Ứng dụng validate cấu hình JWT_SECRET khi khởi động:

### **Production Mode (TESTING=false)**
- JWT_SECRET phải được thiết lập và không rỗng
- Ứng dụng không khởi động được nếu thiếu
- Thông báo lỗi rõ ràng được cung cấp

### **Testing Mode (TESTING=true)**
- JWT_SECRET là tùy chọn
- Secret dự phòng được sử dụng tự động
- Không có lỗi validation

## 🚨 **XỬ LÝ LỖI**

Nếu JWT_SECRET bị thiếu trong production:

```
ValueError: JWT_SECRET must be set via environment variable for production. 
Set JWT_SECRET in your .env file or environment variables.
```

## 🔧 **TROUBLESHOOTING**

### **Ứng dụng không khởi động**
1. Kiểm tra JWT_SECRET có được thiết lập trong environment
2. Xác minh biến môi trường TESTING
3. Kiểm tra cấu hình file .env
4. Đảm bảo không có lỗi chính tả trong tên biến

### **Lỗi Authentication**
1. Xác minh JWT_SECRET khớp giữa các services
2. Kiểm tra cấu hình JWT_ALGORITHM
3. Xác minh thiết lập JWT_EXPIRY
4. Kiểm tra đồng bộ hóa đồng hồ hệ thống

## 📋 **SECURITY CHECKLIST**

- [ ] JWT_SECRET được thiết lập cho production
- [ ] Secret an toàn mã hóa (256+ bits)
- [ ] Secret khác nhau cho mỗi môi trường
- [ ] Secret không được commit vào version control
- [ ] Lịch trình xoay secret được thiết lập
- [ ] Hệ thống quản lý secret an toàn được triển khai
- [ ] Giám sát việc tiết lộ secret
- [ ] Quy trình backup và recovery cho secret

## 📁 **FILES LIÊN QUAN**

- `app/config.py` - Validation cấu hình
- `app/core/security.py` - Triển khai JWT
- `env.example` - Template environment
- `scripts/generate_jwt_secret.py` - Trình tạo secret

## 📞 **HỖ TRỢ**

Đối với các câu hỏi hoặc vấn đề liên quan đến bảo mật:
1. Kiểm tra tài liệu này
2. Xem xét kỹ thông báo lỗi
3. Xác minh cấu hình environment
4. Liên hệ team phát triển

---

**⚠️ QUAN TRỌNG:** Đây là cấu hình quan trọng về bảo mật. Đảm bảo tất cả thành viên team hiểu và tuân thủ các hướng dẫn này.

---

**Changelog:**
- **v1.0 (2025-01-28):** Di chuyển từ root docs/ và cập nhật theo chuẩn ISO

