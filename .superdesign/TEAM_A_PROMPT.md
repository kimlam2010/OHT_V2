# 🎨 **TEAM A PROMPT - OHT-50 UI/UX Implementation**

**Ngày tạo:** 2025-01-28  
**Team:** Designer A  
**Trạng thái:** 🔄 IN PROGRESS - Layout Structure Framework  
**Phạm vi:** Phase 1 - Foundation & Core Pages

---

## 📋 **YOUR ASSIGNED TASKS**

### **Phase 1 - Foundation (Week 1-2) 🔴**

#### **P1-T2: Layout Structure Framework (CRITICAL)**
- **Due Date:** 2025-02-18
- **Priority:** 🔴 Critical
- **Effort:** 7 days

**Requirements:**
- Implement 12-column grid system cho tất cả pages
- Create responsive breakpoints definition
- Build header/footer structure templates
- Develop layout testing framework
- Ensure cross-browser compatibility
- **Navigation System Integration** theo REQ_UI_UX_Information_Architecture.md
- **Top Navigation Bar** với primary navigation
- **Sidebar Navigation** với secondary navigation
- **Breadcrumbs** cho current location
- **Emergency Navigation** cho quick access to safety controls

**Deliverables:**
- [ ] `layout_framework.html` - Demo page với 12-column grid
- [ ] `responsive_breakpoints.html` - Test page cho mobile/tablet/desktop
- [ ] `header_footer_templates.html` - Template cho header/footer
- [ ] Layout testing documentation

#### **P1-T4: Accessibility Framework (CRITICAL)**
- **Due Date:** 2025-03-04
- **Priority:** 🔴 Critical
- **Effort:** 5 days

**Requirements:**
- Implement global accessibility CSS framework
- Create focus management system
- Build screen reader support
- Add high contrast mode support
- Develop keyboard navigation framework

**Deliverables:**
- [ ] `accessibility_framework.html` - Demo page với accessibility features
- [ ] `focus_management.html` - Test page cho keyboard navigation
- [ ] `screen_reader_test.html` - Test page cho screen readers
- [ ] Accessibility testing documentation

### **Phase 2 - Core Pages (Week 3-4) 🟡**

#### **P2-T2: Dashboard Main Interface**
- **Due Date:** 2025-03-18
- **Priority:** 🟡 High
- **Effort:** 7 days

**Requirements:**
- Real-time status với emergency controls
- 12-column grid layout
- Industrial theme implementation
- Safety-first design
- Responsive design

**Deliverables:**
- [ ] `2-1-dashboard-main.html` - Main dashboard interface
- [ ] Real-time status indicators
- [ ] Emergency controls integration
- [ ] Responsive layout testing

#### **P2-T4: Map & Navigation Interface**
- **Due Date:** 2025-04-01
- **Priority:** 🟡 High
- **Effort:** 7 days

**Requirements:**
- LiDAR visualization với navigation
- Interactive map controls
- Safety zone indicators
- Path planning interface
- Real-time position tracking

**Deliverables:**
- [ ] `4-1-map-interface.html` - Map & navigation interface
- [ ] Interactive map controls
- [ ] Safety zone visualization
- [ ] Path planning interface

---

## 🎯 **DESIGN REQUIREMENTS**

### **Layout Structure Standards:**
```css
/* 12-Column Grid System for all pages */
.page-container {
  display: grid;
  grid-template-columns: repeat(12, 1fr);
  grid-template-rows: auto 1fr auto;
  gap: var(--space-lg);
  height: 100vh;
  padding: var(--space-lg);
  background: var(--bg-secondary);
}

/* Header Section - Always visible */
.page-header {
  grid-column: 1 / -1;
  grid-row: 1;
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md) var(--space-lg);
  background: var(--bg-primary);
  border-radius: var(--radius-lg);
  box-shadow: var(--shadow-sm);
  border: 1px solid var(--industrial-gray-300);
}

/* Main Content Area */
.page-main {
  grid-column: 1 / -1;
  grid-row: 2;
  display: grid;
  grid-template-columns: 3fr 1fr;
  gap: var(--space-lg);
  overflow: hidden;
}

/* Footer Section */
.page-footer {
  grid-column: 1 / -1;
  grid-row: 3;
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md) var(--space-lg);
  background: var(--bg-primary);
  border-radius: var(--radius-lg);
  box-shadow: var(--shadow-sm);
  border: 1px solid var(--industrial-gray-300);
}
```

