# 🎨 REQ_FE_Basic_UI - Yêu cầu UI cơ bản Frontend

**Mục đích:** Định nghĩa yêu cầu UI/UX cơ bản cho OHT-50 Master Module Dashboard

---

## 📋 **Thông tin tài liệu:**
- **Phiên bản:** v1.0
- **Ngày tạo:** 2025-01-28
- **Người tạo:** PM Team
- **Trạng thái:** Implementation Guide
- **Cập nhật theo:** System Architecture v2.0

---

## 🎯 **Mục tiêu:**

### **Yêu cầu chính:**
1. **User-Friendly Interface** - Giao diện thân thiện người dùng
2. **Responsive Design** - Thiết kế đáp ứng trên nhiều thiết bị
3. **Basic Interaction** - Tương tác cơ bản đơn giản

---

## 🎨 **Visual Design:**

### **1. Color Scheme:**
Sử dụng theme của ShadcnUI với custom màu primary

```css
/* Basic Color Palette */
:root {
  --radius: 0.65rem;
  --background: oklch(1 0 0);
  --foreground: oklch(0.141 0.005 285.823);
  --card: oklch(1 0 0);
  --card-foreground: oklch(0.141 0.005 285.823);
  --popover: oklch(1 0 0);
  --popover-foreground: oklch(0.141 0.005 285.823);
  --primary: oklch(0.772 0.161 68);
  --primary-foreground: oklch(0.98 0.016 73.684);
  --secondary: oklch(0.967 0.001 286.375);
  --secondary-foreground: oklch(0.21 0.006 285.885);
  --muted: oklch(0.967 0.001 286.375);
  --muted-foreground: oklch(0.552 0.016 285.938);
  --accent: oklch(0.967 0.001 286.375);
  --accent-foreground: oklch(0.21 0.006 285.885);
  --destructive: oklch(0.577 0.245 27.325);
  --border: oklch(0.92 0.004 286.32);
  --input: oklch(0.92 0.004 286.32);
  --ring: oklch(0.772 0.161 68);
  --chart-1: oklch(0.646 0.222 41.116);
  --chart-2: oklch(0.6 0.118 184.704);
  --chart-3: oklch(0.398 0.07 227.392);
  --chart-4: oklch(0.828 0.189 84.429);
  --chart-5: oklch(0.769 0.188 70.08);
  --sidebar: oklch(0.985 0 0);
  --sidebar-foreground: oklch(0.141 0.005 285.823);
  --sidebar-primary: oklch(0.772 0.161 68);
  --sidebar-primary-foreground: oklch(0.98 0.016 73.684);
  --sidebar-accent: oklch(0.967 0.001 286.375);
  --sidebar-accent-foreground: oklch(0.21 0.006 285.885);
  --sidebar-border: oklch(0.92 0.004 286.32);
  --sidebar-ring: oklch(0.772 0.161 68);
}

.dark {
  --background: oklch(0.141 0.005 285.823);
  --foreground: oklch(0.985 0 0);
  --card: oklch(0.21 0.006 285.885);
  --card-foreground: oklch(0.985 0 0);
  --popover: oklch(0.21 0.006 285.885);
  --popover-foreground: oklch(0.985 0 0);
  --primary: oklch(0.646 0.222 41.116);
  --primary-foreground: oklch(0.98 0.016 73.684);
  --secondary: oklch(0.274 0.006 286.033);
  --secondary-foreground: oklch(0.985 0 0);
  --muted: oklch(0.274 0.006 286.033);
  --muted-foreground: oklch(0.705 0.015 286.067);
  --accent: oklch(0.274 0.006 286.033);
  --accent-foreground: oklch(0.985 0 0);
  --destructive: oklch(0.704 0.191 22.216);
  --border: oklch(1 0 0 / 10%);
  --input: oklch(1 0 0 / 15%);
  --ring: oklch(0.646 0.222 41.116);
  --chart-1: oklch(0.488 0.243 264.376);
  --chart-2: oklch(0.696 0.17 162.48);
  --chart-3: oklch(0.769 0.188 70.08);
  --chart-4: oklch(0.627 0.265 303.9);
  --chart-5: oklch(0.645 0.246 16.439);
  --sidebar: oklch(0.21 0.006 285.885);
  --sidebar-foreground: oklch(0.985 0 0);
  --sidebar-primary: oklch(0.646 0.222 41.116);
  --sidebar-primary-foreground: oklch(0.98 0.016 73.684);
  --sidebar-accent: oklch(0.274 0.006 286.033);
  --sidebar-accent-foreground: oklch(0.985 0 0);
  --sidebar-border: oklch(1 0 0 / 10%);
  --sidebar-ring: oklch(0.646 0.222 41.116);
}

```

### **2. Typography:**
Sử dụng style và font chữ Be Vietnam Pro của google font

[Be Vietnam Pro](https://fonts.google.com/specimen/Be+Vietnam+Pro)
---

## 🖱️ **Basic Interaction:**
Giao diện tuân thủ theo Tailwindcss và component của ShadcnUI

---
## 🎯 **User Experience Guidelines:**

### **1. Navigation:**
- ✅ **Clear Navigation**: Menu rõ ràng, dễ hiểu
- ✅ **Breadcrumbs**: Hiển thị vị trí hiện tại
- ✅ **Active States**: Highlight trang/section hiện tại

### **2. Feedback:**
- ✅ **Loading States**: Spinner/skeleton cho async operations
- ✅ **Confirm Dialog**: Hiện thông báo xác nhận khi xóa hoặc cho 1 hành động không thể hoàn tác
- ✅ **Error Messages**: Thông báo lỗi rõ ràng
- ✅ **Success Messages**: Xác nhận thao tác thành công

### **3. Performance:**
- ✅ **Fast Loading**: Tải nhanh trên mọi thiết bị
- ✅ **Smooth Animations**: Animation mượt mà, không lag
- ✅ **Optimized Images**: Hình ảnh được tối ưu

---
