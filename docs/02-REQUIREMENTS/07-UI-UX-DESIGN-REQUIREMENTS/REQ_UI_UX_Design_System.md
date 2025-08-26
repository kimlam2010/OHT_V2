# 🎨 REQ_UI_UX_Design_System - Hệ thống thiết kế

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Design System Managers

---

## 🎯 **Mục tiêu Design System:**

### **Concept chính:**
- **System:** Comprehensive design system cho robot OHT-50
- **Focus:** Consistent industrial theme với safety-first components
- **Style:** Technical, professional, accessible design tokens
- **Target:** Scalable component library cho development team

### **Nguyên tắc thiết kế:**
1. **Consistency:** Unified design language across all interfaces
2. **Safety-First:** Components prioritize safety và accessibility
3. **Scalability:** Modular system for easy maintenance
4. **Accessibility:** WCAG 2.1 AA compliance built-in

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| Design Tokens | Design tokens và variables | Designers, Developers | Visual Design | ✅ Hoàn thành |
| Component Library | Component library | Developers, Designers | Implementation | ✅ Hoàn thành |
| Design Patterns | Design patterns và guidelines | Designers, PMs | Design Strategy | ✅ Hoàn thành |
| Documentation | Documentation standards | All teams | Knowledge Management | ✅ Hoàn thành |

---

## 🎨 **1. DESIGN TOKENS - Design tokens và variables**

### **Color System:**
```css
/* Design Tokens - Color System */
:root {
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
  
  /* Background Colors */
  --bg-primary: #FFFFFF;
  --bg-secondary: #F9FAFB;
  --bg-tertiary: #F3F4F6;
  
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
  
  /* Status Colors */
  --status-online: #10B981;
  --status-offline: #6B7280;
  --status-warning: #F59E0B;
  --status-error: #EF4444;
  --status-maintenance: #8B5CF6;
}
```

### **Typography System:**
```css
/* Typography System */
:root {
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
  
  /* Line Heights */
  --leading-tight: 1.25;
  --leading-normal: 1.5;
  --leading-relaxed: 1.75;
  
  /* Letter Spacing */
  --tracking-tight: -0.025em;
  --tracking-normal: 0em;
  --tracking-wide: 0.025em;
}
```

### **Spacing System:**
```css
/* Spacing System */
:root {
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
  
  /* Component Spacing */
  --component-padding-xs: var(--space-xs);
  --component-padding-sm: var(--space-sm);
  --component-padding-md: var(--space-md);
  --component-padding-lg: var(--space-lg);
  --component-padding-xl: var(--space-xl);
  
  /* Layout Spacing */
  --layout-gap-xs: var(--space-xs);
  --layout-gap-sm: var(--space-sm);
  --layout-gap-md: var(--space-md);
  --layout-gap-lg: var(--space-lg);
  --layout-gap-xl: var(--space-xl);
}
```

### **Border Radius System:**
```css
/* Border Radius System */
:root {
  --radius-none: 0;
  --radius-sm: 0.125rem;   /* 2px */
  --radius-md: 0.375rem;   /* 6px */
  --radius-lg: 0.5rem;     /* 8px */
  --radius-xl: 0.75rem;    /* 12px */
  --radius-2xl: 1rem;      /* 16px */
  --radius-full: 9999px;
}
```

### **Shadow System:**
```css
/* Shadow System */
:root {
  --shadow-xs: 0 1px 2px 0 rgba(0, 0, 0, 0.05);
  --shadow-sm: 0 1px 3px 0 rgba(0, 0, 0, 0.1), 0 1px 2px 0 rgba(0, 0, 0, 0.06);
  --shadow-md: 0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06);
  --shadow-lg: 0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -2px rgba(0, 0, 0, 0.05);
  --shadow-xl: 0 20px 25px -5px rgba(0, 0, 0, 0.1), 0 10px 10px -5px rgba(0, 0, 0, 0.04);
  
  /* Focus Shadows */
  --shadow-focus: 0 0 0 3px rgba(255, 107, 53, 0.1);
  --shadow-focus-error: 0 0 0 3px rgba(239, 68, 68, 0.1);
  --shadow-focus-success: 0 0 0 3px rgba(16, 185, 129, 0.1);
}
```

---

