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

---

# 🎯 **TEAM A TASK ASSIGNMENT - DASHBOARD COMPLETION**

**Team:** Team A (Layout & Navigation)  
**Task:** Hoàn thiện Dashboard Main Interface  
**Priority:** 🔴 CRITICAL  
**Due Date:** 2025-01-29  
**Status:** 🚀 IN PROGRESS

---

## 📋 **TASK OVERVIEW**

### **Mục tiêu:**
Hoàn thiện trang `2-1-dashboard-main.html` để tuân thủ 100% yêu cầu từ `REQ_UI_UX_Dashboard_Design.md` và foundation design system.

### **Current Status:**
- ✅ Layout structure cơ bản đã đúng
- ✅ Design system compliance đạt 95%
- ❌ **THIẾU:** Manual Control Panel
- ❌ **THIẾU:** LiDAR Map Interface  
- ❌ **THIẾU:** Battery visual indicator
- ❌ **THIẾU:** Mode switch controls

---

## 🎯 **CRITICAL REQUIREMENTS - PHẢI HOÀN THÀNH**

### **1. MANUAL CONTROL PANEL (BẮT BUỘC)**
```html
<!-- Thêm vào dashboard sau Emergency Controls -->
<section class="col-span-6 manual-control-panel">
  <div class="control-header">
    <h3>🎮 Manual Control</h3>
    <div class="control-mode-switch">
      <label class="switch">
        <input type="checkbox" id="manualMode">
        <span class="slider"></span>
      </label>
      <span class="mode-label">Manual Mode</span>
    </div>
  </div>
  
  <div class="control-grid">
    <!-- Movement Controls -->
    <div class="control-section">
      <h4>Movement</h4>
      <div class="movement-controls">
        <button class="control-btn btn-forward" data-action="forward">
          <i data-lucide="arrow-up"></i>
          <span>Forward</span>
        </button>
        
        <div class="horizontal-controls">
          <button class="control-btn btn-left" data-action="left">
            <i data-lucide="arrow-left"></i>
            <span>Left</span>
          </button>
          
          <button class="control-btn btn-stop" data-action="stop">
            <i data-lucide="square"></i>
            <span>Stop</span>
          </button>
          
          <button class="control-btn btn-right" data-action="right">
            <i data-lucide="arrow-right"></i>
            <span>Right</span>
          </button>
        </div>
        
        <button class="control-btn btn-backward" data-action="backward">
          <i data-lucide="arrow-down"></i>
          <span>Backward</span>
        </button>
      </div>
    </div>
    
    <!-- Speed Control -->
    <div class="control-section">
      <h4>Speed Control</h4>
      <div class="speed-control">
        <input type="range" id="speedSlider" min="0" max="100" value="50" class="speed-slider">
        <div class="speed-display">
          <span class="speed-value">50%</span>
          <span class="speed-unit">Max Speed</span>
        </div>
        <div class="speed-presets">
          <button class="speed-preset" onclick="setSpeed(25)">Slow</button>
          <button class="speed-preset active" onclick="setSpeed(50)">Normal</button>
          <button class="speed-preset" onclick="setSpeed(75)">Fast</button>
          <button class="speed-preset" onclick="setSpeed(100)">Max</button>
        </div>
      </div>
    </div>
  </div>
</section>
```

### **2. LIDAR MAP INTERFACE (BẮT BUỘC)**
```html
<!-- Thêm vào dashboard thay thế chart placeholder -->
<section class="col-span-6 map-interface">
  <div class="map-header">
    <h3>🗺️ Robot Navigation Map</h3>
    <div class="map-controls">
      <button class="map-btn" data-action="zoom-in" aria-label="Zoom In">
        <i data-lucide="plus"></i>
      </button>
      <button class="map-btn" data-action="zoom-out" aria-label="Zoom Out">
        <i data-lucide="minus"></i>
      </button>
      <button class="map-btn" data-action="reset-view" aria-label="Reset View">
        <i data-lucide="refresh-cw"></i>
      </button>
      <button class="map-btn" data-action="fullscreen" aria-label="Fullscreen">
        <i data-lucide="maximize-2"></i>
      </button>
    </div>
  </div>
  
  <div class="map-container">
    <canvas id="lidarCanvas" class="lidar-canvas"></canvas>
    
    <!-- Robot Position Indicator -->
    <div class="robot-indicator" id="robotIndicator">
      <div class="robot-marker">🤖</div>
      <div class="robot-direction"></div>
    </div>
    
    <!-- Obstacle Markers -->
    <div class="obstacle-markers" id="obstacleMarkers">
      <!-- Dynamic obstacle markers -->
    </div>
    
    <!-- Path Visualization -->
    <svg class="path-overlay" id="pathOverlay">
      <!-- Dynamic path lines -->
    </svg>
  </div>
  
  <div class="map-info">
    <div class="info-item">
      <span class="info-label">Position:</span>
      <span class="info-value" id="positionInfo">X: 15.2m, Y: 8.7m</span>
    </div>
    <div class="info-item">
      <span class="info-label">Heading:</span>
      <span class="info-value" id="headingInfo">45°</span>
    </div>
    <div class="info-item">
      <span class="info-label">Distance to Goal:</span>
      <span class="info-value" id="distanceInfo">12.5m</span>
    </div>
  </div>
</section>
```

