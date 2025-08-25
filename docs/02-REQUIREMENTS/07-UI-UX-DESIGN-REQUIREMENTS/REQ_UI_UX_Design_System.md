# 🎨 REQ_UI_UX_Design_System - Hệ thống thiết kế

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ⏳ Đang tạo  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Product Managers

---

## 🎯 **Mục tiêu Design System:**

### **Mục đích chính:**
- Tạo hệ thống thiết kế nhất quán cho OHT-50
- Định nghĩa design tokens và components
- Đảm bảo scalability và maintainability
- Cung cấp guidelines cho development team

### **Nguyên tắc thiết kế:**
1. **Clarity:** Rõ ràng, dễ hiểu
2. **Efficiency:** Hiệu quả, tiết kiệm thời gian
3. **Safety:** An toàn, đáng tin cậy
4. **Accessibility:** Khả năng tiếp cận cho mọi người
5. **Consistency:** Nhất quán trong toàn bộ hệ thống

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| **Design Tokens** | Định nghĩa màu sắc, typography, spacing | UI/UX Designers, FE Devs | Visual consistency | ⏳ Đang tạo |
| **Component Library** | Thư viện components chuẩn | UI/UX Designers, FE Devs | Code reusability | ⏳ Đang tạo |
| **Typography System** | Hệ thống font chữ | UI/UX Designers | Text readability | ⏳ Đang tạo |
| **Color System** | Hệ thống màu sắc | UI/UX Designers | Visual hierarchy | ⏳ Đang tạo |
| **Spacing System** | Hệ thống khoảng cách | UI/UX Designers | Layout consistency | ⏳ Đang tạo |
| **Icon System** | Hệ thống icon | UI/UX Designers | Visual communication | ⏳ Đang tạo |

---

## 🎨 **DESIGN TOKENS:**

### **Color Tokens:**
```css
/* Primary Colors */
--color-primary-50: #eff6ff;
--color-primary-100: #dbeafe;
--color-primary-500: #3b82f6;
--color-primary-600: #2563eb;
--color-primary-900: #1e3a8a;

/* Secondary Colors */
--color-secondary-50: #f8fafc;
--color-secondary-100: #f1f5f9;
--color-secondary-500: #64748b;
--color-secondary-600: #475569;
--color-secondary-900: #0f172a;

/* Status Colors */
--color-success-50: #f0fdf4;
--color-success-500: #22c55e;
--color-success-600: #16a34a;

--color-warning-50: #fffbeb;
--color-warning-500: #f59e0b;
--color-warning-600: #d97706;

--color-error-50: #fef2f2;
--color-error-500: #ef4444;
--color-error-600: #dc2626;

--color-info-50: #eff6ff;
--color-info-500: #3b82f6;
--color-info-600: #2563eb;

/* Neutral Colors */
--color-white: #ffffff;
--color-black: #000000;
--color-gray-50: #f9fafb;
--color-gray-100: #f3f4f6;
--color-gray-200: #e5e7eb;
--color-gray-300: #d1d5db;
--color-gray-400: #9ca3af;
--color-gray-500: #6b7280;
--color-gray-600: #4b5563;
--color-gray-700: #374151;
--color-gray-800: #1f2937;
--color-gray-900: #111827;
```

### **Typography Tokens:**
```css
/* Font Families */
--font-family-sans: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
--font-family-mono: 'JetBrains Mono', 'Fira Code', 'Source Code Pro', monospace;

/* Font Sizes */
--font-size-xs: 0.75rem;    /* 12px */
--font-size-sm: 0.875rem;   /* 14px */
--font-size-base: 1rem;     /* 16px */
--font-size-lg: 1.125rem;   /* 18px */
--font-size-xl: 1.25rem;    /* 20px */
--font-size-2xl: 1.5rem;    /* 24px */
--font-size-3xl: 1.875rem;  /* 30px */
--font-size-4xl: 2.25rem;   /* 36px */

/* Font Weights */
--font-weight-light: 300;
--font-weight-normal: 400;
--font-weight-medium: 500;
--font-weight-semibold: 600;
--font-weight-bold: 700;

/* Line Heights */
--line-height-tight: 1.25;
--line-height-normal: 1.5;
--line-height-relaxed: 1.75;
```

