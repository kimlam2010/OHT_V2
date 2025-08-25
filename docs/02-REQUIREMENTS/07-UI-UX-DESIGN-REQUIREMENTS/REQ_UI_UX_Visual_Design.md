# 🎨 REQ_UI_UX_Visual_Design - Thiết kế hình ảnh

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Product Managers

---

## 🎯 **Mục tiêu thiết kế hình ảnh:**

### **Concept chính:**
- **Phong cách:** Industrial/Technical với màu cam làm chủ đạo
- **Mood:** Professional, Reliable, Safety-focused
- **Inspiration:** Tesla Dashboard, SpaceX Control Center, Industrial SCADA

### **Nguyên tắc thiết kế:**
1. **Clarity:** Rõ ràng, dễ đọc trong môi trường ngoài trời
2. **Safety:** Màu sắc và contrast phù hợp với cảnh báo
3. **Efficiency:** Tối ưu cho thao tác nhanh và chính xác
4. **Consistency:** Nhất quán trong toàn bộ hệ thống

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| Color Palette | Định nghĩa bảng màu | Designers, Developers | Design System | ✅ Hoàn thành |
| Typography | Hệ thống font chữ | Designers, Developers | Design System | ✅ Hoàn thành |
| Iconography | Hệ thống icon | Designers, Developers | Component Library | ✅ Hoàn thành |
| Layout Grid | Hệ thống layout | Designers, Developers | Information Architecture | ✅ Hoàn thành |

---

## 🎨 **1. COLOR PALETTE - Bảng màu**

### **Primary Colors:**
```css
/* Orange - Màu chủ đạo */
--primary-orange: #FF6B35;        /* Main brand color */
--primary-orange-light: #FF8A5C;  /* Hover states */
--primary-orange-dark: #E55A2B;   /* Active states */

/* Industrial Grays */
--industrial-gray-100: #F8F9FA;   /* Background light */
--industrial-gray-200: #E9ECEF;   /* Background medium */
--industrial-gray-300: #DEE2E6;   /* Borders */
--industrial-gray-400: #CED4DA;   /* Disabled states */
--industrial-gray-500: #ADB5BD;   /* Text secondary */
--industrial-gray-600: #6C757D;   /* Text primary */
--industrial-gray-700: #495057;   /* Headers */
--industrial-gray-800: #343A40;   /* Dark backgrounds */
--industrial-gray-900: #212529;   /* Darkest backgrounds */
```

### **Semantic Colors:**
```css
/* Success States */
--success-green: #28A745;         /* Normal operation */
--success-green-light: #34CE57;   /* Hover states */

/* Warning States */
--warning-yellow: #FFC107;        /* Caution alerts */
--warning-yellow-light: #FFD54F;  /* Hover states */

/* Error States */
--error-red: #DC3545;             /* Critical errors */
--error-red-light: #E74C3C;       /* Hover states */

/* Info States */
--info-blue: #17A2B8;             /* Information */
--info-blue-light: #20C997;       /* Hover states */
```

### **Background Colors:**
```css
/* Light Theme (Primary) */
--bg-primary: #FFFFFF;            /* Main background */
--bg-secondary: #F8F9FA;          /* Secondary background */
--bg-tertiary: #E9ECEF;           /* Tertiary background */

/* Dark Theme (Alternative) */
--bg-dark-primary: #212529;       /* Dark main background */
--bg-dark-secondary: #343A40;     /* Dark secondary */
--bg-dark-tertiary: #495057;      /* Dark tertiary */
```

---

## 📝 **2. TYPOGRAPHY - Hệ thống font chữ**

### **Font Family:**
```css
/* Primary Font - Industrial/Technical */
--font-primary: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;

/* Monospace Font - Data/Code */
--font-mono: 'JetBrains Mono', 'Fira Code', 'Source Code Pro', monospace;

/* Display Font - Headers */
--font-display: 'Space Grotesk', 'Inter', sans-serif;
```

