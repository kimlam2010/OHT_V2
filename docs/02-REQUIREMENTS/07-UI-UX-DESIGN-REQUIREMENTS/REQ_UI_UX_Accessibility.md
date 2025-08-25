# ♿ REQ_UI_UX_Accessibility - Khả năng tiếp cận

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Accessibility Specialists, Product Managers

---

## 🎯 **Mục tiêu Accessibility:**

### **Concept chính:**
- **System:** Comprehensive accessibility framework cho robot OHT-50
- **Focus:** Inclusive design cho mọi người dùng, kể cả người khuyết tật
- **Style:** Industrial theme với accessibility-first approach
- **Target:** Quản lý và Kỹ sư kỹ thuật cao với diverse abilities

### **Nguyên tắc thiết kế:**
1. **Universal Design:** Thiết kế cho tất cả người dùng từ đầu
2. **WCAG 2.1 AA Compliance:** Tuân thủ tiêu chuẩn quốc tế
3. **Safety Priority:** Đảm bảo an toàn cho mọi người dùng
4. **Clear Communication:** Thông tin rõ ràng qua nhiều kênh

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| Visual Accessibility | Khả năng tiếp cận thị giác | Designers, Developers | Visual Design | ✅ Hoàn thành |
| Motor Accessibility | Khả năng tiếp cận vận động | Designers, Developers | Interaction Design | ✅ Hoàn thành |
| Cognitive Accessibility | Khả năng tiếp cận nhận thức | Designers, Developers | User Experience | ✅ Hoàn thành |
| Assistive Technology | Công nghệ hỗ trợ | Designers, Developers | Technical Implementation | ✅ Hoàn thành |

---

## 👁️ **1. VISUAL ACCESSIBILITY - Khả năng tiếp cận thị giác**

### **Color Contrast Requirements:**
```html
<!-- Color Contrast Standards -->
<div class="accessibility-standards">
  <div class="contrast-requirements">
    <h3>🎨 Color Contrast Standards</h3>
    
    <div class="contrast-grid">
      <div class="contrast-item normal">
        <div class="contrast-preview">
          <span class="text-sample">Normal Text</span>
          <span class="contrast-ratio">4.5:1</span>
        </div>
        <p>Normal text must have 4.5:1 contrast ratio</p>
      </div>
      
      <div class="contrast-item large">
        <div class="contrast-preview">
          <span class="text-sample large-text">Large Text</span>
          <span class="contrast-ratio">3:1</span>
        </div>
        <p>Large text (18pt+) must have 3:1 contrast ratio</p>
      </div>
      
      <div class="contrast-item ui">
        <div class="contrast-preview">
          <span class="text-sample">UI Elements</span>
          <span class="contrast-ratio">3:1</span>
        </div>
        <p>UI elements must have 3:1 contrast ratio</p>
      </div>
    </div>
  </div>
</div>
```

### **Color Accessibility CSS:**
```css
/* Accessibility Standards */
.accessibility-standards {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.contrast-requirements h3 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-bold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

.contrast-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: var(--space-lg);
}

.contrast-item {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
}

.contrast-preview {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-sm);
  padding: var(--space-md);
  background: var(--bg-primary);
  border-radius: 0.5rem;
}

.text-sample {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

.text-sample.large-text {
  font-size: 1.25rem;
}

.contrast-ratio {
  background: var(--primary-orange);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.875rem;
  font-weight: var(--font-bold);
}

.contrast-item p {
  margin: 0;
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
  line-height: 1.4;
}

/* High Contrast Mode Support */
@media (prefers-contrast: high) {
  :root {
    --primary-orange: #ff6b35;
    --industrial-gray-700: #000000;
    --industrial-gray-600: #333333;
    --industrial-gray-500: #666666;
    --industrial-gray-300: #cccccc;
    --industrial-gray-200: #e6e6e6;
    --bg-primary: #ffffff;
    --bg-secondary: #f8f9fa;
  }
  
  .contrast-item {
    border-left-width: 6px;
  }
  
  .text-sample {
    font-weight: var(--font-bold);
  }
}

/* Reduced Motion Support */
@media (prefers-reduced-motion: reduce) {
  * {
    animation-duration: 0.01ms !important;
    animation-iteration-count: 1 !important;
    transition-duration: 0.01ms !important;
  }
  
  .alert-banner {
    animation: none;
  }
  
  .status-indicator {
    animation: none;
  }
}
```