### **Spacing Tokens:**
```css
/* Spacing Scale */
--spacing-0: 0;
--spacing-1: 0.25rem;   /* 4px */
--spacing-2: 0.5rem;    /* 8px */
--spacing-3: 0.75rem;   /* 12px */
--spacing-4: 1rem;      /* 16px */
--spacing-5: 1.25rem;   /* 20px */
--spacing-6: 1.5rem;    /* 24px */
--spacing-8: 2rem;      /* 32px */
--spacing-10: 2.5rem;   /* 40px */
--spacing-12: 3rem;     /* 48px */
--spacing-16: 4rem;     /* 64px */
--spacing-20: 5rem;     /* 80px */
--spacing-24: 6rem;     /* 96px */
```

### **Border Radius Tokens:**
```css
/* Border Radius */
--radius-none: 0;
--radius-sm: 0.125rem;   /* 2px */
--radius-base: 0.25rem;  /* 4px */
--radius-md: 0.375rem;   /* 6px */
--radius-lg: 0.5rem;     /* 8px */
--radius-xl: 0.75rem;    /* 12px */
--radius-2xl: 1rem;      /* 16px */
--radius-full: 9999px;
```

### **Shadow Tokens:**
```css
/* Shadows */
--shadow-sm: 0 1px 2px 0 rgb(0 0 0 / 0.05);
--shadow-base: 0 1px 3px 0 rgb(0 0 0 / 0.1), 0 1px 2px -1px rgb(0 0 0 / 0.1);
--shadow-md: 0 4px 6px -1px rgb(0 0 0 / 0.1), 0 2px 4px -2px rgb(0 0 0 / 0.1);
--shadow-lg: 0 10px 15px -3px rgb(0 0 0 / 0.1), 0 4px 6px -4px rgb(0 0 0 / 0.1);
--shadow-xl: 0 20px 25px -5px rgb(0 0 0 / 0.1), 0 8px 10px -6px rgb(0 0 0 / 0.1);
```

---

## 🧩 **COMPONENT LIBRARY:**

### **1. Button Components:**
```typescript
// Primary Button
interface ButtonProps {
  variant: 'primary' | 'secondary' | 'outline' | 'ghost' | 'danger';
  size: 'sm' | 'md' | 'lg';
  disabled?: boolean;
  loading?: boolean;
  icon?: React.ReactNode;
  children: React.ReactNode;
  onClick?: () => void;
}

// Button Variants
const buttonVariants = {
  primary: 'bg-primary-600 text-white hover:bg-primary-700',
  secondary: 'bg-secondary-600 text-white hover:bg-secondary-700',
  outline: 'border border-gray-300 text-gray-700 hover:bg-gray-50',
  ghost: 'text-gray-700 hover:bg-gray-100',
  danger: 'bg-error-600 text-white hover:bg-error-700'
};

// Button Sizes
const buttonSizes = {
  sm: 'px-3 py-1.5 text-sm',
  md: 'px-4 py-2 text-base',
  lg: 'px-6 py-3 text-lg'
};
```

### **2. Form Components:**
```typescript
// Input Component
interface InputProps {
  type: 'text' | 'email' | 'password' | 'number' | 'tel';
  label?: string;
  placeholder?: string;
  error?: string;
  disabled?: boolean;
  required?: boolean;
  value: string;
  onChange: (value: string) => void;
}

// Select Component
interface SelectProps {
  label?: string;
  options: Array<{ value: string; label: string }>;
  value: string;
  onChange: (value: string) => void;
  disabled?: boolean;
  error?: string;
}

// Checkbox Component
interface CheckboxProps {
  label: string;
  checked: boolean;
  onChange: (checked: boolean) => void;
  disabled?: boolean;
}
```

### **3. Data Display Components:**
```typescript
// Status Badge
interface StatusBadgeProps {
  status: 'success' | 'warning' | 'error' | 'info' | 'neutral';
  text: string;
  size?: 'sm' | 'md' | 'lg';
}

// Data Table
interface DataTableProps {
  columns: Array<{
    key: string;
    label: string;
    sortable?: boolean;
    width?: string;
  }>;
  data: Array<Record<string, any>>;
  pagination?: {
    current: number;
    total: number;
    pageSize: number;
  };
  onSort?: (key: string, direction: 'asc' | 'desc') => void;
  onPageChange?: (page: number) => void;
}

// Chart Component
interface ChartProps {
  type: 'line' | 'bar' | 'pie' | 'area';
  data: Array<{ label: string; value: number }>;
  title?: string;
  height?: number;
  colors?: string[];
}
```