### **3. BATTERY VISUAL INDICATOR (BẮT BUỘC)**
```html
<!-- Cập nhật battery display trong System Status -->
<div class="metric-display">
  <span class="metric-label">Battery</span>
  <div class="battery-indicator">
    <div class="battery-level" style="width: 87%"></div>
    <span class="battery-text">87%</span>
  </div>
</div>
```

### **4. MODE SWITCH (BẮT BUỘC)**
```html
<!-- Thêm vào header hoặc status panel -->
<div class="mode-selector">
  <button class="mode-btn active" data-mode="auto">AUTO</button>
  <button class="mode-btn" data-mode="manual">MANUAL</button>
  <button class="mode-btn" data-mode="semi">SEMI</button>
</div>
```

---

## 🎨 **CSS REQUIREMENTS - PHẢI IMPLEMENT**

### **1. Manual Control Panel CSS:**
```css
/* Manual Control Panel Styles */
.manual-control-panel {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: var(--radius-lg);
  padding: var(--space-lg);
  box-shadow: var(--shadow-md);
}

.control-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-lg);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

/* Mode Switch */
.control-mode-switch {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
}

.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}

.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: var(--industrial-gray-400);
  transition: 0.4s;
  border-radius: 34px;
}

.slider:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  transition: 0.4s;
  border-radius: 50%;
}

input:checked + .slider {
  background-color: var(--primary-orange);
}

input:checked + .slider:before {
  transform: translateX(26px);
}

/* Movement Controls */
.movement-controls {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: var(--space-sm);
}

.horizontal-controls {
  display: flex;
  gap: var(--space-sm);
  align-items: center;
}

.control-btn {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  min-width: 80px;
  min-height: 80px;
  background: var(--bg-primary);
  border: 2px solid var(--industrial-gray-300);
  border-radius: var(--radius-md);
  cursor: pointer;
  transition: all 0.2s ease;
  font-weight: var(--font-medium);
}

.control-btn:hover {
  border-color: var(--primary-orange);
  background: rgba(255, 107, 53, 0.1);
  transform: translateY(-2px);
  box-shadow: var(--shadow-md);
}

.btn-forward { background: var(--safety-green); color: white; }
.btn-backward { background: var(--safety-orange); color: white; }
.btn-left, .btn-right { background: var(--primary-orange); color: white; }
.btn-stop { background: var(--safety-red); color: white; }

/* Speed Control */
.speed-control {
  display: flex;
  flex-direction: column;
  gap: var(--space-md);
}

.speed-slider {
  width: 100%;
  height: 8px;
  border-radius: 4px;
  background: var(--industrial-gray-300);
  outline: none;
  -webkit-appearance: none;
}

.speed-slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 20px;
  height: 20px;
  border-radius: 50%;
  background: var(--primary-orange);
  cursor: pointer;
  box-shadow: var(--shadow-sm);
}

.speed-presets {
  display: flex;
  gap: var(--space-sm);
  justify-content: center;
}

.speed-preset {
  padding: var(--space-xs) var(--space-sm);
  border: 1px solid var(--industrial-gray-300);
  border-radius: var(--radius-sm);
  background: var(--bg-primary);
  color: var(--industrial-gray-700);
  cursor: pointer;
  transition: all 0.2s ease;
}

.speed-preset.active {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}
```