### **Typography Accessibility:**
```css
/* Typography Accessibility */
.accessible-typography {
  /* Minimum font sizes */
  --font-size-minimum: 16px;
  --font-size-large: 18px;
  --font-size-extra-large: 24px;
  
  /* Line height for readability */
  --line-height-normal: 1.5;
  --line-height-relaxed: 1.75;
  
  /* Letter spacing for clarity */
  --letter-spacing-normal: 0.01em;
  --letter-spacing-wide: 0.05em;
}

/* Font scaling support */
html {
  font-size: 16px;
}

@media (min-width: 768px) {
  html {
    font-size: 18px;
  }
}

/* Focus indicators */
.focus-visible {
  outline: 3px solid var(--primary-orange);
  outline-offset: 2px;
  border-radius: 0.25rem;
}

/* Skip links */
.skip-link {
  position: absolute;
  top: -40px;
  left: 6px;
  background: var(--primary-orange);
  color: white;
  padding: 8px;
  text-decoration: none;
  border-radius: 0.25rem;
  z-index: 10000;
}

.skip-link:focus {
  top: 6px;
}
```

---

## 🖱️ **2. MOTOR ACCESSIBILITY - Khả năng tiếp cận vận động**

### **Keyboard Navigation:**
```html
<!-- Keyboard Navigation System -->
<div class="keyboard-navigation">
  <h3>⌨️ Keyboard Navigation</h3>
  
  <div class="nav-instructions">
    <div class="instruction-item">
      <kbd>Tab</kbd>
      <span>Navigate between interactive elements</span>
    </div>
    
    <div class="instruction-item">
      <kbd>Enter</kbd> / <kbd>Space</kbd>
      <span>Activate buttons and controls</span>
    </div>
    
    <div class="instruction-item">
      <kbd>Arrow Keys</kbd>
      <span>Navigate within components</span>
    </div>
    
    <div class="instruction-item">
      <kbd>Escape</kbd>
      <span>Close dialogs and menus</span>
    </div>
    
    <div class="instruction-item">
      <kbd>Ctrl + M</kbd>
      <span>Emergency stop (global shortcut)</span>
    </div>
  </div>
  
  <div class="focus-management">
    <h4>Focus Management</h4>
    <div class="focus-indicators">
      <div class="indicator-item">
        <div class="focus-demo focused">Focused Element</div>
        <span>Clear orange outline</span>
      </div>
      
      <div class="indicator-item">
        <div class="focus-demo keyboard">Keyboard Focus</div>
        <span>Enhanced visibility</span>
      </div>
    </div>
  </div>
</div>
```

