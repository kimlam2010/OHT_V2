# 🎨 **TEAM B PROMPT - OHT-50 UI/UX Implementation**

**Ngày tạo:** 2025-01-28  
**Team:** Designer B  
**Trạng thái:** 🔄 IN PROGRESS - Unified Design System Creation  
**Phạm vi:** Phase 1 - Foundation & Core Pages

---

## 📋 **YOUR ASSIGNED TASKS**

### **Phase 1 - Foundation (Week 1-2) 🔴**

#### **P1-T1: Unified Design System Creation (CRITICAL)**
- **Due Date:** 2025-02-11
- **Priority:** 🔴 Critical
- **Effort:** 7 days

**Requirements:**
- Create complete design tokens system
- Develop color palette documentation
- Build typography guidelines
- Define spacing and layout rules
- Establish component design principles

**Deliverables:**
- [ ] `design_system_guide.md` - Complete design system documentation
- [ ] `color_palette_demo.html` - Interactive color palette showcase
- [ ] `typography_guide.html` - Typography examples và guidelines
- [ ] `spacing_system.html` - Spacing scale demonstration
- [ ] Design system documentation

#### **P1-T3: Component Library Foundation (CRITICAL)**
- **Due Date:** 2025-02-25
- **Priority:** 🔴 Critical
- **Effort:** 7 days

**Requirements:**
- Build button component library theo REQ_UI_UX_Design_System.md
- Create card component system với consistent styling
- Develop form component library với validation
- Implement navigation components theo REQ_UI_UX_Information_Architecture.md
- Integrate icon system (Lucide Icons)
- **Emergency Components** (E-Stop buttons, alert banners)
- **Status Components** (indicators, progress bars)
- **Control Components** (sliders, toggles, buttons)
- **Data Display Components** (tables, charts, metrics)

**Deliverables:**
- [ ] `component_library.html` - Interactive component showcase
- [ ] `button_variants.html` - All button variants demo
- [ ] `card_system.html` - Card component system demo
- [ ] `form_components.html` - Form components demo
- [ ] `navigation_demo.html` - Navigation components demo

### **Phase 2 - Core Pages (Week 3-4) 🟡**

#### **P2-T1: Authentication & Login Page**
- **Due Date:** 2025-03-11
- **Priority:** 🟡 High
- **Effort:** 7 days

**Requirements:**
- Secure login với proper validation
- Industrial theme implementation
- Safety-first design
- Responsive design
- Accessibility compliance

**Deliverables:**
- [ ] `1-1-auth-login.html` - Authentication & login interface
- [ ] Form validation system
- [ ] Error handling interface
- [ ] Security indicators
- [ ] Responsive layout testing

#### **P2-T3: Control Panel Interface**
- **Due Date:** 2025-03-25
- **Priority:** 🟡 High
- **Effort:** 7 days

**Requirements:**
- Manual control với safety features
- Emergency controls integration
- Real-time status indicators
- Interactive control elements
- Safety confirmation dialogs

**Deliverables:**
- [ ] `3-1-control-panel.html` - Control panel interface
- [ ] Manual control elements
- [ ] Emergency controls
- [ ] Safety confirmation system
- [ ] Real-time status display

---

## 🎯 **DESIGN REQUIREMENTS**

### **Design System Standards:**
```css
/* Unified Design System - OHT-50 Industrial Theme */
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
  
  /* Safety Colors */
  --safety-red: #DC2626;
  --safety-orange: #EA580C;
  --safety-yellow: #CA8A04;
  --safety-green: #16A34A;
  
  /* Typography */
  --font-sans: 'Inter', -apple-system, BlinkMacSystemFont, sans-serif;
  --font-mono: 'JetBrains Mono', 'Fira Code', monospace;
  --font-display: 'Space Grotesk', 'Inter', sans-serif;
  
  /* Spacing */
  --space-xs: 0.25rem;
  --space-sm: 0.5rem;
  --space-md: 1rem;
  --space-lg: 1.5rem;
  --space-xl: 2rem;
  --space-2xl: 3rem;
}
```