### **Font Sizes:**
```css
/* Display Sizes */
--text-display-1: 3.5rem;         /* 56px - Main headers */
--text-display-2: 3rem;           /* 48px - Section headers */
--text-display-3: 2.5rem;         /* 40px - Subsection headers */

/* Heading Sizes */
--text-h1: 2rem;                  /* 32px - Page titles */
--text-h2: 1.75rem;               /* 28px - Section titles */
--text-h3: 1.5rem;                /* 24px - Subsection titles */
--text-h4: 1.25rem;               /* 20px - Card titles */
--text-h5: 1.125rem;              /* 18px - Small titles */
--text-h6: 1rem;                  /* 16px - Micro titles */

/* Body Sizes */
--text-body-large: 1.125rem;      /* 18px - Large body text */
--text-body: 1rem;                /* 16px - Standard body text */
--text-body-small: 0.875rem;      /* 14px - Small body text */
--text-caption: 0.75rem;          /* 12px - Captions/labels */
```

### **Font Weights:**
```css
--font-light: 300;                /* Light weight */
--font-normal: 400;               /* Normal weight */
--font-medium: 500;               /* Medium weight */
--font-semibold: 600;             /* Semibold weight */
--font-bold: 700;                 /* Bold weight */
--font-extrabold: 800;            /* Extrabold weight */
```

---

## 🔧 **3. ICONOGRAPHY - Hệ thống icon**

### **Icon Style:**
- **Style:** Outlined với fill cho active states
- **Weight:** 2px stroke width
- **Size:** 24px base size, scalable
- **Color:** Inherit từ parent element

### **Icon Categories:**
```css
/* Navigation Icons */
--icon-home: "🏠";
--icon-dashboard: "📊";
--icon-settings: "⚙️";
--icon-map: "🗺️";
--icon-control: "🎮";

/* Status Icons */
--icon-status-online: "🟢";
--icon-status-offline: "🔴";
--icon-status-warning: "🟡";
--icon-status-error: "🔴";

/* Control Icons */
--icon-play: "▶️";
--icon-pause: "⏸️";
--icon-stop: "⏹️";
--icon-reset: "🔄";
--icon-emergency: "🚨";

/* Data Icons */
--icon-chart: "📈";
--icon-table: "📋";
--icon-export: "📤";
--icon-import: "📥";
--icon-download: "⬇️";
```

### **Icon Sizes:**
```css
--icon-xs: 16px;                  /* Extra small */
--icon-sm: 20px;                  /* Small */
--icon-md: 24px;                  /* Medium (default) */
--icon-lg: 32px;                  /* Large */
--icon-xl: 48px;                  /* Extra large */
```

---

## 📐 **4. LAYOUT GRID - Hệ thống layout**

### **Grid System:**
```css
/* Base Grid */
--grid-columns: 12;               /* 12-column grid */
--grid-gutter: 1rem;              /* 16px gutter */
--grid-margin: 1.5rem;            /* 24px margin */

/* Breakpoints */
--breakpoint-xs: 0px;             /* Extra small */
--breakpoint-sm: 576px;           /* Small */
--breakpoint-md: 768px;           /* Medium */
--breakpoint-lg: 992px;           /* Large */
--breakpoint-xl: 1200px;          /* Extra large */
--breakpoint-xxl: 1400px;         /* Extra extra large */
```

### **Spacing System:**
```css
/* Spacing Scale */
--space-xs: 0.25rem;              /* 4px */
--space-sm: 0.5rem;               /* 8px */
--space-md: 1rem;                 /* 16px */
--space-lg: 1.5rem;               /* 24px */
--space-xl: 2rem;                 /* 32px */
--space-2xl: 3rem;                /* 48px */
--space-3xl: 4rem;                /* 64px */
```

### **Container Sizes:**
```css
/* Container Max Widths */
--container-sm: 540px;            /* Small container */
--container-md: 720px;            /* Medium container */
--container-lg: 960px;            /* Large container */
--container-xl: 1140px;           /* Extra large container */
--container-xxl: 1320px;          /* Extra extra large container */
```

---

## 🎨 **5. COMPONENT STYLES - Kiểu dáng component**

### **Buttons:**
```css
/* Primary Button */
.btn-primary {
  background: var(--primary-orange);
  color: white;
  border: none;
  padding: 0.75rem 1.5rem;
  border-radius: 0.5rem;
  font-weight: var(--font-semibold);
  transition: all 0.2s ease;
}

.btn-primary:hover {
  background: var(--primary-orange-light);
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.3);
}

/* Secondary Button */
.btn-secondary {
  background: transparent;
  color: var(--primary-orange);
  border: 2px solid var(--primary-orange);
  padding: 0.75rem 1.5rem;
  border-radius: 0.5rem;
  font-weight: var(--font-semibold);
  transition: all 0.2s ease;
}

.btn-secondary:hover {
  background: var(--primary-orange);
  color: white;
}
```