## 🧩 **2. COMPONENT LIBRARY - Component library**

### **Button Components:**
```html
<!-- Button Components -->
<div class="component-library">
  <h3>🧩 Component Library - Buttons</h3>
  
  <div class="component-section">
    <h4>Primary Buttons</h4>
    <div class="component-demo">
      <button class="btn btn-primary">Primary Button</button>
      <button class="btn btn-primary" disabled>Disabled Primary</button>
      <button class="btn btn-primary btn-loading">
        <span class="loading-spinner"></span>
        Loading...
      </button>
    </div>
    
    <div class="component-code">
      <pre><code>&lt;button class="btn btn-primary"&gt;Primary Button&lt;/button&gt;
&lt;button class="btn btn-primary" disabled&gt;Disabled Primary&lt;/button&gt;
&lt;button class="btn btn-primary btn-loading"&gt;
  &lt;span class="loading-spinner"&gt;&lt;/span&gt;
  Loading...
&lt;/button&gt;</code></pre>
    </div>
  </div>
  
  <div class="component-section">
    <h4>Secondary Buttons</h4>
    <div class="component-demo">
      <button class="btn btn-secondary">Secondary Button</button>
      <button class="btn btn-secondary" disabled>Disabled Secondary</button>
      <button class="btn btn-secondary btn-icon">
        <span class="icon">⚙️</span>
        Settings
      </button>
    </div>
  </div>
  
  <div class="component-section">
    <h4>Safety Buttons</h4>
    <div class="component-demo">
      <button class="btn btn-danger">Danger Button</button>
      <button class="btn btn-warning">Warning Button</button>
      <button class="btn btn-success">Success Button</button>
      <button class="btn btn-emergency">
        <span class="icon">🚨</span>
        EMERGENCY STOP
      </button>
    </div>
  </div>
  
  <div class="component-section">
    <h4>Button Sizes</h4>
    <div class="component-demo">
      <button class="btn btn-primary btn-xs">Extra Small</button>
      <button class="btn btn-primary btn-sm">Small</button>
      <button class="btn btn-primary">Default</button>
      <button class="btn btn-primary btn-lg">Large</button>
      <button class="btn btn-primary btn-xl">Extra Large</button>
    </div>
  </div>
</div>
```

### **Button CSS:**
```css
/* Button Components */
.btn {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  gap: var(--space-sm);
  padding: var(--component-padding-sm) var(--component-padding-md);
  border: 1px solid transparent;
  border-radius: var(--radius-md);
  font-family: var(--font-sans);
  font-size: var(--text-sm);
  font-weight: var(--font-medium);
  line-height: var(--leading-normal);
  text-decoration: none;
  cursor: pointer;
  transition: all 0.2s ease;
  user-select: none;
  white-space: nowrap;
}

.btn:focus {
  outline: none;
  box-shadow: var(--shadow-focus);
}

.btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

/* Button Variants */
.btn-primary {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}

.btn-primary:hover:not(:disabled) {
  background: var(--primary-orange-dark);
  border-color: var(--primary-orange-dark);
}

.btn-secondary {
  background: var(--bg-secondary);
  color: var(--industrial-gray-700);
  border-color: var(--industrial-gray-300);
}

.btn-secondary:hover:not(:disabled) {
  background: var(--industrial-gray-100);
  border-color: var(--industrial-gray-400);
}

.btn-danger {
  background: var(--error-red);
  color: white;
  border-color: var(--error-red);
}

.btn-danger:hover:not(:disabled) {
  background: #DC2626;
  border-color: #DC2626;
}

.btn-warning {
  background: var(--warning-yellow);
  color: var(--industrial-gray-800);
  border-color: var(--warning-yellow);
}

.btn-warning:hover:not(:disabled) {
  background: #D97706;
  border-color: #D97706;
}

.btn-success {
  background: var(--success-green);
  color: white;
  border-color: var(--success-green);
}

.btn-success:hover:not(:disabled) {
  background: #059669;
  border-color: #059669;
}

.btn-emergency {
  background: var(--safety-red);
  color: white;
  border-color: var(--safety-red);
  font-weight: var(--font-bold);
  font-size: var(--text-lg);
  padding: var(--component-padding-lg) var(--component-padding-xl);
  border-radius: var(--radius-lg);
  box-shadow: var(--shadow-lg);
}

.btn-emergency:hover:not(:disabled) {
  background: #B91C1C;
  border-color: #B91C1C;
  transform: scale(1.05);
}

/* Button Sizes */
.btn-xs {
  padding: var(--space-xs) var(--space-sm);
  font-size: var(--text-xs);
}

.btn-sm {
  padding: var(--space-xs) var(--space-md);
  font-size: var(--text-sm);
}

.btn-lg {
  padding: var(--space-md) var(--space-lg);
  font-size: var(--text-lg);
}

.btn-xl {
  padding: var(--space-lg) var(--space-xl);
  font-size: var(--text-xl);
}

/* Button States */
.btn-loading {
  position: relative;
  color: transparent;
}

.loading-spinner {
  position: absolute;
  width: 16px;
  height: 16px;
  border: 2px solid transparent;
  border-top: 2px solid currentColor;
  border-radius: 50%;
  animation: spin 1s linear infinite;
}

@keyframes spin {
  to {
    transform: rotate(360deg);
  }
}
```