### **4. Navigation Components:**
```typescript
// Navigation Menu
interface NavMenuProps {
  items: Array<{
    key: string;
    label: string;
    icon?: React.ReactNode;
    href?: string;
    children?: Array<{ key: string; label: string; href: string }>;
  }>;
  activeKey?: string;
  onSelect?: (key: string) => void;
}

// Breadcrumb
interface BreadcrumbProps {
  items: Array<{
    label: string;
    href?: string;
  }>;
  separator?: React.ReactNode;
}

// Pagination
interface PaginationProps {
  current: number;
  total: number;
  pageSize: number;
  onChange: (page: number) => void;
  showSizeChanger?: boolean;
  onPageSizeChange?: (size: number) => void;
}
```

### **5. Feedback Components:**
```typescript
// Alert Component
interface AlertProps {
  type: 'success' | 'warning' | 'error' | 'info';
  title?: string;
  message: string;
  closable?: boolean;
  onClose?: () => void;
}

// Modal Component
interface ModalProps {
  title: string;
  visible: boolean;
  onClose: () => void;
  children: React.ReactNode;
  footer?: React.ReactNode;
  width?: number | string;
}

// Toast Component
interface ToastProps {
  type: 'success' | 'warning' | 'error' | 'info';
  message: string;
  duration?: number;
  onClose?: () => void;
}
```

---

## 📱 **RESPONSIVE DESIGN:**

### **Breakpoints:**
```css
/* Mobile First Approach */
--breakpoint-sm: 640px;   /* Small devices */
--breakpoint-md: 768px;   /* Medium devices */
--breakpoint-lg: 1024px;  /* Large devices */
--breakpoint-xl: 1280px;  /* Extra large devices */
--breakpoint-2xl: 1536px; /* 2X large devices */

/* Media Queries */
@media (min-width: 640px) { /* sm */ }
@media (min-width: 768px) { /* md */ }
@media (min-width: 1024px) { /* lg */ }
@media (min-width: 1280px) { /* xl */ }
@media (min-width: 1536px) { /* 2xl */ }
```

### **Grid System:**
```css
/* CSS Grid */
.grid {
  display: grid;
  gap: var(--spacing-4);
}

.grid-cols-1 { grid-template-columns: repeat(1, minmax(0, 1fr)); }
.grid-cols-2 { grid-template-columns: repeat(2, minmax(0, 1fr)); }
.grid-cols-3 { grid-template-columns: repeat(3, minmax(0, 1fr)); }
.grid-cols-4 { grid-template-columns: repeat(4, minmax(0, 1fr)); }
.grid-cols-6 { grid-template-columns: repeat(6, minmax(0, 1fr)); }
.grid-cols-12 { grid-template-columns: repeat(12, minmax(0, 1fr)); }

/* Responsive Grid */
@media (min-width: 768px) {
  .md\:grid-cols-2 { grid-template-columns: repeat(2, minmax(0, 1fr)); }
  .md\:grid-cols-3 { grid-template-columns: repeat(3, minmax(0, 1fr)); }
  .md\:grid-cols-4 { grid-template-columns: repeat(4, minmax(0, 1fr)); }
}
```

---

## ♿ **ACCESSIBILITY GUIDELINES:**

### **WCAG 2.1 AA Compliance:**
```css
/* Focus States */
.focus-visible {
  outline: 2px solid var(--color-primary-500);
  outline-offset: 2px;
}

/* High Contrast Mode */
@media (prefers-contrast: high) {
  :root {
    --color-primary-600: #1e40af;
    --color-error-600: #b91c1c;
    --color-success-600: #059669;
  }
}

/* Reduced Motion */
@media (prefers-reduced-motion: reduce) {
  * {
    animation-duration: 0.01ms !important;
    animation-iteration-count: 1 !important;
    transition-duration: 0.01ms !important;
  }
}
```