### **2. Map Interface CSS:**
```css
/* Map Interface Styles */
.map-interface {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: var(--radius-lg);
  overflow: hidden;
  box-shadow: var(--shadow-md);
}

.map-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-secondary);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.map-controls {
  display: flex;
  gap: var(--space-sm);
}

.map-btn {
  width: 40px;
  height: 40px;
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: var(--radius-md);
  cursor: pointer;
  transition: all 0.2s ease;
  display: flex;
  align-items: center;
  justify-content: center;
}

.map-btn:hover {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}

.map-container {
  position: relative;
  width: 100%;
  height: 400px;
  background: var(--bg-secondary);
  overflow: hidden;
}

.lidar-canvas {
  width: 100%;
  height: 100%;
  background: linear-gradient(45deg, var(--bg-secondary) 25%, transparent 25%), 
              linear-gradient(-45deg, var(--bg-secondary) 25%, transparent 25%), 
              linear-gradient(45deg, transparent 75%, var(--bg-secondary) 75%), 
              linear-gradient(-45deg, transparent 75%, var(--bg-secondary) 75%);
  background-size: 20px 20px;
  background-position: 0 0, 0 10px, 10px -10px, -10px 0px;
}

.robot-indicator {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  z-index: 10;
}

.robot-marker {
  font-size: 2rem;
  filter: drop-shadow(0 2px 4px rgba(0, 0, 0, 0.3));
}

.map-info {
  display: flex;
  justify-content: space-around;
  padding: var(--space-md);
  background: var(--bg-secondary);
  border-top: 1px solid var(--industrial-gray-300);
}

.info-item {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: var(--space-xs);
}

.info-label {
  font-size: var(--text-sm);
  color: var(--industrial-gray-500);
  font-weight: var(--font-medium);
}

.info-value {
  font-size: var(--text-base);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}
```

### **3. Battery Indicator CSS:**
```css
/* Battery Indicator */
.battery-indicator {
  position: relative;
  width: 60px;
  height: 20px;
  background: var(--industrial-gray-300);
  border-radius: 10px;
  overflow: hidden;
}

.battery-level {
  height: 100%;
  background: linear-gradient(90deg, var(--success-green) 0%, var(--warning-yellow) 50%, var(--error-red) 100%);
  transition: width 0.3s ease;
}

.battery-text {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  font-size: var(--text-xs);
  font-weight: var(--font-bold);
  color: white;
  text-shadow: 0 1px 2px rgba(0, 0, 0, 0.5);
}
```

### **4. Mode Selector CSS:**
```css
/* Mode Selector */
.mode-selector {
  display: flex;
  gap: var(--space-sm);
  margin-bottom: var(--space-lg);
}

.mode-btn {
  flex: 1;
  padding: var(--space-sm) var(--space-md);
  border: 2px solid var(--industrial-gray-300);
  border-radius: var(--radius-md);
  background: var(--bg-primary);
  color: var(--industrial-gray-700);
  cursor: pointer;
  transition: all 0.2s ease;
  font-weight: var(--font-medium);
}

.mode-btn.active {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}
```

---

## 🔧 **JAVASCRIPT REQUIREMENTS - PHẢI IMPLEMENT**

### **1. Manual Control Functionality:**
```javascript
// Thêm vào DashboardController class
initializeManualControls() {
  // Mode switch
  document.getElementById('manualMode').addEventListener('change', (e) => {
    this.setManualMode(e.target.checked);
  });
  
  // Directional controls
  document.querySelectorAll('.control-btn[data-action]').forEach(btn => {
    btn.addEventListener('click', (e) => {
      this.handleDirectionalControl(e.currentTarget.dataset.action);
    });
  });
  
  // Speed control
  document.getElementById('speedSlider').addEventListener('input', (e) => {
    this.updateSpeed(e.target.value);
  });
  
  // Speed presets
  document.querySelectorAll('.speed-preset').forEach(btn => {
    btn.addEventListener('click', (e) => {
      this.setSpeedPreset(e.target.textContent.toLowerCase());
    });
  });
}

setManualMode(enabled) {
  const controls = document.querySelectorAll('.control-btn');
  controls.forEach(control => {
    control.disabled = !enabled;
  });
  
  this.addLogEntry('INFO', `Manual mode ${enabled ? 'enabled' : 'disabled'}`);
}

handleDirectionalControl(action) {
  if (!document.getElementById('manualMode').checked) {
    this.addLogEntry('WARNING', 'Manual mode required for directional controls');
    return;
  }
  
  this.addLogEntry('INFO', `Manual control: ${action}`);
  // Implement actual control logic here
}

updateSpeed(value) {
  document.querySelector('.speed-value').textContent = `${value}%`;
  this.addLogEntry('INFO', `Speed set to ${value}%`);
}

setSpeedPreset(preset) {
  const presets = { slow: 25, normal: 50, fast: 75, max: 100 };
  const value = presets[preset];
  
  document.getElementById('speedSlider').value = value;
  this.updateSpeed(value);
  
  // Update preset buttons
  document.querySelectorAll('.speed-preset').forEach(btn => {
    btn.classList.remove('active');
  });
  event.target.classList.add('active');
}
```