### **Motor Accessibility CSS:**
```css
/* Keyboard Navigation */
.keyboard-navigation {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.nav-instructions {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: var(--space-md);
  margin-bottom: var(--space-lg);
}

.instruction-item {
  display: flex;
  align-items: center;
  gap: var(--space-md);
  padding: var(--space-md);
  background: var(--bg-secondary);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
}

kbd {
  background: var(--industrial-gray-200);
  border: 1px solid var(--industrial-gray-400);
  border-radius: 0.25rem;
  padding: var(--space-xs) var(--space-sm);
  font-family: var(--font-mono);
  font-size: 0.875rem;
  font-weight: var(--font-bold);
  color: var(--industrial-gray-700);
  min-width: 60px;
  text-align: center;
}

.instruction-item span {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
}

/* Focus Management */
.focus-management h4 {
  margin-bottom: var(--space-md);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.focus-indicators {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: var(--space-md);
}

.indicator-item {
  text-align: center;
}

.focus-demo {
  padding: var(--space-md);
  margin-bottom: var(--space-sm);
  border-radius: 0.5rem;
  font-weight: var(--font-semibold);
  background: var(--bg-secondary);
  border: 2px solid transparent;
}

.focus-demo.focused {
  border-color: var(--primary-orange);
  box-shadow: 0 0 0 2px rgba(255, 107, 53, 0.2);
}

.focus-demo.keyboard {
  border-color: var(--primary-orange);
  box-shadow: 0 0 0 4px rgba(255, 107, 53, 0.3);
  background: rgba(255, 107, 53, 0.1);
}

.indicator-item span {
  font-size: 0.75rem;
  color: var(--industrial-gray-500);
}

/* Touch Target Sizes */
.touch-target {
  min-width: 44px;
  min-height: 44px;
  padding: var(--space-sm);
}

/* Large touch targets for critical controls */
.emergency-control {
  min-width: 60px;
  min-height: 60px;
  padding: var(--space-md);
}

/* Hover states for mouse users */
@media (hover: hover) {
  .interactive-element:hover {
    background: rgba(255, 107, 53, 0.1);
    transform: translateY(-1px);
  }
}

/* Disable hover effects for touch devices */
@media (hover: none) {
  .interactive-element:hover {
    background: inherit;
    transform: none;
  }
}
```

---

## 🧠 **3. COGNITIVE ACCESSIBILITY - Khả năng tiếp cận nhận thức**

### **Clear Information Architecture:**
```html
<!-- Cognitive Accessibility Features -->
<div class="cognitive-accessibility">
  <h3>🧠 Cognitive Accessibility</h3>
  
  <div class="cognitive-features">
    <div class="feature-item">
      <h4>📋 Clear Information Hierarchy</h4>
      <div class="hierarchy-example">
        <div class="info-level primary">
          <span class="level-label">Primary</span>
          <span class="level-desc">Most important information</span>
        </div>
        <div class="info-level secondary">
          <span class="level-label">Secondary</span>
          <span class="level-desc">Supporting details</span>
        </div>
        <div class="info-level tertiary">
          <span class="level-label">Tertiary</span>
          <span class="level-desc">Additional context</span>
        </div>
      </div>
    </div>
    
    <div class="feature-item">
      <h4>🎯 Consistent Patterns</h4>
      <div class="pattern-examples">
        <div class="pattern-item">
          <div class="pattern-icon">🚨</div>
          <span>Emergency controls always red</span>
        </div>
        <div class="pattern-item">
          <div class="pattern-icon">✅</div>
          <span>Success states always green</span>
        </div>
        <div class="pattern-item">
          <div class="pattern-icon">⚠️</div>
          <span>Warnings always yellow</span>
        </div>
      </div>
    </div>
    
    <div class="feature-item">
      <h4>⏱️ Time Management</h4>
      <div class="time-features">
        <div class="time-item">
          <span class="time-label">Auto-dismiss</span>
          <span class="time-value">10 seconds</span>
        </div>
        <div class="time-item">
          <span class="time-label">Session timeout</span>
          <span class="time-value">30 minutes</span>
        </div>
        <div class="time-item">
          <span class="time-label">Confirmation delay</span>
          <span class="time-value">3 seconds</span>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Cognitive Accessibility CSS:**
```css
/* Cognitive Accessibility */
.cognitive-accessibility {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.cognitive-features {
  display: grid;
  gap: var(--space-xl);
}

.feature-item h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

/* Information Hierarchy */
.hierarchy-example {
  display: flex;
  flex-direction: column;
  gap: var(--space-md);
}

.info-level {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  border-radius: 0.5rem;
  border-left: 4px solid;
}

.info-level.primary {
  background: rgba(255, 107, 53, 0.1);
  border-left-color: var(--primary-orange);
}

.info-level.secondary {
  background: rgba(108, 117, 125, 0.1);
  border-left-color: var(--industrial-gray-500);
}

.info-level.tertiary {
  background: rgba(255, 193, 7, 0.1);
  border-left-color: var(--warning-yellow);
}

.level-label {
  font-weight: var(--font-bold);
  color: var(--industrial-gray-700);
}

.level-desc {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
}

/* Consistent Patterns */
.pattern-examples {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: var(--space-md);
}

.pattern-item {
  display: flex;
  align-items: center;
  gap: var(--space-md);
  padding: var(--space-md);
  background: var(--bg-secondary);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
}

.pattern-icon {
  font-size: 1.5rem;
  flex-shrink: 0;
}

.pattern-item span {
  font-size: 0.875rem;
  color: var(--industrial-gray-700);
  font-weight: var(--font-medium);
}

/* Time Management */
.time-features {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
  gap: var(--space-md);
}

.time-item {
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-secondary);
  border-radius: 0.5rem;
  text-align: center;
}