### **Semantic HTML:**
```html
<!-- Proper heading hierarchy -->
<h1>Main Title</h1>
<h2>Section Title</h2>
<h3>Subsection Title</h3>

<!-- Proper form labels -->
<label for="username">Username</label>
<input id="username" type="text" aria-describedby="username-help" />

<!-- ARIA labels -->
<button aria-label="Close modal">×</button>
<div role="alert" aria-live="polite">Success message</div>
```

---

## 🎨 **VISUAL HIERARCHY:**

### **Typography Scale:**
```css
/* Heading Styles */
.heading-1 {
  font-size: var(--font-size-4xl);
  font-weight: var(--font-weight-bold);
  line-height: var(--line-height-tight);
  color: var(--color-gray-900);
}

.heading-2 {
  font-size: var(--font-size-3xl);
  font-weight: var(--font-weight-semibold);
  line-height: var(--line-height-tight);
  color: var(--color-gray-900);
}

.heading-3 {
  font-size: var(--font-size-2xl);
  font-weight: var(--font-weight-semibold);
  line-height: var(--line-height-tight);
  color: var(--color-gray-800);
}

/* Body Text */
.body-large {
  font-size: var(--font-size-lg);
  font-weight: var(--font-weight-normal);
  line-height: var(--line-height-normal);
  color: var(--color-gray-700);
}

.body-base {
  font-size: var(--font-size-base);
  font-weight: var(--font-weight-normal);
  line-height: var(--line-height-normal);
  color: var(--color-gray-700);
}

.body-small {
  font-size: var(--font-size-sm);
  font-weight: var(--font-weight-normal);
  line-height: var(--line-height-normal);
  color: var(--color-gray-600);
}
```

### **Color Usage:**
```css
/* Text Colors */
.text-primary { color: var(--color-primary-600); }
.text-secondary { color: var(--color-secondary-600); }
.text-success { color: var(--color-success-600); }
.text-warning { color: var(--color-warning-600); }
.text-error { color: var(--color-error-600); }
.text-info { color: var(--color-info-600); }

/* Background Colors */
.bg-primary { background-color: var(--color-primary-50); }
.bg-secondary { background-color: var(--color-secondary-50); }
.bg-success { background-color: var(--color-success-50); }
.bg-warning { background-color: var(--color-warning-50); }
.bg-error { background-color: var(--color-error-50); }
.bg-info { background-color: var(--color-info-50); }
```

---

## 📋 **IMPLEMENTATION GUIDELINES:**

### **1. CSS Custom Properties:**
```css
/* Use CSS custom properties for design tokens */
:root {
  /* Import all design tokens here */
}

/* Component-specific tokens */
.button {
  --button-padding-x: var(--spacing-4);
  --button-padding-y: var(--spacing-2);
  --button-border-radius: var(--radius-md);
  --button-font-size: var(--font-size-base);
  --button-font-weight: var(--font-weight-medium);
}
```

### **2. Component Structure:**
```typescript
// Component file structure
components/
├── Button/
│   ├── Button.tsx
│   ├── Button.styles.css
│   ├── Button.test.tsx
│   └── index.ts
├── Input/
│   ├── Input.tsx
│   ├── Input.styles.css
│   ├── Input.test.tsx
│   └── index.ts
└── index.ts
```

### **3. Storybook Integration:**
```typescript
// Storybook stories for components
export default {
  title: 'Components/Button',
  component: Button,
  parameters: {
    docs: {
      description: {
        component: 'A versatile button component with multiple variants.'
      }
    }
  }
};

export const Primary = {
  args: {
    variant: 'primary',
    children: 'Primary Button'
  }
};
```

---

## 📚 **DOCUMENTATION:**

### **Design Token Documentation:**
- Color palette với hex codes
- Typography scale với examples
- Spacing scale với visual examples
- Component usage guidelines
- Accessibility requirements

### **Component Documentation:**
- Props interface
- Usage examples
- Accessibility considerations
- Best practices
- Common patterns

---

**Changelog:**
- v1.0 (2025-01-28): Initial design system framework
- Added design tokens
- Added component library structure
- Added accessibility guidelines
- Added implementation guidelines

**Next Steps:**
- Create component implementations
- Set up Storybook
- Implement design tokens in CSS
- Create component documentation
