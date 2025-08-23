# 🎨 OHT-50 Design System

**Phiên bản:** v1.0  
**Ngày cập nhật:** 2025-01-27  
**Team:** Frontend  
**Status:** ✅ COMPLETE

---

## 📋 **Tổng quan**

Design System cho OHT-50 Master Module Dashboard, đảm bảo consistency, accessibility và user experience tốt nhất.

### **🎯 Mục tiêu:**
- Unified design language cho toàn bộ dashboard
- Accessibility compliance (WCAG 2.1 AA)
- Responsive design cho mọi thiết bị
- Dark mode support
- Performance optimization

---

## 🎨 **Design Tokens**

### **Color System (OKLCH)**

```css
:root {
  /* Primary Colors */
  --primary: oklch(0.6489 0.2370 26.9728);      /* Blue */
  --secondary: oklch(0.9680 0.2110 109.7692);   /* Green */
  --accent: oklch(0.5635 0.2408 260.8178);      /* Purple */
  --destructive: oklch(0.5770 0.2450 27.3250);  /* Red */
  
  /* System States */
  --state-idle: oklch(0.7323 0.2492 142.4953);  /* Green */
  --state-move: oklch(0.6489 0.2370 26.9728);   /* Blue */
  --state-dock: oklch(0.9680 0.2110 109.7692);  /* Yellow */
  --state-fault: oklch(0.5770 0.2450 27.3250);  /* Red */
  --state-estop: oklch(0.5931 0.2726 328.3634); /* Red */
  
  /* Neutral Colors */
  --background: oklch(1.0000 0 0);              /* White */
  --foreground: oklch(0.1448 0 0);              /* Black */
  --card: oklch(1.0000 0 0);                    /* White */
  --border: oklch(0.9219 0 0);                  /* Light Gray */
  --muted: oklch(0.9702 0 0);                   /* Very Light Gray */
  --muted-foreground: oklch(0.5486 0 0);        /* Medium Gray */
}
```

### **Typography System**

```css
:root {
  /* Font Families */
  --font-sans: 'Inter', system-ui, sans-serif;
  --font-mono: 'JetBrains Mono', monospace;
  --font-serif: 'Merriweather', serif;
  
  /* Font Sizes */
  --text-xs: 0.75rem;    /* 12px */
  --text-sm: 0.875rem;   /* 14px */
  --text-base: 1rem;     /* 16px */
  --text-lg: 1.125rem;   /* 18px */
  --text-xl: 1.25rem;    /* 20px */
  --text-2xl: 1.5rem;    /* 24px */
  --text-3xl: 1.875rem;  /* 30px */
  
  /* Font Weights */
  --font-normal: 400;
  --font-medium: 500;
  --font-semibold: 600;
  --font-bold: 700;
  
  /* Line Heights */
  --leading-tight: 1.25;
  --leading-normal: 1.5;
  --leading-relaxed: 1.75;
}
```

### **Spacing System**

```css
:root {
  /* Spacing Scale */
  --spacing-xs: 0.25rem;   /* 4px */
  --spacing-sm: 0.5rem;    /* 8px */
  --spacing-md: 1rem;      /* 16px */
  --spacing-lg: 1.5rem;    /* 24px */
  --spacing-xl: 2rem;      /* 32px */
  --spacing-2xl: 3rem;     /* 48px */
  --spacing-3xl: 4rem;     /* 64px */
  
  /* Component Spacing */
  --space-1: 0.25rem;
  --space-2: 0.5rem;
  --space-3: 0.75rem;
  --space-4: 1rem;
  --space-5: 1.25rem;
  --space-6: 1.5rem;
  --space-8: 2rem;
  --space-10: 2.5rem;
  --space-12: 3rem;
  --space-16: 4rem;
  --space-20: 5rem;
  --space-24: 6rem;
}
```

### **Border Radius**