### **Component Library Standards:**
```html
<!-- Button Components -->
<button class="btn btn-primary">Primary Button</button>
<button class="btn btn-secondary">Secondary Button</button>
<button class="btn btn-danger">Danger Button</button>
<button class="btn btn-emergency">
  <i data-lucide="alert-triangle"></i>
  E-STOP
</button>

<!-- Card Components -->
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

<!-- Form Components -->
<div class="form-group">
  <label for="input1" class="form-label">Label</label>
  <input type="text" id="input1" class="form-input" placeholder="Enter text...">
  <div class="form-help">Help text</div>
</div>
```

### **Safety-First Design:**
- **Emergency controls** luôn visible và accessible
- **E-Stop button** trên mọi page (theo REQ_UI_UX_Alert_System.md)
- **Safety colors** cho critical information (Red: #DC2626, Orange: #EA580C, Yellow: #CA8A04, Green: #16A34A)
- **Clear visual hierarchy** cho safety information
- **Confirmation dialogs** cho critical actions
- **Alert System Integration** theo REQ_UI_UX_Alert_System.md
- **Emergency Stop Response** < 100ms theo safety requirements
- **Real-time Status Indicators** cho safety monitoring

---

## 📁 **FILE STRUCTURE TO CREATE**

```
.superdesign/design_iterations/foundation/
├── design_system_guide.md              # P1-T1 Deliverable
├── color_palette_demo.html             # P1-T1 Deliverable
├── typography_guide.html               # P1-T1 Deliverable
├── spacing_system.html                 # P1-T1 Deliverable
├── component_library.html              # P1-T3 Deliverable
├── button_variants.html                # P1-T3 Deliverable
├── card_system.html                    # P1-T3 Deliverable
├── form_components.html                # P1-T3 Deliverable
└── navigation_demo.html                # P1-T3 Deliverable

.superdesign/design_iterations/core_pages/
├── 1-1-auth-login.html                 # P2-T1 Deliverable
└── 3-1-control-panel.html              # P2-T3 Deliverable
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
5. **Confirmation Dialogs** cho critical actions

### **Accessibility Features:**
1. **WCAG 2.1 AA Compliance** (theo REQ_UI_UX_Accessibility.md)
2. **Keyboard Navigation** với focus indicators
3. **Screen Reader Support** với ARIA labels
4. **High Contrast Mode** support
5. **Focus Management** system
6. **Touch Target Sizes** minimum 44px
7. **Reduced Motion** preferences support
8. **Semantic HTML** structure
9. **Color Contrast** ratios meet WCAG standards

### **Responsive Design:**
1. **Mobile-first approach** theo REQ_UI_UX_Visual_Design.md
2. **Breakpoints:** 640px, 768px, 1024px, 1280px, 1536px
3. **Touch-friendly targets** (min 44px)
4. **Readable text** trên mọi device
5. **Industrial Theme** với orange color scheme (#FF6B35)
6. **Outdoor Visibility** cho môi trường ngoài trời
7. **Typography System** (Inter, JetBrains Mono, Space Grotesk)

---

## 📊 **ACCEPTANCE CRITERIA**

### **Design System:**
- [ ] Complete design tokens system
- [ ] Color palette documentation
- [ ] Typography guidelines
- [ ] Spacing and layout rules
- [ ] Component design principles

### **Component Library:**
- [ ] Button component library
- [ ] Card component system
- [ ] Form component library
- [ ] Navigation components
- [ ] Icon system integration

### **Authentication Interface:**
- [ ] Secure login form
- [ ] Form validation system
- [ ] Error handling interface
- [ ] Security indicators
- [ ] Responsive layout tested

### **Control Panel Interface:**
- [ ] Manual control elements
- [ ] Emergency controls integration
- [ ] Real-time status indicators
- [ ] Safety confirmation system
- [ ] Interactive control elements

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
1. **Day 1-3:** Unified Design System Creation
2. **Day 4-7:** Component Library Foundation

### **Step 2: Core Pages (Week 3-4)**
1. **Day 1-3:** Authentication & Login Page
2. **Day 4-7:** Control Panel Interface

### **Step 3: Testing & Validation**
1. **Cross-browser testing**
2. **Accessibility testing**
3. **Responsive testing**
4. **Documentation update**

---

## 🎯 **SUCCESS METRICS**

### **Design System Metrics:**
- **Design Token Coverage:** 100% components use design tokens
- **Component Reusability:** 90% components reusable across pages
- **Consistency Score:** 100% visual consistency across interface
- **Theme Compliance:** 100% industrial theme implementation

### **Component Library Metrics:**
- **Component Coverage:** 100% required components implemented
- **Variant Support:** All button/form/card variants available
- **Icon Integration:** 100% icons properly integrated
- **Documentation:** 100% components documented

### **Interface Metrics:**
- **Form Validation:** 100% forms have proper validation
- **Error Handling:** 100% error states handled
- **Safety Features:** 100% emergency controls implemented
- **Responsive Design:** 100% responsive on all devices

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
2. **Start** với P1-T1: Unified Design System Creation
3. **Create** `design_system_guide.md` documentation
4. **Build** `color_palette_demo.html` showcase
5. **Document** progress và issues

**Remember:** Safety-first design, industrial theme, accessibility compliance!

---

## 📋 **SPECIFIC TASK BREAKDOWN**

### **P1-T1: Unified Design System Creation**

#### **Day 1-2: Design Tokens & Color Palette**
- [ ] Create comprehensive design tokens documentation
- [ ] Build interactive color palette showcase
- [ ] Document color usage guidelines
- [ ] Create color accessibility guidelines

#### **Day 3-4: Typography & Spacing**
- [ ] Create typography guide với examples
- [ ] Build spacing system demonstration
- [ ] Document font usage guidelines
- [ ] Create responsive typography rules

#### **Day 5-7: Documentation & Guidelines**
- [ ] Create component design principles
- [ ] Build design system guide
- [ ] Document usage guidelines
- [ ] Create implementation examples

### **P1-T3: Component Library Foundation**

#### **Day 1-2: Button & Card Components**
- [ ] Create button component library với all variants
- [ ] Build card component system
- [ ] Implement interactive demos
- [ ] Document component usage

#### **Day 3-4: Form & Navigation Components**
- [ ] Create form component library
- [ ] Build navigation components
- [ ] Implement form validation examples
- [ ] Create navigation patterns

#### **Day 5-7: Integration & Documentation**
- [ ] Integrate icon system
- [ ] Create component showcase
- [ ] Document all components
- [ ] Create implementation guides

### **P2-T1: Authentication & Login Page**

#### **Day 1-3: Core Interface**
- [ ] Create login form với validation
- [ ] Implement error handling
- [ ] Add security indicators
- [ ] Create responsive layout

#### **Day 4-7: Enhancement & Testing**
- [ ] Add accessibility features
- [ ] Implement safety features
- [ ] Test responsive design
- [ ] Validate accessibility compliance

### **P2-T3: Control Panel Interface**

#### **Day 1-3: Core Controls**
- [ ] Create manual control elements
- [ ] Implement emergency controls
- [ ] Add real-time status indicators
- [ ] Create safety confirmation system

#### **Day 4-7: Enhancement & Testing**
- [ ] Add interactive elements
- [ ] Implement safety features
- [ ] Test responsive design
- [ ] Validate accessibility compliance

---

**📅 Last Updated:** 2025-01-28  
**👤 Team:** Designer B  
**🎯 Status:** Ready to Start - Foundation Phase