### **Accessibility Standards:**
```css
/* Global Accessibility Framework */
*:focus-visible {
  outline: 2px solid var(--primary-orange);
  outline-offset: 2px;
  border-radius: var(--radius-sm);
}

@media (prefers-reduced-motion: reduce) {
  * {
    animation-duration: 0.01ms !important;
    transition-duration: 0.01ms !important;
  }
}

/* Skip links for all pages */
.skip-link {
  position: absolute;
  top: -40px;
  left: 6px;
  background: var(--primary-orange);
  color: white;
  padding: var(--space-sm) var(--space-md);
  text-decoration: none;
  border-radius: var(--radius-sm);
  z-index: 10000;
  font-weight: 600;
}

.skip-link:focus {
  top: 6px;
}
```

### **Safety-First Design:**
- **Emergency controls** luôn visible và accessible
- **E-Stop button** trên mọi page (theo REQ_UI_UX_Alert_System.md)
- **Safety colors** cho critical information (Red: #DC2626, Orange: #EA580C, Yellow: #CA8A04, Green: #16A34A)
- **Clear visual hierarchy** cho safety information
- **Alert System Integration** theo REQ_UI_UX_Alert_System.md
- **Emergency Stop Response** < 100ms theo safety requirements

---

## 📁 **FILE STRUCTURE TO CREATE**

```
.superdesign/design_iterations/foundation/
├── layout_framework.html              # P1-T2 Deliverable
├── responsive_breakpoints.html        # P1-T2 Deliverable
├── header_footer_templates.html       # P1-T2 Deliverable
├── accessibility_framework.html       # P1-T4 Deliverable
├── focus_management.html              # P1-T4 Deliverable
└── screen_reader_test.html            # P1-T4 Deliverable

.superdesign/design_iterations/core_pages/
├── 2-1-dashboard-main.html            # P2-T2 Deliverable
└── 4-1-map-interface.html             # P2-T4 Deliverable
```

---

## 🎨 **DESIGN GUIDELINES**

### **Industrial Theme:**
- **Primary Color:** Orange (#FF6B35)
- **Background:** Light grays (#F9FAFB, #F3F4F6)
- **Text:** Dark grays (#374151, #111827)
- **Safety Colors:** Red (#DC2626), Orange (#EA580C), Yellow (#CA8A04), Green (#16A34A)

### **Typography:**
- **Sans Serif:** Inter (body text)
- **Monospace:** JetBrains Mono (code, data)
- **Display:** Space Grotesk (headings)

### **Spacing:**
- **Base Unit:** 8px
- **Scale:** XS(4px), SM(8px), MD(16px), LG(24px), XL(32px), 2XL(48px)

### **Components:**
- Use existing components từ `theme_components.css`
- Follow BEM methodology cho CSS classes
- Implement proper ARIA labels
- Ensure keyboard navigation

---

## 🚨 **CRITICAL REQUIREMENTS**

### **Safety Features:**
1. **E-Stop Button** trên mọi page
2. **Emergency Contact** link
3. **Safety Status Indicators**
4. **Clear Warning Messages**

### **Accessibility Features:**
1. **WCAG 2.1 AA Compliance** (theo REQ_UI_UX_Accessibility.md)
2. **Keyboard Navigation** với focus indicators
3. **Screen Reader Support** với ARIA labels
4. **High Contrast Mode** support
5. **Focus Management** system
6. **Touch Target Sizes** minimum 44px
7. **Reduced Motion** preferences support
8. **Semantic HTML** structure

### **Responsive Design:**
1. **Mobile-first approach** theo REQ_UI_UX_Visual_Design.md
2. **Breakpoints:** 640px, 768px, 1024px, 1280px, 1536px
3. **Touch-friendly targets** (min 44px)
4. **Readable text** trên mọi device
5. **Industrial Theme** với orange color scheme (#FF6B35)
6. **Outdoor Visibility** cho môi trường ngoài trời
7. **12-Column Grid System** cho tất cả layouts

---

## 📊 **ACCEPTANCE CRITERIA**

### **Layout Framework:**
- [ ] 12-column grid system implemented
- [ ] Responsive breakpoints working
- [ ] Header/footer consistent across pages
- [ ] Cross-browser compatibility tested

### **Accessibility Framework:**
- [ ] Focus indicators visible
- [ ] Keyboard navigation functional
- [ ] Screen reader content accessible
- [ ] High contrast mode supported

### **Dashboard Interface:**
- [ ] Real-time status indicators
- [ ] Emergency controls integrated
- [ ] Industrial theme applied
- [ ] Responsive layout tested

### **Map Interface:**
- [ ] Interactive map controls
- [ ] Safety zone visualization
- [ ] Path planning interface
- [ ] Real-time position tracking

---

## 🔧 **TECHNICAL REQUIREMENTS**

### **HTML Structure:**
```html
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Page Title - OHT-50</title>
    
    <!-- Google Fonts -->
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700;800&family=JetBrains+Mono:wght@400;500;600;700&family=Space+Grotesk:wght@300;400;500;600;700&display=swap" rel="stylesheet">
    
    <!-- Lucide Icons -->
    <script src="https://unpkg.com/lucide@latest/dist/umd/lucide.min.js"></script>
    
    <!-- Tailwind CSS -->
    <script src="https://cdn.tailwindcss.com"></script>
    
    <!-- OHT-50 Theme -->
    <link rel="stylesheet" href="../../themes/theme_oht50_unified.css">
    <link rel="stylesheet" href="../../themes/theme_components.css">
</head>
<body>
    <!-- Skip Link -->
    <a href="#main-content" class="skip-link">Skip to main content</a>
    
    <!-- Header -->
    <header class="page-header">
        <!-- Header content -->
    </header>
    
    <!-- Main Content -->
    <main id="main-content" class="page-main">
        <!-- Main content -->
    </main>
    
    <!-- Footer -->
    <footer class="page-footer">
        <!-- Footer content -->
    </footer>
    
    <script>
        // Initialize Lucide icons
        lucide.createIcons();
    </script>
</body>
</html>
```

### **CSS Guidelines:**
- Use CSS custom properties từ theme
- Follow BEM methodology
- Implement responsive design
- Ensure accessibility compliance

---

## 📝 **WORKFLOW**

### **Step 1: Foundation (Week 1-2)**
1. **Day 1-3:** Layout Structure Framework
2. **Day 4-5:** Accessibility Framework
3. **Day 6-7:** Testing và documentation

### **Step 2: Core Pages (Week 3-4)**
1. **Day 1-3:** Dashboard Main Interface
2. **Day 4-7:** Map & Navigation Interface

### **Step 3: Testing & Validation**
1. **Cross-browser testing**
2. **Accessibility testing**
3. **Responsive testing**
4. **Documentation update**

---

## 🎯 **SUCCESS METRICS**

### **Layout Metrics:**
- **Grid Implementation:** 100% pages use 12-column grid
- **Responsive Coverage:** 100% pages work on all breakpoints
- **Layout Consistency:** 100% layout consistency across pages
- **Cross-browser Compatibility:** 100% browser support

### **Accessibility Metrics:**
- **WCAG Compliance:** 100% WCAG 2.1 AA compliance
- **Screen Reader Support:** 100% content accessible via screen readers
- **Keyboard Navigation:** 100% functionality accessible via keyboard
- **Focus Management:** 100% interactive elements have focus indicators

### **Design Metrics:**
- **Theme Compliance:** 100% industrial theme implementation
- **Safety Features:** 100% emergency controls implemented
- **Component Usage:** 90% components from design system
- **Visual Consistency:** 100% visual consistency across interface

---

## 📞 **SUPPORT & RESOURCES**

### **Available Resources:**
- **Design System:** `theme_oht50_unified.css`
- **Component Library:** `theme_components.css`
- **Foundation Demo:** `design_system.html`
- **Roadmap:** `DESIGNER_ROADMAP_PLAN.md`

### **Contact:**
- **Team Lead:** For technical questions
- **Design System:** For component questions
- **Accessibility:** For WCAG compliance questions

---

## 🚀 **START WORKING**

**Next Steps:**
1. **Review** design system và component library
2. **Start** với P1-T2: Layout Structure Framework
3. **Create** `layout_framework.html` demo page
4. **Test** responsive breakpoints
5. **Document** progress và issues

**Remember:** Safety-first design, industrial theme, accessibility compliance!

---

**📅 Last Updated:** 2025-01-28  
**👤 Team:** Designer A  
**🎯 Status:** Ready to Start - Foundation Phase