```css
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

### **Shadows**

```css
:root {
  --shadow-sm: 0 1px 2px 0 rgb(0 0 0 / 0.05);
  --shadow-md: 0 4px 6px -1px rgb(0 0 0 / 0.1), 0 2px 4px -2px rgb(0 0 0 / 0.1);
  --shadow-lg: 0 10px 15px -3px rgb(0 0 0 / 0.1), 0 4px 6px -4px rgb(0 0 0 / 0.1);
  --shadow-xl: 0 20px 25px -5px rgb(0 0 0 / 0.1), 0 8px 10px -6px rgb(0 0 0 / 0.1);
  --shadow-2xl: 0 25px 50px -12px rgb(0 0 0 / 0.25);
}
```

---

## 🎯 **System States**

### **OHT-50 System States**

| State | Color | Description | Usage |
|-------|-------|-------------|-------|
| **Idle** | `--state-idle` | Hệ thống sẵn sàng | Khi OHT-50 đang chờ lệnh |
| **Move** | `--state-move` | Đang di chuyển | Khi OHT-50 đang chạy |
| **Dock** | `--state-dock` | Đang dock | Khi OHT-50 đang vào/ra dock |
| **Fault** | `--state-fault` | Lỗi hệ thống | Khi có lỗi cần xử lý |
| **E-Stop** | `--state-estop` | Dừng khẩn cấp | Khi E-Stop được kích hoạt |

### **Component States**

| State | Description | Visual Indicators |
|-------|-------------|-------------------|
| **Default** | Trạng thái bình thường | Normal styling |
| **Hover** | Khi hover chuột | Slight color change, shadow |
| **Active** | Khi đang được click | Pressed appearance |
| **Focus** | Khi được focus | Focus ring, outline |
| **Disabled** | Khi không thể tương tác | Reduced opacity, no interaction |
| **Loading** | Khi đang xử lý | Spinner, skeleton |

---

## 📱 **Responsive Design**

### **Breakpoints**

```css
/* Mobile First Approach */
--breakpoint-sm: 640px;   /* Small devices */
--breakpoint-md: 768px;   /* Medium devices */
--breakpoint-lg: 1024px;  /* Large devices */
--breakpoint-xl: 1280px;  /* Extra large devices */
--breakpoint-2xl: 1536px; /* 2X large devices */
```

### **Grid System**

```css
/* 12-column grid system */
.grid {
  display: grid;
  grid-template-columns: repeat(12, 1fr);
  gap: var(--spacing-md);
}

/* Responsive columns */
.col-1 { grid-column: span 1; }
.col-2 { grid-column: span 2; }
.col-3 { grid-column: span 3; }
.col-4 { grid-column: span 4; }
.col-6 { grid-column: span 6; }
.col-12 { grid-column: span 12; }

/* Mobile responsive */
@media (max-width: 768px) {
  .col-md-6 { grid-column: span 12; }
  .col-md-4 { grid-column: span 6; }
}
```

---

## 🌙 **Dark Mode**

### **Dark Mode Tokens**

```css
[data-theme="dark"] {
  --background: oklch(0.18 0.03 50);
  --foreground: oklch(0.92 0.02 85);
  --card: oklch(0.22 0.03 50);
  --card-foreground: oklch(0.96 0.01 85);
  --border: oklch(0.30 0.02 50);
  --muted: oklch(0.26 0.02 50);
  --muted-foreground: oklch(0.80 0.02 85);
  --primary: oklch(0.72 0.16 45);
  --secondary: oklch(0.65 0.06 115);
  --accent: oklch(0.70 0.12 35);
  --destructive: oklch(0.62 0.20 28);
  --ring: oklch(0.72 0.16 45);
}
```

### **Implementation**

```javascript
// Theme toggle
const toggleTheme = () => {
  const currentTheme = document.documentElement.getAttribute('data-theme');
  const newTheme = currentTheme === 'dark' ? 'light' : 'dark';
  document.documentElement.setAttribute('data-theme', newTheme);
  localStorage.setItem('theme', newTheme);
};

// Initialize theme
const savedTheme = localStorage.getItem('theme') || 'light';
document.documentElement.setAttribute('data-theme', savedTheme);
```

---

## ♿ **Accessibility**

### **Color Contrast**

Tất cả color combinations phải đạt WCAG 2.1 AA standards:
- **Normal text:** 4.5:1 contrast ratio
- **Large text:** 3:1 contrast ratio
- **UI components:** 3:1 contrast ratio

### **Focus Management**

```css
/* Focus indicators */
.btn:focus-visible,
a:focus-visible,
input:focus-visible {
  outline: 2px solid var(--ring);
  outline-offset: 2px;
}

/* Skip links */
.skip-link {
  position: absolute;
  top: -40px;
  left: 6px;
  background: var(--primary);
  color: var(--primary-foreground);
  padding: 8px;
  text-decoration: none;
  border-radius: var(--radius-md);
  z-index: 100;
}

