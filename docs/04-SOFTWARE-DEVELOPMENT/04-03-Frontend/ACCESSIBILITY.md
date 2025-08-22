---
title: "Accessibility"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "Frontend Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['software-development']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# ♿ OHT-50 Accessibility Guidelines

**Phiên bản:** v1.0  
**Ngày cập nhật:** 2025-01-27  
**Team:** Frontend  
**Status:** ✅ COMPLETE

---

## 📋 **Tổng quan**

Accessibility Guidelines cho OHT-50 Dashboard, đảm bảo ứng dụng có thể sử dụng bởi tất cả người dùng, bao gồm người khuyết tật.

### **🎯 Mục tiêu:**
- WCAG 2.1 AA compliance
- Keyboard navigation support
- Screen reader compatibility
- Color contrast requirements
- Touch target sizes

---

## 🎨 **Color & Contrast**

### **WCAG 2.1 AA Standards**

| Element Type | Minimum Contrast Ratio | Example |
|--------------|----------------------|---------|
| **Normal Text** | 4.5:1 | Body text, labels |
| **Large Text** | 3:1 | Headings, titles |
| **UI Components** | 3:1 | Buttons, links |
| **Graphics** | 3:1 | Icons, charts |

### **Color Contrast Testing**

```css
/* Good contrast examples */
.text-primary {
  color: oklch(0.1448 0 0); /* Black text */
  background: oklch(1.0000 0 0); /* White background */
  /* Contrast ratio: 21:1 ✅ */
}

.text-secondary {
  color: oklch(0.5486 0 0); /* Medium gray text */
  background: oklch(1.0000 0 0); /* White background */
  /* Contrast ratio: 4.5:1 ✅ */
}

/* Avoid low contrast */
.text-bad {
  color: oklch(0.8000 0 0); /* Light gray text */
  background: oklch(1.0000 0 0); /* White background */
  /* Contrast ratio: 2.5:1 ❌ */
}
```

### **Color Blindness Considerations**

```css
/* Use patterns and icons in addition to color */
.status-indicator {
  /* Don't rely only on color */
  &::before {
    content: "●"; /* Add symbol */
  }
  
  &.success::before { content: "✓"; }
  &.warning::before { content: "⚠"; }
  &.error::before { content: "✗"; }
}
```

---

## ⌨️ **Keyboard Navigation**

### **Tab Order**

```html
<!-- Logical tab order -->
<div class="control-panel">
  <button tabindex="0">Start</button>
  <button tabindex="0">Stop</button>
  <button tabindex="0">Emergency Stop</button>
  <input tabindex="0" type="number" />
</div>
```

### **Focus Management**

```css
/* Visible focus indicators */
.btn:focus-visible,
a:focus-visible,
input:focus-visible {
  outline: 2px solid var(--ring);
  outline-offset: 2px;
  border-radius: var(--radius-md);
}

/* Custom focus styles */
.custom-button:focus-visible {
  box-shadow: 0 0 0 2px var(--background), 0 0 0 4px var(--ring);
}
```

### **Skip Links**

```html
<!-- Skip to main content -->
<a href="#main-content" class="skip-link">
  Skip to main content
</a>

<main id="main-content">
  <!-- Main content here -->
</main>
```

```css
.skip-link {
  position: absolute;
  top: -40px;
  left: 6px;
  background: var(--primary);
  color: var(--primary-foreground);
  padding: 8px 16px;
  text-decoration: none;
  border-radius: var(--radius-md);
  z-index: 1000;
  transition: top 0.2s;
}

.skip-link:focus {
  top: 6px;
}
```

---

## 🗣️ **Screen Reader Support**

### **Semantic HTML**

```html
<!-- Use semantic elements -->
<main role="main">
  <section aria-labelledby="status-heading">
    <h2 id="status-heading">System Status</h2>
    <div role="status" aria-live="polite">
      Current status: Idle
    </div>
  </section>
</main>

<!-- Proper heading hierarchy -->
<h1>OHT-50 Dashboard</h1>
<h2>System Control</h2>
<h3>Operation Mode</h3>
```

### **ARIA Labels**

```html
<!-- Descriptive labels -->
<button aria-label="Start system operation">
  <i data-lucide="play"></i>
</button>

<button aria-label="Emergency stop - stops all operations immediately">
  <i data-lucide="alert-triangle"></i>
</button>

<!-- Form labels -->
<label for="speed-input">Speed Limit (m/s)</label>
<input 
  id="speed-input" 
  type="number" 
  aria-describedby="speed-help"
  min="0" 
  max="100" 
/>
<div id="speed-help">Enter speed limit between 0 and 100 meters per second</div>
```

### **Live Regions**