### **2. Map Interface Functionality:**
```javascript
// Thêm vào DashboardController class
initializeMapInterface() {
  // Map controls
  document.querySelectorAll('.map-btn').forEach(btn => {
    btn.addEventListener('click', (e) => {
      this.handleMapControl(e.currentTarget.dataset.action);
    });
  });
  
  // Initialize canvas
  this.initializeLidarCanvas();
}

handleMapControl(action) {
  switch(action) {
    case 'zoom-in':
      this.zoomMap(1.2);
      break;
    case 'zoom-out':
      this.zoomMap(0.8);
      break;
    case 'reset-view':
      this.resetMapView();
      break;
    case 'fullscreen':
      this.toggleFullscreen();
      break;
  }
}

initializeLidarCanvas() {
  const canvas = document.getElementById('lidarCanvas');
  const ctx = canvas.getContext('2d');
  
  // Set canvas size
  canvas.width = canvas.offsetWidth;
  canvas.height = canvas.offsetHeight;
  
  // Draw grid
  this.drawGrid(ctx, canvas.width, canvas.height);
  
  // Start animation loop
  this.animateMap();
}

drawGrid(ctx, width, height) {
  ctx.strokeStyle = 'rgba(0,0,0,0.1)';
  ctx.lineWidth = 1;
  
  // Vertical lines
  for (let x = 0; x <= width; x += 20) {
    ctx.beginPath();
    ctx.moveTo(x, 0);
    ctx.lineTo(x, height);
    ctx.stroke();
  }
  
  // Horizontal lines
  for (let y = 0; y <= height; y += 20) {
    ctx.beginPath();
    ctx.moveTo(0, y);
    ctx.lineTo(width, y);
    ctx.stroke();
  }
}

animateMap() {
  // Implement map animation loop
  requestAnimationFrame(() => this.animateMap());
}
```

---

## 📋 **DELIVERABLES CHECKLIST**

### **BẮT BUỘC HOÀN THÀNH:**
- [ ] **Manual Control Panel** với directional controls
- [ ] **LiDAR Map Interface** với canvas visualization
- [ ] **Battery visual indicator** với progress bar
- [ ] **Mode switch** (AUTO/MANUAL/SEMI)
- [ ] **Speed control** với slider và presets
- [ ] **Map controls** (zoom, reset, fullscreen)
- [ ] **JavaScript functionality** cho tất cả controls
- [ ] **Responsive design** cho tất cả components mới

### **QUALITY REQUIREMENTS:**
- [ ] **Tuân thủ 100%** design system foundation
- [ ] **Accessibility compliance** WCAG 2.1 AA
- [ ] **Safety-first design** principles
- [ ] **Real-time updates** cho map và controls
- [ ] **Error handling** cho tất cả interactions
- [ ] **Cross-browser compatibility**

---

## 🚨 **CRITICAL SUCCESS FACTORS**

### **1. Safety-First:**
- Emergency controls luôn accessible
- Manual mode có proper validation
- Speed limits enforced
- Confirmation dialogs cho critical actions

### **2. Real-time Updates:**
- Map position updates live
- Battery level real-time
- Status indicators responsive
- Log entries immediate

### **3. User Experience:**
- Intuitive control layout
- Clear visual feedback
- Responsive interactions
- Consistent design language

---

## 📞 **SUPPORT & RESOURCES**

### **Reference Documents:**
- `REQ_UI_UX_Dashboard_Design.md` - Yêu cầu chi tiết
- `theme_oht50_unified.css` - Design system foundation
- `theme_components.css` - Component library

### **Team Lead Contact:**
- **Review:** Sau khi hoàn thành
- **Support:** Khi gặp technical issues
- **Approval:** Trước khi merge

---

**🎯 MỤC TIÊU: Dashboard hoàn thiện 100% theo yêu cầu UI/UX và sẵn sàng cho production!**

**⏰ DEADLINE: 2025-01-29 - KHÔNG ĐƯỢC TRỄ!**