.skip-link:focus {
  top: 6px;
}
```

### **Keyboard Navigation**

- Tất cả interactive elements phải accessible bằng keyboard
- Tab order phải logical và intuitive
- Escape key để close modals/dropdowns
- Enter/Space để activate buttons

### **Screen Reader Support**

```html
<!-- Semantic HTML -->
<button aria-label="Close dialog" aria-pressed="false">
  <i data-lucide="x"></i>
</button>

<!-- ARIA labels -->
<div role="status" aria-live="polite">
  Loading data...
</div>

<!-- Skip links -->
<a href="#main-content" class="skip-link">
  Skip to main content
</a>
```

---

## 🧩 **Component Library**

### **Core Components**

1. **Buttons**
   - Primary, Secondary, Ghost, Destructive
   - Sizes: sm, md, lg
   - States: default, hover, active, disabled, loading

2. **Cards**
   - Default, Elevated, Interactive
   - Header, Content, Footer sections

3. **Forms**
   - Input fields, Select dropdowns, Checkboxes, Radio buttons
   - Validation states và error messages

4. **Navigation**
   - Sidebar navigation, Breadcrumbs, Pagination
   - Active states và hover effects

5. **Data Display**
   - Tables, Lists, Badges, Status indicators
   - Loading states và empty states

6. **Feedback**
   - Alerts, Toasts, Modals, Tooltips
   - Success, Warning, Error, Info types

### **Component Usage**

```jsx
// Button component
<Button variant="primary" size="md" disabled={false}>
  Start Operation
</Button>

// Card component
<Card>
  <CardHeader>
    <CardTitle>System Status</CardTitle>
  </CardHeader>
  <CardContent>
    <StatusBadge state="idle">Idle</StatusBadge>
  </CardContent>
</Card>

// Form component
<FormField>
  <FormLabel>Speed Limit</FormLabel>
  <FormInput type="number" min="0" max="100" />
  <FormMessage>Enter speed limit in m/s</FormMessage>
</FormField>
```

---

## 📊 **Performance Guidelines**

### **Optimization Targets**

- **First Contentful Paint:** < 1.5s
- **Largest Contentful Paint:** < 2.5s
- **Cumulative Layout Shift:** < 0.1
- **First Input Delay:** < 100ms

### **Best Practices**

1. **CSS Optimization**
   - Use CSS custom properties cho theming
   - Minimize CSS bundle size
   - Use critical CSS inline

2. **Image Optimization**
   - Use WebP format với fallbacks
   - Implement lazy loading
   - Provide appropriate sizes

3. **JavaScript Optimization**
   - Code splitting và lazy loading
   - Tree shaking cho unused code
   - Minimize bundle size

---

## 🔧 **Implementation**

### **Setup Instructions**

1. **Install dependencies:**
```bash
npm install @radix-ui/react-* lucide-react
```

2. **Import design tokens:**
```css
@import './design-tokens.css';
```

3. **Setup theme provider:**
```jsx
import { ThemeProvider } from './components/theme-provider';

function App() {
  return (
    <ThemeProvider>
      <Dashboard />
    </ThemeProvider>
  );
}
```

### **Development Workflow**

1. **Design tokens** → Update CSS custom properties
2. **Components** → Create/update component library
3. **Pages** → Implement using components
4. **Testing** → Accessibility và performance testing
5. **Documentation** → Update component docs

---

## 📚 **Resources**

### **Design Files**
- **Figma:** [OHT-50 Design System](https://figma.com/file/...)
- **Icons:** [Lucide Icons](https://lucide.dev/)
- **Fonts:** [Google Fonts](https://fonts.google.com/)

### **Documentation**
- [Component Library](./COMPONENT_LIBRARY.md)
- [Accessibility Guidelines](./ACCESSIBILITY.md)
- [Icon Guidelines](./ICON_GUIDELINES.md)

### **Tools**
- **Color Contrast:** [WebAIM Contrast Checker](https://webaim.org/resources/contrastchecker/)
- **Accessibility:** [axe DevTools](https://www.deque.com/axe/)
- **Performance:** [Lighthouse](https://developers.google.com/web/tools/lighthouse)

---

**Changelog v1.0:**
- ✅ Created comprehensive design system
- ✅ Defined color tokens với OKLCH
- ✅ Established typography system
- ✅ Set up responsive design guidelines
- ✅ Added accessibility requirements
- ✅ Created component library structure
- ✅ Added performance guidelines