```html
<!-- Status updates -->
<div role="status" aria-live="polite" aria-atomic="true">
  <span id="system-status">System is idle</span>
</div>

<!-- Error messages -->
<div role="alert" aria-live="assertive">
  <span id="error-message">Connection failed</span>
</div>

<!-- Progress updates -->
<div role="progressbar" aria-valuenow="75" aria-valuemin="0" aria-valuemax="100">
  <span class="sr-only">Loading: 75% complete</span>
</div>
```

---

## 👆 **Touch Targets**

### **Minimum Touch Target Size**

```css
/* Minimum 44x44px touch targets */
.btn {
  min-width: 44px;
  min-height: 44px;
  padding: 12px 16px;
}

/* Touch-friendly spacing */
.touch-controls {
  display: flex;
  gap: 8px; /* Minimum spacing between touch targets */
}

/* Larger touch targets for critical actions */
.emergency-stop {
  min-width: 60px;
  min-height: 60px;
  padding: 16px;
}
```

### **Touch Feedback**

```css
/* Visual feedback for touch interactions */
.btn:active {
  transform: scale(0.95);
  transition: transform 0.1s;
}

/* Hover states for desktop */
@media (hover: hover) {
  .btn:hover {
    background-color: var(--accent);
  }
}
```

---

## 📱 **Mobile Accessibility**

### **Viewport Configuration**

```html
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=5.0">
```

### **Touch Gestures**

```javascript
// Support for touch gestures
const touchHandler = {
  onTouchStart: (e) => {
    // Handle touch start
  },
  onTouchMove: (e) => {
    // Handle touch move
  },
  onTouchEnd: (e) => {
    // Handle touch end
  }
};

// Avoid gesture conflicts
document.addEventListener('touchstart', (e) => {
  if (e.touches.length > 1) {
    e.preventDefault(); // Prevent zoom on double tap
  }
}, { passive: false });
```

### **Mobile Navigation**

```html
<!-- Collapsible navigation for mobile -->
<nav class="mobile-nav" aria-label="Main navigation">
  <button 
    class="nav-toggle" 
    aria-expanded="false"
    aria-controls="nav-menu"
  >
    <span class="sr-only">Toggle navigation menu</span>
    <i data-lucide="menu"></i>
  </button>
  
  <ul id="nav-menu" class="nav-menu" hidden>
    <li><a href="/overview">Tổng quan</a></li>
    <li><a href="/vax-charts">Đồ thị v‑a‑x</a></li>
    <li><a href="/logs">Nhật ký</a></li>
  </ul>
</nav>
```

---

## 🧪 **Testing & Validation**

### **Manual Testing Checklist**

- [ ] **Keyboard Navigation**
  - [ ] Tab through all interactive elements
  - [ ] Use Enter/Space to activate buttons
  - [ ] Use Escape to close modals
  - [ ] Check logical tab order

- [ ] **Screen Reader Testing**
  - [ ] Test with NVDA (Windows)
  - [ ] Test with VoiceOver (macOS)
  - [ ] Verify all content is announced
  - [ ] Check form labels and descriptions

- [ ] **Visual Testing**
  - [ ] Test with high contrast mode
  - [ ] Test with color blindness simulators
  - [ ] Verify focus indicators are visible
  - [ ] Check text size scaling

### **Automated Testing**

```javascript
// Jest accessibility testing
import { axe, toHaveNoViolations } from 'jest-axe';

expect.extend(toHaveNoViolations);

test('should not have accessibility violations', async () => {
  const { container } = render(<MyComponent />);
  const results = await axe(container);
  expect(results).toHaveNoViolations();
});
```

### **Lighthouse Accessibility Audit**

```bash
# Run Lighthouse accessibility audit
npx lighthouse https://localhost:3000 --only-categories=accessibility --output=json
```

---

## 🔧 **Implementation Examples**

### **Accessible Button Component**

```jsx
interface AccessibleButtonProps {
  children: React.ReactNode;
  onClick: () => void;
  disabled?: boolean;
  loading?: boolean;
  'aria-label'?: string;
  'aria-describedby'?: string;
}

const AccessibleButton: React.FC<AccessibleButtonProps> = ({
  children,
  onClick,
  disabled = false,
  loading = false,
  'aria-label': ariaLabel,
  'aria-describedby': ariaDescribedBy,
  ...props
}) => {
  return (
    <button
      onClick={onClick}
      disabled={disabled || loading}
      aria-label={ariaLabel}
      aria-describedby={ariaDescribedBy}
      aria-busy={loading}
      className={cn(
        'btn',
        disabled && 'btn-disabled',
        loading && 'btn-loading'
      )}
      {...props}
    >
      {loading && <span className="sr-only">Loading</span>}
      {children}
    </button>
  );
};
```

