# 🎨 **OHT-50 DESIGN SYSTEM DOCUMENTATION**

**Phiên bản:** v2.0  
**Ngày cập nhật:** 2025-02-11  
**Mục tiêu:** Tài liệu hoàn chỉnh về design system cho OHT-50 Industrial Theme

---

## 📋 **MỤC LỤC**

1. [Design Philosophy](#design-philosophy)
2. [Color System](#color-system)
3. [Typography](#typography)
4. [Spacing System](#spacing-system)
5. [Component Library](#component-library)
6. [Layout System](#layout-system)
7. [Accessibility Guidelines](#accessibility-guidelines)
8. [Safety-First Design](#safety-first-design)
9. [Implementation Guidelines](#implementation-guidelines)
10. [File Structure](#file-structure)

---

## 🎯 **DESIGN PHILOSOPHY**

### **Core Principles:**
- **Safety-First:** Emergency controls và safety indicators ưu tiên cao nhất
- **Industrial Theme:** Orange color scheme (#FF6B35) cho industrial environment
- **Accessibility:** WCAG 2.1 AA compliance bắt buộc
- **Responsive Design:** Mobile-first approach với industrial touch targets
- **Consistency:** Unified design tokens across all components

### **Design Goals:**
- **Clarity:** Clear visual hierarchy và information architecture
- **Efficiency:** Fast interaction và minimal cognitive load
- **Reliability:** Robust design cho industrial environment
- **Scalability:** Design system có thể mở rộng cho future features

---

## 🎨 **COLOR SYSTEM**

### **Primary Colors:**
```css
--primary-orange: #FF6B35;        /* Main brand color */
--primary-orange-light: #FF8A5C;  /* Hover states */
--primary-orange-dark: #E55A2B;   /* Active states */
```

### **Industrial Gray Scale:**
```css
--industrial-gray-50: #F9FAFB;    /* Background lightest */
--industrial-gray-100: #F3F4F6;   /* Background light */
--industrial-gray-200: #E5E7EB;   /* Border light */
--industrial-gray-300: #D1D5DB;   /* Border medium */
--industrial-gray-400: #9CA3AF;   /* Text muted */
--industrial-gray-500: #6B7280;   /* Text secondary */
--industrial-gray-600: #4B5563;   /* Text primary */
--industrial-gray-700: #374151;   /* Headings */
--industrial-gray-800: #1F2937;   /* Headings strong */
--industrial-gray-900: #111827;   /* Text darkest */
```

### **Safety Colors:**
```css
--safety-red: #DC2626;            /* Emergency, Stop */
--safety-orange: #EA580C;         /* Warning, Caution */
--safety-yellow: #CA8A04;         /* Attention, Alert */
--safety-green: #16A34A;          /* Success, Safe */
```

### **Semantic Colors:**
```css
--success: var(--safety-green);   /* Success states */
--warning: var(--safety-yellow);  /* Warning states */
--error: var(--safety-red);       /* Error states */
--info: #3B82F6;                  /* Information states */
```

---

## 📝 **TYPOGRAPHY**

### **Font Families:**
```css
--font-sans: 'Inter', -apple-system, BlinkMacSystemFont, sans-serif;
--font-mono: 'JetBrains Mono', 'Fira Code', monospace;
--font-display: 'Space Grotesk', 'Inter', sans-serif;
```

### **Font Weights:**
```css
--font-light: 300;
--font-normal: 400;
--font-medium: 500;
--font-semibold: 600;
--font-bold: 700;
--font-extrabold: 800;
```

### **Font Sizes:**
```css
--text-xs: 0.75rem;      /* 12px */
--text-sm: 0.875rem;     /* 14px */
--text-base: 1rem;       /* 16px */
--text-lg: 1.125rem;     /* 18px */
--text-xl: 1.25rem;      /* 20px */
--text-2xl: 1.5rem;      /* 24px */
--text-3xl: 1.875rem;    /* 30px */
--text-4xl: 2.25rem;     /* 36px */
--text-5xl: 3rem;        /* 48px */
```

### **Line Heights:**
```css
--leading-tight: 1.25;
--leading-snug: 1.375;
--leading-normal: 1.5;
--leading-relaxed: 1.625;
--leading-loose: 2;
```

---

## 📏 **SPACING SYSTEM**

### **Spacing Scale:**
```css
--space-xs: 0.25rem;     /* 4px */
--space-sm: 0.5rem;      /* 8px */
--space-md: 1rem;        /* 16px */
--space-lg: 1.5rem;      /* 24px */
--space-xl: 2rem;        /* 32px */
--space-2xl: 3rem;       /* 48px */
```

### **Usage Guidelines:**
- **XS (4px):** Micro spacing, icon padding
- **SM (8px):** Small gaps, button padding
- **MD (16px):** Standard spacing, component margins
- **LG (24px):** Section spacing, card padding
- **XL (32px):** Large sections, page margins
- **2XL (48px):** Major sections, hero spacing

---

## 🧩 **COMPONENT LIBRARY**

### **Button Components:**
```html
<!-- Primary Button -->
<button class="btn btn-primary">Primary Action</button>

<!-- Secondary Button -->
<button class="btn btn-secondary">Secondary Action</button>

<!-- Danger Button -->
<button class="btn btn-danger">Delete</button>

<!-- Emergency Button -->
<button class="btn btn-emergency">
  <i data-lucide="alert-triangle"></i>
  E-STOP
</button>
```

### **Card Components:**
```html
<!-- Basic Card -->
<div class="card">
  <div class="card-header">
    <h3 class="card-title">Card Title</h3>
  </div>
  <div class="card-body">
    <p>Card content goes here</p>
  </div>
  <div class="card-footer">
    <button class="btn btn-primary">Action</button>
  </div>
</div>
```

### **Form Components:**
```html
<!-- Form Group -->
<div class="form-group">
  <label for="input1" class="form-label">Label</label>
  <input type="text" id="input1" class="form-input" placeholder="Enter text...">
  <div class="form-help">Help text</div>
  <div class="form-error">Error message</div>
</div>
```

### **Alert Components:**
```html
<!-- Success Alert -->
<div class="alert alert-success">
  <i data-lucide="check-circle"></i>
  <span>Operation completed successfully</span>
</div>

<!-- Warning Alert -->
<div class="alert alert-warning">
  <i data-lucide="alert-triangle"></i>
  <span>Warning message</span>
</div>

<!-- Error Alert -->
<div class="alert alert-error">
  <i data-lucide="x-circle"></i>
  <span>Error message</span>
</div>
```

---

## 📐 **LAYOUT SYSTEM**

### **Grid System:**
```css
/* 12-column grid system */
.grid {
  display: grid;
  grid-template-columns: repeat(12, 1fr);
  gap: var(--space-md);
}

/* Responsive breakpoints */
@media (min-width: 640px) { /* sm */ }
@media (min-width: 768px) { /* md */ }
@media (min-width: 1024px) { /* lg */ }
@media (min-width: 1280px) { /* xl */ }
@media (min-width: 1536px) { /* 2xl */ }
```

### **Page Structure:**
```html
<div class="page-container">
  <header class="page-header">
    <!-- Navigation -->
  </header>
  
  <main class="page-main">
    <div class="page-content">
      <!-- Main content -->
    </div>
  </main>
  
  <footer class="page-footer">
    <!-- Footer content -->
  </footer>
</div>
```

---

## ♿ **ACCESSIBILITY GUIDELINES**

### **WCAG 2.1 AA Compliance:**
- **Color Contrast:** Minimum 4.5:1 for normal text, 3:1 for large text
- **Focus Indicators:** Visible focus indicators for all interactive elements
- **Keyboard Navigation:** All functionality accessible via keyboard
- **Screen Reader Support:** Proper ARIA labels và semantic HTML
- **Touch Targets:** Minimum 44px touch targets for mobile

### **Implementation:**
```css
/* Focus indicators */
.btn:focus {
  outline: 2px solid var(--primary-orange);
  outline-offset: 2px;
}

/* High contrast mode */
@media (prefers-contrast: high) {
  .btn {
    border: 2px solid currentColor;
  }
}
```

---

## 🚨 **SAFETY-FIRST DESIGN**

### **Emergency Components:**
- **E-Stop Buttons:** Large, red, prominent placement
- **Alert Banners:** High visibility với safety colors
- **Status Indicators:** Clear visual feedback cho system states
- **Confirmation Dialogs:** Required cho dangerous operations

### **Safety Color Usage:**
- **Red (#DC2626):** Emergency stop, critical errors
- **Orange (#EA580C):** Warnings, system alerts
- **Yellow (#CA8A04):** Attention, maintenance required
- **Green (#16A34A):** Safe, normal operation

### **Emergency Response:**
- **Response Time:** < 100ms cho emergency controls
- **Visual Feedback:** Immediate visual confirmation
- **Audio Alerts:** Configurable audio notifications
- **Haptic Feedback:** Vibration cho mobile devices

---

## 🛠️ **IMPLEMENTATION GUIDELINES**

### **CSS Variables Usage:**
```css
/* Always use CSS variables */
.button {
  background-color: var(--primary-orange);
  color: var(--industrial-gray-50);
  padding: var(--space-sm) var(--space-md);
  border-radius: var(--radius-md);
}
```

### **Component Structure:**
```html
<!-- Consistent component structure -->
<div class="component-name">
  <div class="component-header">
    <!-- Header content -->
  </div>
  <div class="component-body">
    <!-- Body content -->
  </div>
  <div class="component-footer">
    <!-- Footer content -->
  </div>
</div>
```

### **Responsive Design:**
```css
/* Mobile-first approach */
.component {
  /* Mobile styles */
}

@media (min-width: 768px) {
  .component {
    /* Tablet styles */
  }
}

@media (min-width: 1024px) {
  .component {
    /* Desktop styles */
  }
}
```

---

## 📁 **FILE STRUCTURE**

### **Design System Files:**
```
.superdesign/
├── design_iterations/
│   └── foundation/
│       ├── design_system.html              # Interactive design system
│       ├── color_palette_demo.html         # Color system showcase
│       ├── typography_guide.html           # Typography examples
│       ├── spacing_system.html             # Spacing scale demo
│       ├── component_library.html          # Component showcase
│       ├── button_variants.html            # Button components
│       ├── card_system.html                # Card components
│       ├── form_components.html            # Form components
│       └── navigation_demo.html            # Navigation components
├── themes/
│   ├── theme_oht50_unified.css            # Main theme file
│   └── theme_components.css                # Component styles
└── documentation/
    ├── design_system_documentation.md      # This file
    ├── component_guidelines.md             # Component usage
    └── accessibility_checklist.md          # Accessibility guide
```

### **Theme Files:**
- **theme_oht50_unified.css:** Design tokens, colors, typography, spacing
- **theme_components.css:** Component-specific styles
- **theme_utilities.css:** Utility classes và helpers

---

## 📊 **DESIGN TOKENS**

### **Border Radius:**
```css
--radius-sm: 0.125rem;   /* 2px */
--radius-md: 0.375rem;   /* 6px */
--radius-lg: 0.5rem;     /* 8px */
--radius-xl: 0.75rem;    /* 12px */
--radius-2xl: 1rem;      /* 16px */
```

### **Shadows:**
```css
--shadow-sm: 0 1px 2px 0 rgb(0 0 0 / 0.05);
--shadow-md: 0 4px 6px -1px rgb(0 0 0 / 0.1);
--shadow-lg: 0 10px 15px -3px rgb(0 0 0 / 0.1);
--shadow-xl: 0 20px 25px -5px rgb(0 0 0 / 0.1);
```

### **Transitions:**
```css
--transition-fast: 150ms ease-in-out;
--transition-normal: 250ms ease-in-out;
--transition-slow: 350ms ease-in-out;
```

---

## 🎯 **QUALITY ASSURANCE**

### **Design Review Checklist:**
- [ ] Follows design system guidelines
- [ ] Uses correct color palette
- [ ] Implements proper typography
- [ ] Follows spacing system
- [ ] Includes accessibility features
- [ ] Implements safety-first design
- [ ] Responsive design tested
- [ ] Performance optimized

### **Accessibility Checklist:**
- [ ] WCAG 2.1 AA compliance
- [ ] Keyboard navigation support
- [ ] Screen reader compatibility
- [ ] Color contrast ratios
- [ ] Focus indicators visible
- [ ] Touch target sizes adequate
- [ ] Semantic HTML structure
- [ ] ARIA labels implemented

---

## 📈 **VERSION HISTORY**

### **v2.0 (2025-02-11):**
- ✅ Complete design system documentation
- ✅ Updated color palette với safety colors
- ✅ Enhanced accessibility guidelines
- ✅ Added safety-first design principles
- ✅ Improved component library structure

### **v1.0 (2025-01-28):**
- ✅ Initial design system creation
- ✅ Basic color palette
- ✅ Typography guidelines
- ✅ Spacing system

---

## 🔗 **RELATED DOCUMENTS**

- **REQ_UI_UX_Design_System.md:** Design system requirements
- **REQ_UI_UX_Accessibility.md:** Accessibility requirements
- **REQ_UI_UX_Alert_System.md:** Alert system requirements
- **REQ_UI_UX_Information_Architecture.md:** Information architecture
- **TEAM_PROGRESS_TRACKER.md:** Project progress tracking

---

**🚨 Lưu ý:** Tài liệu này phải được cập nhật mỗi khi có thay đổi trong design system. Tất cả team members phải tuân thủ guidelines này để đảm bảo consistency.