.time-label {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
  margin-bottom: var(--space-xs);
}

.time-value {
  font-size: 1.125rem;
  font-weight: var(--font-bold);
  color: var(--primary-orange);
}

/* Error Prevention */
.error-prevention {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-top: var(--space-lg);
}

.error-prevention h4 {
  margin-bottom: var(--space-md);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.confirmation-dialog {
  background: var(--bg-primary);
  border: 2px solid var(--error-red);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin: var(--space-md) 0;
}

.confirmation-dialog h5 {
  color: var(--error-red);
  margin-bottom: var(--space-md);
  font-weight: var(--font-bold);
}

.confirmation-actions {
  display: flex;
  gap: var(--space-md);
  justify-content: flex-end;
}

.confirmation-btn {
  padding: var(--space-sm) var(--space-lg);
  border-radius: 0.5rem;
  font-weight: var(--font-semibold);
  cursor: pointer;
  transition: all 0.2s ease;
}

.confirmation-btn.danger {
  background: var(--error-red);
  color: white;
  border: none;
}

.confirmation-btn.cancel {
  background: var(--bg-secondary);
  color: var(--industrial-gray-700);
  border: 1px solid var(--industrial-gray-300);
}
```

---

## 🔧 **4. ASSISTIVE TECHNOLOGY - Công nghệ hỗ trợ**

### **Screen Reader Support:**
```html
<!-- Assistive Technology Support -->
<div class="assistive-technology">
  <h3>🔧 Assistive Technology Support</h3>
  
  <div class="at-features">
    <div class="at-feature">
      <h4>📢 Screen Reader Support</h4>
      <div class="sr-examples">
        <div class="sr-item">
          <code>aria-label="Emergency Stop Button"</code>
          <span>Clear button descriptions</span>
        </div>
        <div class="sr-item">
          <code>aria-live="polite"</code>
          <span>Status updates announced</span>
        </div>
        <div class="sr-item">
          <code>aria-expanded="true"</code>
          <span>Menu state indicated</span>
        </div>
        <div class="sr-item">
          <code>role="alert"</code>
          <span>Critical alerts prioritized</span>
        </div>
      </div>
    </div>
    
    <div class="at-feature">
      <h4>🎯 Semantic HTML</h4>
      <div class="semantic-examples">
        <div class="semantic-item">
          <code>&lt;button&gt;</code>
          <span>Interactive buttons</span>
        </div>
        <div class="semantic-item">
          <code>&lt;nav&gt;</code>
          <span>Navigation sections</span>
        </div>
        <div class="semantic-item">
          <code>&lt;main&gt;</code>
          <span>Primary content area</span>
        </div>
        <div class="semantic-item">
          <code>&lt;section&gt;</code>
          <span>Content sections</span>
        </div>
      </div>
    </div>
    
    <div class="at-feature">
      <h4>🔊 Audio Descriptions</h4>
      <div class="audio-features">
        <div class="audio-item">
          <span class="audio-icon">🔊</span>
          <span>Status announcements</span>
        </div>
        <div class="audio-item">
          <span class="audio-icon">🎵</span>
          <span>Alert sounds</span>
        </div>
        <div class="audio-item">
          <span class="audio-icon">📢</span>
          <span>Voice feedback</span>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Assistive Technology CSS:**
```css
/* Assistive Technology Support */
.assistive-technology {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.at-features {
  display: grid;
  gap: var(--space-xl);
}

.at-feature h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

/* Screen Reader Examples */
.sr-examples {
  display: grid;
  gap: var(--space-md);
}

.sr-item {
  display: flex;
  flex-direction: column;
  gap: var(--space-xs);
  padding: var(--space-md);
  background: var(--bg-secondary);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
}

.sr-item code {
  background: var(--bg-primary);
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-family: var(--font-mono);
  font-size: 0.875rem;
  color: var(--primary-orange);
  border: 1px solid var(--industrial-gray-300);
}

.sr-item span {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
}

/* Semantic HTML Examples */
.semantic-examples {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: var(--space-md);
}

.semantic-item {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: var(--space-sm);
  padding: var(--space-md);
  background: var(--bg-secondary);
  border-radius: 0.5rem;
  text-align: center;
}

.semantic-item code {
  background: var(--bg-primary);
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-family: var(--font-mono);
  font-size: 0.875rem;
  color: var(--primary-orange);
  border: 1px solid var(--industrial-gray-300);
}

.semantic-item span {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
}

/* Audio Features */
.audio-features {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
  gap: var(--space-md);
}

.audio-item {
  display: flex;
  align-items: center;
  gap: var(--space-md);
  padding: var(--space-md);
  background: var(--bg-secondary);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
}

.audio-icon {
  font-size: 1.5rem;
  flex-shrink: 0;
}

.audio-item span:last-child {
  font-size: 0.875rem;
  color: var(--industrial-gray-700);
  font-weight: var(--font-medium);
}

/* Screen Reader Only Content */
.sr-only {
  position: absolute;
  width: 1px;
  height: 1px;
  padding: 0;
  margin: -1px;
  overflow: hidden;
  clip: rect(0, 0, 0, 0);
  white-space: nowrap;
  border: 0;
}

/* Focus Indicators for Screen Readers */
.focus-indicator {
  position: relative;
}

.focus-indicator::after {
  content: '';
  position: absolute;
  top: -2px;
  left: -2px;
  right: -2px;
  bottom: -2px;
  border: 2px solid var(--primary-orange);
  border-radius: 0.5rem;
  opacity: 0;
  transition: opacity 0.2s ease;
}

.focus-indicator:focus::after {
  opacity: 1;
}

/* High Contrast Mode Enhancements */
@media (prefers-contrast: high) {
  .sr-item,
  .semantic-item,
  .audio-item {
    border-left-width: 6px;
  }
  
  code {
    background: var(--industrial-gray-200);
    border-color: var(--industrial-gray-700);
  }
}
```

---

## 📚 **Tham chiếu:**

- [Visual Design](./REQ_UI_UX_Visual_Design.md) - Thiết kế hình ảnh
- [Interaction Design](./REQ_UI_UX_Interaction_Design.md) - Thiết kế tương tác
- [Alert System](./REQ_UI_UX_Alert_System.md) - Hệ thống cảnh báo
- [Voice Control](./REQ_UI_UX_Voice_Control.md) - Điều khiển bằng giọng nói

---

**Changelog v1.0:**
- ✅ Created comprehensive accessibility framework
- ✅ Defined visual accessibility standards (WCAG 2.1 AA)
- ✅ Implemented motor accessibility features
- ✅ Added cognitive accessibility considerations
- ✅ Designed assistive technology support
- ✅ Included high contrast mode support
- ✅ Added reduced motion preferences
- ✅ Created keyboard navigation system
- ✅ Implemented screen reader support
- ✅ Added semantic HTML guidelines
- ✅ Created focus management system
- ✅ Added error prevention mechanisms
- ✅ Included touch target sizing guidelines

**🚨 Lưu ý:** Tài liệu này tuân thủ WCAG 2.1 AA standards và đảm bảo accessibility cho mọi người dùng, đặc biệt quan trọng cho môi trường công nghiệp với robot OHT-50.
