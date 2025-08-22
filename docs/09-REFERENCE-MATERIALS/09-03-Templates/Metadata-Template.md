---
title: "Document Title"
version: "v1.0.0"
date_created: "YYYY-MM-DD"
date_modified: "2025-08-22"
author: "Author Name"
reviewer: "Reviewer Name"
approver: "Approver Name"
status: "Draft|Review|Approved|Archived"
classification: "Internal|External|Confidential"
security_level: "Public|Internal|Confidential|Secret"
tags: ["tag1", "tag2", "tag3"]
related_docs: ["doc1", "doc2"]
compliance: ["ISO 9001:2015", "IEEE 12207"]
---

# 📋 **TEMPLATE METADATA CHUẨN**

## 🎯 **MỤC ĐÍCH**

Template chuẩn cho metadata headers của tất cả files trong hệ thống tài liệu OHT-50 Master Module.

## 📋 **HƯỚNG DẪN SỬ DỤNG**

### **1. Title (Bắt buộc)**
- Mô tả ngắn gọn nội dung file
- Format: Sentence case
- Example: "System Architecture Baseline"

### **2. Version (Bắt buộc)**
- Semantic Versioning: `vX.Y.Z`
- Major.Minor.Patch
- Example: "v1.2.3"

### **3. Dates (Bắt buộc)**
- Format: ISO 8601 `YYYY-MM-DD`
- date_created: Ngày tạo file
- date_modified: Ngày cập nhật cuối

### **4. Authorship (Bắt buộc)**
- author: Người tạo/sửa chính
- reviewer: Người review
- approver: Người approve (CTO/PM)

### **5. Status (Bắt buộc)**
- Draft: Đang viết
- Review: Đang review
- Approved: Đã approve
- Archived: Đã lưu trữ

### **6. Classification (Bắt buộc)**
- Internal: Nội bộ công ty
- External: Đối tác, khách hàng
- Confidential: Bảo mật cao

### **7. Security Level (Bắt buộc)**
- Public: Công khai
- Internal: Nội bộ
- Confidential: Bảo mật
- Secret: Tuyệt mật

### **8. Tags (Khuyến nghị)**
- Từ khóa tìm kiếm
- Tối đa 5 tags
- Format: lowercase, hyphenated

### **9. Related Docs (Khuyến nghị)**
- Files liên quan
- Format: relative paths
- Tối đa 5 references

### **10. Compliance (Bắt buộc)**
- Tiêu chuẩn tuân thủ
- Example: ["ISO 9001:2015", "IEEE 12207"]

## 📝 **VÍ DỤ HOÀN CHỈNH**

```yaml
---
title: "RS485 Motor Control Integration Guide"
version: "v2.1.0"
date_created: "2025-01-15"
date_modified: "2025-01-27"
author: "EMBED Team"
reviewer: "Firmware Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Confidential"
tags: ["rs485", "motor-control", "integration", "modbus"]
related_docs: ["02-03-Interface-Definitions/modbus-spec", "05-02-Integration-Tests/rs485-tests"]
compliance: ["ISO 9001:2015", "IEEE 12207"]
---
```

## 🚨 **QUY TẮC BẮT BUỘC**

1. **Metadata phải có đầy đủ** cho tất cả files
2. **Version phải cập nhật** mỗi khi sửa đổi
3. **Date_modified phải cập nhật** mỗi khi sửa đổi
4. **Status phải chính xác** theo workflow
5. **Compliance phải đúng** với tiêu chuẩn áp dụng

## 🔄 **WORKFLOW CẬP NHẬT**

1. **Tạo file mới:** Copy template này
2. **Edit content:** Cập nhật metadata phù hợp
3. **Review:** Reviewer kiểm tra metadata
4. **Approve:** Approver xác nhận
5. **Publish:** Merge với metadata đầy đủ

---

**Changelog v1.0.0:**
- ✅ Created metadata template
- ✅ Defined all required fields
- ✅ Added usage guidelines
- ✅ Provided complete example
- ✅ Established update workflow