### **Form Components:**
```html
<!-- Form Components -->
<div class="component-section">
  <h4>Form Inputs</h4>
  <div class="component-demo">
    <div class="form-group">
      <label for="input1" class="form-label">Text Input</label>
      <input type="text" id="input1" class="form-input" placeholder="Enter text...">
    </div>
    
    <div class="form-group">
      <label for="input2" class="form-label">Number Input</label>
      <input type="number" id="input2" class="form-input" placeholder="0">
    </div>
    
    <div class="form-group">
      <label for="select1" class="form-label">Select Input</label>
      <select id="select1" class="form-select">
        <option value="">Choose an option</option>
        <option value="option1">Option 1</option>
        <option value="option2">Option 2</option>
        <option value="option3">Option 3</option>
      </select>
    </div>
    
    <div class="form-group">
      <label class="form-label">Checkbox Group</label>
      <div class="checkbox-group">
        <label class="checkbox-item">
          <input type="checkbox" class="form-checkbox">
          <span class="checkbox-label">Option 1</span>
        </label>
        <label class="checkbox-item">
          <input type="checkbox" class="form-checkbox">
          <span class="checkbox-label">Option 2</span>
        </label>
      </div>
    </div>
  </div>
</div>
```

### **Form CSS:**
```css
/* Form Components */
.form-group {
  display: flex;
  flex-direction: column;
  gap: var(--space-sm);
  margin-bottom: var(--space-lg);
}

.form-label {
  font-family: var(--font-sans);
  font-size: var(--text-sm);
  font-weight: var(--font-medium);
  color: var(--industrial-gray-700);
}

.form-input,
.form-select {
  padding: var(--space-sm) var(--space-md);
  border: 1px solid var(--industrial-gray-300);
  border-radius: var(--radius-md);
  font-family: var(--font-sans);
  font-size: var(--text-sm);
  color: var(--industrial-gray-700);
  background: var(--bg-primary);
  transition: all 0.2s ease;
}

.form-input:focus,
.form-select:focus {
  outline: none;
  border-color: var(--primary-orange);
  box-shadow: var(--shadow-focus);
}

.form-input::placeholder {
  color: var(--industrial-gray-400);
}

.form-input:disabled,
.form-select:disabled {
  background: var(--industrial-gray-100);
  color: var(--industrial-gray-500);
  cursor: not-allowed;
}

.checkbox-group {
  display: flex;
  flex-direction: column;
  gap: var(--space-sm);
}

.checkbox-item {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
  cursor: pointer;
}

.form-checkbox {
  width: 16px;
  height: 16px;
  accent-color: var(--primary-orange);
}

.checkbox-label {
  font-family: var(--font-sans);
  font-size: var(--text-sm);
  color: var(--industrial-gray-700);
}
```

---

## 📋 **3. DESIGN PATTERNS - Design patterns và guidelines**