### **Cards:**
```css
/* Standard Card */
.card {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
  transition: all 0.2s ease;
}

.card:hover {
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.15);
  transform: translateY(-2px);
}

/* Status Card */
.card-status {
  border-left: 4px solid var(--primary-orange);
  background: linear-gradient(135deg, var(--bg-primary) 0%, var(--bg-secondary) 100%);
}
```

### **Alerts:**
```css
/* Success Alert */
.alert-success {
  background: rgba(40, 167, 69, 0.1);
  border: 1px solid var(--success-green);
  color: var(--success-green);
  padding: var(--space-md);
  border-radius: 0.5rem;
  border-left: 4px solid var(--success-green);
}

/* Warning Alert */
.alert-warning {
  background: rgba(255, 193, 7, 0.1);
  border: 1px solid var(--warning-yellow);
  color: var(--warning-yellow);
  padding: var(--space-md);
  border-radius: 0.5rem;
  border-left: 4px solid var(--warning-yellow);
}

/* Error Alert */
.alert-error {
  background: rgba(220, 53, 69, 0.1);
  border: 1px solid var(--error-red);
  color: var(--error-red);
  padding: var(--space-md);
  border-radius: 0.5rem;
  border-left: 4px solid var(--error-red);
}
```

---

## 📱 **6. RESPONSIVE DESIGN - Thiết kế responsive**

### **Mobile First Approach:**
```css
/* Base styles (mobile) */
.container {
  padding: var(--space-md);
  max-width: 100%;
}

/* Tablet (768px+) */
@media (min-width: 768px) {
  .container {
    padding: var(--space-lg);
    max-width: var(--container-md);
  }
}

/* Desktop (992px+) */
@media (min-width: 992px) {
  .container {
    padding: var(--space-xl);
    max-width: var(--container-lg);
  }
}

/* Large Desktop (1200px+) */
@media (min-width: 1200px) {
  .container {
    max-width: var(--container-xl);
  }
}
```

### **Touch-Friendly Design:**
```css
/* Minimum touch target size */
.btn, .nav-item, .control-button {
  min-height: 44px;
  min-width: 44px;
  padding: var(--space-sm) var(--space-md);
}

/* Touch feedback */
.btn:active, .nav-item:active {
  transform: scale(0.95);
  transition: transform 0.1s ease;
}
```

---

## 🎯 **7. ACCESSIBILITY - Khả năng tiếp cận**

### **Color Contrast:**
- **Normal text:** Minimum 4.5:1 contrast ratio
- **Large text:** Minimum 3:1 contrast ratio
- **UI components:** Minimum 3:1 contrast ratio

### **Focus States:**
```css
/* Focus indicator */
.btn:focus, .nav-item:focus {
  outline: 2px solid var(--primary-orange);
  outline-offset: 2px;
  box-shadow: 0 0 0 4px rgba(255, 107, 53, 0.2);
}

/* Skip link */
.skip-link {
  position: absolute;
  top: -40px;
  left: 6px;
  background: var(--primary-orange);
  color: white;
  padding: 8px;
  text-decoration: none;
  border-radius: 4px;
  z-index: 1000;
}

.skip-link:focus {
  top: 6px;
}
```

---

## 📚 **Tham chiếu:**

- [Design System](./REQ_UI_UX_Design_System.md) - Hệ thống thiết kế tổng thể
- [Information Architecture](./REQ_UI_UX_Information_Architecture.md) - Kiến trúc thông tin
- [Dashboard Design](./REQ_UI_UX_Dashboard_Design.md) - Thiết kế dashboard
- [Component Library](../05-FRONTEND-REQUIREMENTS/REQ_FE_Component_Library.md) - Thư viện component

---

**Changelog v1.0:**
- ✅ Created comprehensive visual design system
- ✅ Defined industrial orange color palette
- ✅ Established typography hierarchy
- ✅ Created iconography system
- ✅ Defined layout grid and spacing
- ✅ Added component styles and responsive design
- ✅ Included accessibility guidelines
