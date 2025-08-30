# 🎨 OHT-50 Design System Guide

**Phiên bản:** v2.0  
**Ngày cập nhật:** 2025-01-28  
**Mục tiêu:** Comprehensive guide cho OHT-50 design system

---

## 🎯 **DESIGN PHILOSOPHY**

### **Industrial Theme:**
- **Primary Color:** Orange (#FF6B35) - Industrial, energetic, safety-focused
- **Secondary Colors:** Industrial grays - Professional, technical, clean
- **Safety Colors:** Red, yellow, green - Clear status indication
- **Typography:** Inter, JetBrains Mono, Space Grotesk - Readable, technical

### **Safety-First Design:**
- **Emergency Controls:** Always prominent và accessible
- **Alert System:** Multi-level notification với clear hierarchy
- **Validation:** Real-time feedback cho critical actions
- **Confirmation:** Dangerous actions require explicit confirmation

---

## 🎨 **DESIGN TOKENS**

### **Color System:**
```css
/* Primary Colors */
--primary-orange: #FF6B35;
--primary-orange-light: #FF8A5C;
--primary-orange-dark: #E55A2B;

/* Industrial Grays */
--industrial-gray-50: #F9FAFB;
--industrial-gray-100: #F3F4F6;
--industrial-gray-200: #E5E7EB;
--industrial-gray-300: #D1D5DB;
--industrial-gray-400: #9CA3AF;
--industrial-gray-500: #6B7280;
--industrial-gray-600: #4B5563;
--industrial-gray-700: #374151;
--industrial-gray-800: #1F2937;
--industrial-gray-900: #111827;

/* Semantic Colors */
--success-green: #10B981;
--warning-yellow: #F59E0B;
--error-red: #EF4444;
--info-blue: #3B82F6;

/* Safety Colors */
--safety-red: #DC2626;
--safety-orange: #EA580C;
--safety-yellow: #CA8A04;
--safety-green: #16A34A;
```

### **Typography System:**
```css
/* Font Families */
--font-sans: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
--font-mono: 'JetBrains Mono', 'Fira Code', 'Source Code Pro', monospace;
--font-display: 'Space Grotesk', 'Inter', sans-serif;

/* Font Sizes */
--text-xs: 0.75rem;    /* 12px */
--text-sm: 0.875rem;   /* 14px */
--text-base: 1rem;     /* 16px */
--text-lg: 1.125rem;   /* 18px */
--text-xl: 1.25rem;    /* 20px */
--text-2xl: 1.5rem;    /* 24px */
--text-3xl: 1.875rem;  /* 30px */
--text-4xl: 2.25rem;   /* 36px */

/* Font Weights */
--font-light: 300;
--font-normal: 400;
--font-medium: 500;
--font-semibold: 600;
--font-bold: 700;
--font-extrabold: 800;
```

### **Spacing System:**
```css
/* Base Spacing Unit */
--space-unit: 0.25rem; /* 4px */

/* Spacing Scale */
--space-xs: 0.25rem;   /* 4px */
--space-sm: 0.5rem;    /* 8px */
--space-md: 1rem;      /* 16px */
--space-lg: 1.5rem;    /* 24px */
--space-xl: 2rem;      /* 32px */
--space-2xl: 3rem;     /* 48px */
--space-3xl: 4rem;     /* 64px */
```

### **Border Radius System:**
```css
--radius-none: 0;
--radius-sm: 0.125rem;   /* 2px */
--radius-md: 0.375rem;   /* 6px */
--radius-lg: 0.5rem;     /* 8px */
--radius-xl: 0.75rem;    /* 12px */
--radius-2xl: 1rem;      /* 16px */
--radius-full: 9999px;
```

### **Shadow System:**
```css
--shadow-xs: 0 1px 2px 0 rgba(0, 0, 0, 0.05);
--shadow-sm: 0 1px 3px 0 rgba(0, 0, 0, 0.1), 0 1px 2px 0 rgba(0, 0, 0, 0.06);
--shadow-md: 0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06);
--shadow-lg: 0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -2px rgba(0, 0, 0, 0.05);
--shadow-xl: 0 20px 25px -5px rgba(0, 0, 0, 0.1), 0 10px 10px -5px rgba(0, 0, 0, 0.04);

/* Focus Shadows */
--shadow-focus: 0 0 0 3px rgba(255, 107, 53, 0.1);
--shadow-focus-error: 0 0 0 3px rgba(239, 68, 68, 0.1);
--shadow-focus-success: 0 0 0 3px rgba(16, 185, 129, 0.1);
```

---

## 🧩 **COMPONENT LIBRARY**

### **Button Components:**
```html
<!-- Primary Button -->
<button class="btn btn-primary">Primary Button</button>

<!-- Secondary Button -->
<button class="btn btn-secondary">Secondary Button</button>

<!-- Danger Button -->
<button class="btn btn-danger">Danger Button</button>

<!-- Emergency Button -->
<button class="btn btn-emergency">
  <span class="icon">🚨</span>
  EMERGENCY STOP
</button>

<!-- Button Sizes -->
<button class="btn btn-primary btn-xs">Extra Small</button>
<button class="btn btn-primary btn-sm">Small</button>
<button class="btn btn-primary">Default</button>
<button class="btn btn-primary btn-lg">Large</button>
<button class="btn btn-primary btn-xl">Extra Large</button>
```

### **Card Components:**
```html
<!-- Default Card -->
<div class="card">
  <div class="card-header">
    <h3 class="card-title">Card Title</h3>
  </div>
  <div class="card-body">
    <p>Card content goes here.</p>
  </div>
  <div class="card-footer">
    <button class="btn btn-primary">Action</button>
  </div>
</div>

<!-- Semantic Cards -->
<div class="card card-danger">
  <div class="card-header">Critical Alert</div>
  <div class="card-body">Emergency situation detected.</div>
</div>

<div class="card card-warning">
  <div class="card-header">Warning</div>
  <div class="card-body">Attention required.</div>
</div>

<div class="card card-success">
  <div class="card-header">Success</div>
  <div class="card-body">Operation completed successfully.</div>
</div>
```

### **Form Components:**
```html
<!-- Form Group -->
<div class="form-group">
  <label for="input1" class="form-label">Input Label</label>
  <input type="text" id="input1" class="form-input" placeholder="Enter text...">
  <div class="form-help">Help text goes here.</div>
</div>

<!-- Form with Validation -->
<div class="form-group">
  <label for="input2" class="form-label">Required Field</label>
  <input type="text" id="input2" class="form-input" required>
  <div class="form-error">This field is required.</div>
</div>

<!-- Form with Success -->
<div class="form-group">
  <label for="input3" class="form-label">Valid Field</label>
  <input type="text" id="input3" class="form-input">
  <div class="form-success">Field is valid.</div>
</div>
```

### **Alert Components:**
```html
<!-- Info Alert -->
<div class="alert alert-info">
  <div class="alert-icon">ℹ️</div>
  <div class="alert-content">
    <div class="alert-title">Information</div>
    <div class="alert-message">This is an informational message.</div>
  </div>
</div>

<!-- Success Alert -->
<div class="alert alert-success">
  <div class="alert-icon">✅</div>
  <div class="alert-content">
    <div class="alert-title">Success</div>
    <div class="alert-message">Operation completed successfully.</div>
  </div>
</div>

<!-- Warning Alert -->
<div class="alert alert-warning">
  <div class="alert-icon">⚠️</div>
  <div class="alert-content">
    <div class="alert-title">Warning</div>
    <div class="alert-message">Attention required.</div>
  </div>
</div>

<!-- Danger Alert -->
<div class="alert alert-danger">
  <div class="alert-icon">🚨</div>
  <div class="alert-content">
    <div class="alert-title">Critical Error</div>
    <div class="alert-message">Emergency situation detected.</div>
  </div>
</div>
```

---

## 📱 **RESPONSIVE DESIGN**

### **Breakpoints:**
```css
/* Mobile First Approach */
/* Base styles for mobile (0-640px) */

/* Small screens (640px+) */
@media (min-width: 640px) {
  /* Tablet styles */
}

/* Medium screens (768px+) */
@media (min-width: 768px) {
  /* Small desktop styles */
}

/* Large screens (1024px+) */
@media (min-width: 1024px) {
  /* Desktop styles */
}

/* Extra large screens (1280px+) */
@media (min-width: 1280px) {
  /* Large desktop styles */
}

/* 2XL screens (1536px+) */
@media (min-width: 1536px) {
  /* Extra large desktop styles */
}
```

### **Grid System:**
```css
/* 12-Column Grid */
.grid {
  display: grid;
  grid-template-columns: repeat(12, 1fr);
  gap: var(--space-lg);
}

/* Responsive Grid Classes */
.grid-cols-1 { grid-template-columns: repeat(1, 1fr); }
.grid-cols-2 { grid-template-columns: repeat(2, 1fr); }
.grid-cols-3 { grid-template-columns: repeat(3, 1fr); }
.grid-cols-4 { grid-template-columns: repeat(4, 1fr); }
.grid-cols-6 { grid-template-columns: repeat(6, 1fr); }
.grid-cols-12 { grid-template-columns: repeat(12, 1fr); }

/* Responsive Breakpoints */
@media (min-width: 768px) {
  .md\:grid-cols-2 { grid-template-columns: repeat(2, 1fr); }
  .md\:grid-cols-3 { grid-template-columns: repeat(3, 1fr); }
  .md\:grid-cols-4 { grid-template-columns: repeat(4, 1fr); }
}

@media (min-width: 1024px) {
  .lg\:grid-cols-3 { grid-template-columns: repeat(3, 1fr); }
  .lg\:grid-cols-4 { grid-template-columns: repeat(4, 1fr); }
  .lg\:grid-cols-6 { grid-template-columns: repeat(6, 1fr); }
}
```

---

## ♿ **ACCESSIBILITY GUIDELINES**

### **WCAG 2.1 AA Compliance:**
- **Color Contrast:** Minimum 4.5:1 for normal text, 3:1 for large text
- **Keyboard Navigation:** All interactive elements accessible via keyboard
- **Screen Reader Support:** Proper ARIA labels và semantic HTML
- **Focus Management:** Clear focus indicators và logical tab order

### **Accessibility Features:**
```css
/* Focus Indicators */
*:focus-visible {
  outline: var(--border-width-2) solid var(--primary-orange);
  outline-offset: var(--space-xs);
  border-radius: var(--radius-sm);
}

/* Skip Links */
.skip-link {
  position: absolute;
  top: -40px;
  left: 6px;
  background: var(--primary-orange);
  color: white;
  padding: 8px;
  text-decoration: none;
  border-radius: var(--radius-sm);
  z-index: 10000;
}

.skip-link:focus {
  top: 6px;
}

/* Reduced Motion */
@media (prefers-reduced-motion: reduce) {
  * {
    animation-duration: 0.01ms !important;
    animation-iteration-count: 1 !important;
    transition-duration: 0.01ms !important;
  }
}

/* High Contrast Mode */
@media (prefers-contrast: high) {
  :root {
    --primary-orange: #ff6b35;
    --industrial-gray-700: #000000;
    --industrial-gray-600: #333333;
    --bg-primary: #ffffff;
  }
}
```

---

## 🚨 **SAFETY DESIGN PATTERNS**

### **Emergency Controls:**
```css
/* Emergency Button */
.btn-emergency {
  background: var(--safety-red);
  color: white;
  border: 3px solid var(--safety-red);
  border-radius: var(--radius-lg);
  padding: var(--space-lg) var(--space-xl);
  font-size: var(--text-lg);
  font-weight: var(--font-bold);
  animation: pulse-emergency 2s infinite;
}

@keyframes pulse-emergency {
  0% { box-shadow: 0 0 0 0 rgba(220, 38, 38, 0.7); }
  70% { box-shadow: 0 0 0 10px rgba(220, 38, 38, 0); }
  100% { box-shadow: 0 0 0 0 rgba(220, 38, 38, 0); }
}
```

### **Alert Hierarchy:**
1. **Critical (Red):** Emergency stop, system failure
2. **Warning (Yellow):** Low battery, maintenance due
3. **Info (Blue):** Status updates, task completion
4. **Success (Green):** Operation successful, system ready

---

## 📋 **USAGE GUIDELINES**

### **Do's:**
- ✅ Use consistent spacing với spacing system
- ✅ Apply semantic colors cho status indicators
- ✅ Implement proper focus management
- ✅ Test với screen readers
- ✅ Follow responsive design principles
- ✅ Use emergency controls cho safety-critical actions

### **Don'ts:**
- ❌ Don't override design tokens
- ❌ Don't skip accessibility features
- ❌ Don't use colors alone để convey information
- ❌ Don't create custom components without approval
- ❌ Don't ignore responsive breakpoints
- ❌ Don't hide emergency controls

---

## 🔧 **IMPLEMENTATION**

### **CSS Import:**
```html
<!-- Import design system -->
<link rel="stylesheet" href="themes/theme_oht50_unified.css">
<link rel="stylesheet" href="themes/theme_components.css">
```

### **HTML Structure:**
```html
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>OHT-50 Dashboard</title>
  <link rel="stylesheet" href="themes/theme_oht50_unified.css">
  <link rel="stylesheet" href="themes/theme_components.css">
</head>
<body>
  <a href="#main-content" class="skip-link">Skip to main content</a>
  
  <div class="page-container">
    <header class="page-header">
      <!-- Navigation content -->
    </header>
    
    <main id="main-content" class="page-main">
      <!-- Main content -->
    </main>
    
    <footer class="page-footer">
      <!-- Footer content -->
    </footer>
  </div>
</body>
</html>
```

---

**Changelog v2.0:**
- ✅ Created comprehensive design system guide
- ✅ Added detailed component examples
- ✅ Included accessibility guidelines
- ✅ Added safety design patterns
- ✅ Enhanced responsive design guidelines
- ✅ Added implementation examples

**🚨 Lưu ý:** Tất cả components phải tuân thủ safety-first design principles và accessibility standards cho OHT-50 robot system.