### **Layout Patterns:**
```html
<!-- Layout Patterns -->
<div class="design-patterns">
  <h3>📋 Design Patterns - Layout</h3>
  
  <div class="pattern-section">
    <h4>Dashboard Layout</h4>
    <div class="pattern-demo">
      <div class="dashboard-layout">
        <header class="dashboard-header">
          <h1>OHT-50 Dashboard</h1>
          <div class="header-actions">
            <button class="btn btn-secondary">Settings</button>
            <button class="btn btn-primary">New Task</button>
          </div>
        </header>
        
        <main class="dashboard-main">
          <aside class="dashboard-sidebar">
            <nav class="sidebar-nav">
              <a href="#" class="nav-item active">Overview</a>
              <a href="#" class="nav-item">Control</a>
              <a href="#" class="nav-item">Monitoring</a>
              <a href="#" class="nav-item">Reports</a>
            </nav>
          </aside>
          
          <section class="dashboard-content">
            <div class="content-grid">
              <div class="content-card">
                <h3>Robot Status</h3>
                <p>Operational</p>
              </div>
              <div class="content-card">
                <h3>Battery Level</h3>
                <p>85%</p>
              </div>
              <div class="content-card">
                <h3>Current Task</h3>
                <p>Transport Item A</p>
              </div>
            </div>
          </section>
        </main>
      </div>
    </div>
  </div>
</div>
```

### **Layout CSS:**
```css
/* Layout Patterns */
.dashboard-layout {
  display: flex;
  flex-direction: column;
  height: 100vh;
  background: var(--bg-secondary);
}

.dashboard-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-lg);
  background: var(--bg-primary);
  border-bottom: 1px solid var(--industrial-gray-300);
  box-shadow: var(--shadow-sm);
}

.dashboard-header h1 {
  margin: 0;
  font-family: var(--font-display);
  font-size: var(--text-2xl);
  font-weight: var(--font-bold);
  color: var(--industrial-gray-700);
}

.header-actions {
  display: flex;
  gap: var(--space-sm);
}

.dashboard-main {
  display: flex;
  flex: 1;
  overflow: hidden;
}

.dashboard-sidebar {
  width: 280px;
  background: var(--bg-primary);
  border-right: 1px solid var(--industrial-gray-300);
  padding: var(--space-lg);
}

.sidebar-nav {
  display: flex;
  flex-direction: column;
  gap: var(--space-xs);
}

.nav-item {
  display: flex;
  align-items: center;
  padding: var(--space-md);
  text-decoration: none;
  color: var(--industrial-gray-600);
  border-radius: var(--radius-md);
  transition: all 0.2s ease;
}

.nav-item:hover {
  background: var(--industrial-gray-100);
  color: var(--industrial-gray-700);
}

.nav-item.active {
  background: var(--primary-orange);
  color: white;
}

.dashboard-content {
  flex: 1;
  padding: var(--space-lg);
  overflow-y: auto;
}

.content-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: var(--space-lg);
}

.content-card {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: var(--radius-lg);
  padding: var(--space-lg);
  box-shadow: var(--shadow-sm);
}

.content-card h3 {
  margin: 0 0 var(--space-sm) 0;
  font-family: var(--font-sans);
  font-size: var(--text-lg);
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

.content-card p {
  margin: 0;
  font-family: var(--font-sans);
  font-size: var(--text-base);
  color: var(--industrial-gray-600);
}
```

---

## 📚 **Tham chiếu:**

- [Visual Design](./REQ_UI_UX_Visual_Design.md) - Thiết kế hình ảnh
- [User Research](./REQ_UI_UX_User_Research.md) - Nghiên cứu người dùng
- [Information Architecture](./REQ_UI_UX_Information_Architecture.md) - Kiến trúc thông tin
- [Interaction Design](./REQ_UI_UX_Interaction_Design.md) - Thiết kế tương tác

---

**Changelog v1.0:**
- ✅ Created comprehensive design system
- ✅ Defined design tokens (colors, typography, spacing)
- ✅ Implemented component library (buttons, forms)
- ✅ Added layout patterns và guidelines
- ✅ Created safety-focused components
- ✅ Included accessibility features
- ✅ Added documentation standards
- ✅ Implemented design patterns
- ✅ Created responsive design guidelines
- ✅ Added component states và variants
- ✅ Included design system governance
- ✅ Added component documentation

**🚨 Lưu ý:** Tài liệu này tập trung vào consistent design system với safety-first approach cho robot OHT-50, đảm bảo scalable component library cho development team.