### **Accessible Form Component**

```jsx
interface AccessibleFormFieldProps {
  label: string;
  id: string;
  error?: string;
  required?: boolean;
  children: React.ReactNode;
}

const AccessibleFormField: React.FC<AccessibleFormFieldProps> = ({
  label,
  id,
  error,
  required = false,
  children
}) => {
  return (
    <div className="form-field">
      <label htmlFor={id} className="form-label">
        {label}
        {required && <span aria-label="required">*</span>}
      </label>
      {children}
      {error && (
        <div 
          id={`${id}-error`} 
          className="form-error" 
          role="alert"
        >
          {error}
        </div>
      )}
    </div>
  );
};
```

### **Accessible Status Component**

```jsx
interface StatusIndicatorProps {
  state: 'idle' | 'move' | 'dock' | 'fault' | 'estop';
  label: string;
}

const StatusIndicator: React.FC<StatusIndicatorProps> = ({ state, label }) => {
  const getStateDescription = (state: string) => {
    const descriptions = {
      idle: 'System is ready and waiting for commands',
      move: 'System is currently in motion',
      dock: 'System is performing docking operation',
      fault: 'System has encountered an error',
      estop: 'Emergency stop is active - system is halted'
    };
    return descriptions[state] || '';
  };

  return (
    <div 
      className={`status-indicator status-${state}`}
      role="status"
      aria-live="polite"
      aria-label={`System status: ${label}`}
    >
      <span className="status-icon" aria-hidden="true">
        {state === 'idle' && '●'}
        {state === 'move' && '▶'}
        {state === 'dock' && '⚡'}
        {state === 'fault' && '⚠'}
        {state === 'estop' && '🛑'}
      </span>
      <span className="status-text">{label}</span>
      <span className="sr-only">{getStateDescription(state)}</span>
    </div>
  );
};
```

---

## 📚 **Resources & Tools**

### **Testing Tools**

- **axe DevTools:** Browser extension for accessibility testing
- **Lighthouse:** Automated accessibility auditing
- **WAVE:** Web accessibility evaluation tool
- **Color Contrast Analyzer:** Color contrast testing

### **Screen Readers**

- **NVDA:** Free screen reader for Windows
- **VoiceOver:** Built-in screen reader for macOS
- **JAWS:** Commercial screen reader for Windows
- **TalkBack:** Built-in screen reader for Android

### **Browser Developer Tools**

```javascript
// Chrome DevTools accessibility features
// 1. Elements tab → Accessibility panel
// 2. Lighthouse tab → Accessibility audit
// 3. Console → axe-core integration

// Firefox DevTools
// 1. Accessibility panel
// 2. Color contrast checker
// 3. ARIA inspector
```

### **Documentation**

- [WCAG 2.1 Guidelines](https://www.w3.org/WAI/WCAG21/quickref/)
- [ARIA Authoring Practices](https://www.w3.org/WAI/ARIA/apg/)
- [Web Accessibility Initiative](https://www.w3.org/WAI/)

---

## 📋 **Checklist**

### **Development Checklist**

- [ ] **Semantic HTML**
  - [ ] Use proper heading hierarchy (h1-h6)
  - [ ] Use semantic elements (main, section, article, etc.)
  - [ ] Provide alt text for images
  - [ ] Use proper form labels

- [ ] **Keyboard Navigation**
  - [ ] All interactive elements are keyboard accessible
  - [ ] Logical tab order
  - [ ] Visible focus indicators
  - [ ] Skip links for main content

- [ ] **Screen Reader Support**
  - [ ] ARIA labels and descriptions
  - [ ] Live regions for dynamic content
  - [ ] Proper roles and states
  - [ ] Semantic markup

- [ ] **Visual Design**
  - [ ] Color contrast meets WCAG AA standards
  - [ ] Don't rely solely on color for information
  - [ ] Text can be resized up to 200%
  - [ ] Touch targets are at least 44x44px

### **Testing Checklist**

- [ ] **Manual Testing**
  - [ ] Test with keyboard only
  - [ ] Test with screen reader
  - [ ] Test with high contrast mode
  - [ ] Test on mobile devices

- [ ] **Automated Testing**
  - [ ] Run accessibility linting
  - [ ] Run automated accessibility tests
  - [ ] Check Lighthouse accessibility score
  - [ ] Validate ARIA markup

---

**Changelog v1.0:**
- ✅ Created comprehensive accessibility guidelines
- ✅ Defined WCAG 2.1 AA requirements
- ✅ Added keyboard navigation guidelines
- ✅ Included screen reader support
- ✅ Added touch target requirements
- ✅ Created testing checklists
- ✅ Added implementation examples