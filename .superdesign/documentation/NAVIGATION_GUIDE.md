# 🧭 OHT-50 Navigation System Guide

**Phiên bản:** v2.0  
**Ngày cập nhật:** 2025-01-28  
**Mục tiêu:** Comprehensive navigation system cho OHT-50

---

## 🎯 **NAVIGATION PHILOSOPHY**

### **Navigation-First Approach:**
- **Bắt buộc:** Không bao giờ tạo page mà không có navigation
- **User Journey:** Mỗi page phải có clear way để chuyển sang page khác
- **Consistency:** Navigation structure nhất quán trong toàn bộ hệ thống
- **Accessibility:** Keyboard navigation và screen reader support

### **Safety-First Navigation:**
- **Emergency Access:** Emergency controls accessible từ mọi page
- **Quick Return:** Easy way to return to dashboard từ bất kỳ đâu
- **Status Visibility:** Current location và system status luôn visible
- **Error Recovery:** Clear navigation path khi có lỗi

---

## 🏗️ **NAVIGATION STRUCTURE**

### **Primary Navigation (Top Bar):**
```html
<!-- Top Navigation Bar -->
<nav class="top-navigation">
  <div class="nav-container">
    <!-- Brand/Logo -->
    <div class="nav-brand">
      <i data-lucide="bot" class="w-6 h-6 text-primary-orange"></i>
      <span class="brand-text">OHT-50</span>
    </div>
    
    <!-- Main Navigation Menu -->
    <div class="nav-menu">
      <a href="/dashboard" class="nav-item" data-page="dashboard">
        <i data-lucide="layout-dashboard"></i>
        <span>Dashboard</span>
      </a>
      
      <a href="/control" class="nav-item" data-page="control">
        <i data-lucide="gamepad-2"></i>
        <span>Control</span>
      </a>
      
      <a href="/map" class="nav-item" data-page="map">
        <i data-lucide="map"></i>
        <span>Map</span>
      </a>
      
      <a href="/monitoring" class="nav-item" data-page="monitoring">
        <i data-lucide="activity"></i>
        <span>Monitoring</span>
      </a>
      
      <a href="/safety" class="nav-item" data-page="safety">
        <i data-lucide="shield-alert"></i>
        <span>Safety</span>
      </a>
      
      <a href="/config" class="nav-item" data-page="config">
        <i data-lucide="settings"></i>
        <span>Config</span>
      </a>
    </div>
    
    <!-- Right Side Actions -->
    <div class="nav-actions">
      <!-- Emergency Stop Button (Always Visible) -->
      <button class="nav-btn btn-emergency" id="emergencyStop">
        <i data-lucide="power-off"></i>
        <span>E-STOP</span>
      </button>
      
      <!-- Notifications -->
      <button class="nav-btn" id="notificationsBtn">
        <i data-lucide="bell"></i>
        <span class="notification-badge">3</span>
      </button>
      
      <!-- User Menu -->
      <div class="user-menu">
        <button class="nav-btn" id="userMenuBtn">
          <i data-lucide="user"></i>
          <span>Admin</span>
        </button>
      </div>
    </div>
  </div>
</nav>
```

### **Secondary Navigation (Sidebar):**
```html
<!-- Sidebar Navigation -->
<aside class="sidebar-navigation" id="sidebarNav">
  <div class="sidebar-header">
    <h3>Navigation</h3>
    <button class="sidebar-toggle" id="sidebarToggle">
      <i data-lucide="menu"></i>
    </button>
  </div>
  
  <nav class="sidebar-menu">
    <!-- Dashboard Section -->
    <div class="nav-section">
      <h4 class="nav-section-title">Dashboard</h4>
      <a href="/dashboard/overview" class="sidebar-item">
        <i data-lucide="layout-dashboard"></i>
        <span>Overview</span>
      </a>
      <a href="/dashboard/quick-actions" class="sidebar-item">
        <i data-lucide="zap"></i>
        <span>Quick Actions</span>
      </a>
      <a href="/dashboard/system-health" class="sidebar-item">
        <i data-lucide="heart"></i>
        <span>System Health</span>
      </a>
    </div>
    
    <!-- Robot Control Section -->
    <div class="nav-section">
      <h4 class="nav-section-title">Robot Control</h4>
      <a href="/control/manual" class="sidebar-item">
        <i data-lucide="gamepad-2"></i>
        <span>Manual Control</span>
      </a>
      <a href="/control/movement" class="sidebar-item">
        <i data-lucide="move"></i>
        <span>Movement</span>
      </a>
      <a href="/control/emergency" class="sidebar-item">
        <i data-lucide="power-off"></i>
        <span>Emergency Stop</span>
      </a>
      <a href="/control/voice" class="sidebar-item">
        <i data-lucide="mic"></i>
        <span>Voice Commands</span>
      </a>
    </div>
    
    <!-- Map & Navigation Section -->
    <div class="nav-section">
      <h4 class="nav-section-title">Map & Navigation</h4>
      <a href="/map/live-location" class="sidebar-item">
        <i data-lucide="map-pin"></i>
        <span>Live Location</span>
      </a>
      <a href="/map/lidar-view" class="sidebar-item">
        <i data-lucide="radar"></i>
        <span>LiDAR View</span>
      </a>
      <a href="/map/dock-status" class="sidebar-item">
        <i data-lucide="home"></i>
        <span>Dock Status</span>
      </a>
      <a href="/map/path-planning" class="sidebar-item">
        <i data-lucide="route"></i>
        <span>Path Planning</span>
      </a>
    </div>
    
    <!-- Monitoring Section -->
    <div class="nav-section">
      <h4 class="nav-section-title">Monitoring</h4>
      <a href="/monitoring/telemetry" class="sidebar-item">
        <i data-lucide="activity"></i>
        <span>Telemetry</span>
      </a>
      <a href="/monitoring/speed-position" class="sidebar-item">
        <i data-lucide="gauge"></i>
        <span>Speed & Position</span>
      </a>
      <a href="/monitoring/connection" class="sidebar-item">
        <i data-lucide="wifi"></i>
        <span>Connection</span>
      </a>
      <a href="/monitoring/performance" class="sidebar-item">
        <i data-lucide="trending-up"></i>
        <span>Performance</span>
      </a>
    </div>
    
    <!-- Safety & Alarms Section -->
    <div class="nav-section">
      <h4 class="nav-section-title">Safety & Alarms</h4>
      <a href="/safety/active-alerts" class="sidebar-item">
        <i data-lucide="alert-triangle"></i>
        <span>Active Alerts</span>
      </a>
      <a href="/safety/alert-history" class="sidebar-item">
        <i data-lucide="history"></i>
        <span>Alert History</span>
      </a>
      <a href="/safety/safety-status" class="sidebar-item">
        <i data-lucide="shield"></i>
        <span>Safety Status</span>
      </a>
      <a href="/safety/emergency" class="sidebar-item">
        <i data-lucide="siren"></i>
        <span>Emergency</span>
      </a>
    </div>
    
    <!-- Configuration Section -->
    <div class="nav-section">
      <h4 class="nav-section-title">Configuration</h4>
      <a href="/config/robot-settings" class="sidebar-item">
        <i data-lucide="bot"></i>
        <span>Robot Settings</span>
      </a>
      <a href="/config/system-params" class="sidebar-item">
        <i data-lucide="settings"></i>
        <span>System Params</span>
      </a>
      <a href="/config/user-management" class="sidebar-item">
        <i data-lucide="users"></i>
        <span>User Management</span>
      </a>
      <a href="/config/access-control" class="sidebar-item">
        <i data-lucide="lock"></i>
        <span>Access Control</span>
      </a>
    </div>
  </nav>
</aside>
```

---

## 🎨 **NAVIGATION STYLES**

### **Top Navigation CSS:**
```css
/* Top Navigation */
.top-navigation {
  background: var(--bg-primary);
  border-bottom: 1px solid var(--industrial-gray-300);
  box-shadow: var(--shadow-sm);
  position: sticky;
  top: 0;
  z-index: 1000;
}

.nav-container {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: var(--space-md) var(--space-lg);
  max-width: 1920px;
  margin: 0 auto;
}

/* Brand */
.nav-brand {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
  font-weight: var(--font-bold);
  color: var(--primary-orange);
  font-size: var(--text-lg);
}

.brand-text {
  font-family: var(--font-display);
}

/* Main Menu */
.nav-menu {
  display: flex;
  align-items: center;
  gap: var(--space-lg);
}

.nav-item {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
  padding: var(--space-sm) var(--space-md);
  text-decoration: none;
  color: var(--industrial-gray-600);
  border-radius: var(--radius-md);
  transition: all 0.2s ease;
  font-weight: var(--font-medium);
}

.nav-item:hover {
  background: var(--industrial-gray-100);
  color: var(--industrial-gray-700);
}

.nav-item.active {
  background: var(--primary-orange);
  color: white;
}

.nav-item i {
  width: 18px;
  height: 18px;
}

/* Actions */
.nav-actions {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
}

.nav-btn {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
  padding: var(--space-sm) var(--space-md);
  background: var(--bg-secondary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: var(--radius-md);
  color: var(--industrial-gray-700);
  cursor: pointer;
  transition: all 0.2s ease;
  font-weight: var(--font-medium);
}

.nav-btn:hover {
  background: var(--industrial-gray-100);
  border-color: var(--industrial-gray-400);
}

/* Emergency Button */
.nav-btn.btn-emergency {
  background: var(--safety-red);
  color: white;
  border-color: var(--safety-red);
  font-weight: var(--font-bold);
  animation: pulse-emergency 2s infinite;
}

.nav-btn.btn-emergency:hover {
  background: var(--safety-red-dark);
  border-color: var(--safety-red-dark);
}

/* Notification Badge */
.notification-badge {
  background: var(--error-red);
  color: white;
  border-radius: var(--radius-full);
  padding: var(--space-xs) var(--space-sm);
  font-size: var(--text-xs);
  font-weight: var(--font-bold);
  min-width: 20px;
  text-align: center;
}
```

### **Sidebar Navigation CSS:**
```css
/* Sidebar Navigation */
.sidebar-navigation {
  width: 280px;
  background: var(--bg-primary);
  border-right: 1px solid var(--industrial-gray-300);
  height: 100vh;
  overflow-y: auto;
  position: fixed;
  left: 0;
  top: 0;
  z-index: 900;
  transition: transform 0.3s ease;
}

.sidebar-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-lg);
  border-bottom: 1px solid var(--industrial-gray-300);
  background: var(--bg-secondary);
}

.sidebar-header h3 {
  margin: 0;
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.sidebar-toggle {
  background: none;
  border: none;
  color: var(--industrial-gray-600);
  cursor: pointer;
  padding: var(--space-xs);
  border-radius: var(--radius-sm);
  transition: all 0.2s ease;
}

.sidebar-toggle:hover {
  background: var(--industrial-gray-200);
  color: var(--industrial-gray-700);
}

/* Sidebar Menu */
.sidebar-menu {
  padding: var(--space-md);
}

.nav-section {
  margin-bottom: var(--space-xl);
}

.nav-section-title {
  margin: 0 0 var(--space-md) 0;
  color: var(--industrial-gray-500);
  font-size: var(--text-sm);
  font-weight: var(--font-semibold);
  text-transform: uppercase;
  letter-spacing: var(--tracking-wide);
}

.sidebar-item {
  display: flex;
  align-items: center;
  gap: var(--space-md);
  padding: var(--space-md);
  text-decoration: none;
  color: var(--industrial-gray-600);
  border-radius: var(--radius-md);
  transition: all 0.2s ease;
  margin-bottom: var(--space-xs);
}

.sidebar-item:hover {
  background: var(--industrial-gray-100);
  color: var(--industrial-gray-700);
}

.sidebar-item.active {
  background: var(--primary-orange);
  color: white;
}

.sidebar-item i {
  width: 20px;
  height: 20px;
  flex-shrink: 0;
}

.sidebar-item span {
  font-weight: var(--font-medium);
}
```

---

## 📱 **RESPONSIVE NAVIGATION**

### **Mobile Navigation:**
```css
/* Mobile Navigation */
@media (max-width: 768px) {
  /* Hide sidebar on mobile */
  .sidebar-navigation {
    transform: translateX(-100%);
  }
  
  .sidebar-navigation.open {
    transform: translateX(0);
  }
  
  /* Mobile menu button */
  .mobile-menu-btn {
    display: block;
  }
  
  /* Adjust top navigation */
  .nav-menu {
    display: none; /* Hide main menu on mobile */
  }
  
  .nav-actions {
    gap: var(--space-xs);
  }
  
  .nav-btn span {
    display: none; /* Hide text, show only icons */
  }
}

/* Mobile Menu Overlay */
.mobile-menu-overlay {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: rgba(0, 0, 0, 0.5);
  z-index: 800;
  opacity: 0;
  visibility: hidden;
  transition: all 0.3s ease;
}

.mobile-menu-overlay.open {
  opacity: 1;
  visibility: visible;
}
```

### **Tablet Navigation:**
```css
/* Tablet Navigation */
@media (min-width: 769px) and (max-width: 1024px) {
  .sidebar-navigation {
    width: 240px;
  }
  
  .nav-item span {
    display: none; /* Hide text in top nav */
  }
  
  .nav-item {
    padding: var(--space-sm);
  }
}
```

---

## ♿ **ACCESSIBILITY FEATURES**

### **Keyboard Navigation:**
```javascript
// Keyboard Navigation Support
document.addEventListener('keydown', function(event) {
  // Tab navigation
  if (event.key === 'Tab') {
    // Handle tab navigation
  }
  
  // Escape key to close modals/menus
  if (event.key === 'Escape') {
    closeAllMenus();
  }
  
  // Arrow keys for menu navigation
  if (event.key === 'ArrowDown' || event.key === 'ArrowUp') {
    navigateMenu(event.key);
  }
  
  // Enter/Space for activation
  if (event.key === 'Enter' || event.key === ' ') {
    activateCurrentItem();
  }
});
```

### **Screen Reader Support:**
```html
<!-- ARIA Labels for Navigation -->
<nav class="top-navigation" role="navigation" aria-label="Main navigation">
  <div class="nav-menu" role="menubar">
    <a href="/dashboard" class="nav-item" role="menuitem" aria-current="page">
      <i data-lucide="layout-dashboard" aria-hidden="true"></i>
      <span>Dashboard</span>
    </a>
  </div>
</nav>

<!-- Skip Links -->
<a href="#main-content" class="skip-link">Skip to main content</a>
<a href="#navigation" class="skip-link">Skip to navigation</a>
```

---

## 🔄 **NAVIGATION BEHAVIORS**

### **Active State Management:**
```javascript
// Active State Management
function updateActiveNavigation() {
  const currentPath = window.location.pathname;
  
  // Update top navigation
  document.querySelectorAll('.nav-item').forEach(item => {
    item.classList.remove('active');
    if (item.getAttribute('href') === currentPath) {
      item.classList.add('active');
    }
  });
  
  // Update sidebar navigation
  document.querySelectorAll('.sidebar-item').forEach(item => {
    item.classList.remove('active');
    if (item.getAttribute('href') === currentPath) {
      item.classList.add('active');
    }
  });
  
  // Update breadcrumbs
  updateBreadcrumbs(currentPath);
}

// Breadcrumb Navigation
function updateBreadcrumbs(path) {
  const breadcrumbContainer = document.getElementById('breadcrumbs');
  const pathSegments = path.split('/').filter(segment => segment);
  
  let breadcrumbHTML = '<a href="/">Home</a>';
  
  pathSegments.forEach((segment, index) => {
    const isLast = index === pathSegments.length - 1;
    const href = '/' + pathSegments.slice(0, index + 1).join('/');
    const label = segment.charAt(0).toUpperCase() + segment.slice(1);
    
    if (isLast) {
      breadcrumbHTML += ` <span aria-current="page">${label}</span>`;
    } else {
      breadcrumbHTML += ` <a href="${href}">${label}</a>`;
    }
  });
  
  breadcrumbContainer.innerHTML = breadcrumbHTML;
}
```

### **Navigation Guards:**
```javascript
// Navigation Guards for Safety
function setupNavigationGuards() {
  // Prevent navigation during critical operations
  document.addEventListener('click', function(event) {
    const link = event.target.closest('a');
    if (link && isCriticalOperationActive()) {
      event.preventDefault();
      showWarningDialog('Cannot navigate during critical operation');
    }
  });
  
  // Emergency stop accessible from anywhere
  document.addEventListener('keydown', function(event) {
    if (event.key === ' ' && event.ctrlKey) {
      event.preventDefault();
      triggerEmergencyStop();
    }
  });
}

function isCriticalOperationActive() {
  return document.body.classList.contains('critical-operation');
}
```

---

## 📊 **NAVIGATION METRICS**

### **Performance Targets:**
- **Navigation Response Time:** < 100ms
- **Menu Open/Close:** < 200ms
- **Page Transition:** < 300ms
- **Keyboard Navigation:** 100% functional
- **Screen Reader Support:** 100% compatible

### **Usability Metrics:**
- **Navigation Accuracy:** > 95%
- **User Journey Completion:** > 90%
- **Error Recovery:** > 85%
- **Accessibility Compliance:** 100% WCAG 2.1 AA

---

## 🚨 **SAFETY NAVIGATION FEATURES**

### **Emergency Navigation:**
```html
<!-- Emergency Navigation Always Visible -->
<div class="emergency-navigation">
  <button class="emergency-btn" id="emergencyStop">
    <i data-lucide="power-off"></i>
    <span>EMERGENCY STOP</span>
  </button>
  
  <button class="emergency-btn secondary" id="pauseBtn">
    <i data-lucide="pause"></i>
    <span>PAUSE</span>
  </button>
  
  <button class="emergency-btn secondary" id="homeBtn">
    <i data-lucide="home"></i>
    <span>HOME</span>
  </button>
</div>
```

### **Safety Navigation CSS:**
```css
/* Emergency Navigation */
.emergency-navigation {
  position: fixed;
  top: 50%;
  right: var(--space-lg);
  transform: translateY(-50%);
  display: flex;
  flex-direction: column;
  gap: var(--space-sm);
  z-index: 2000;
}

.emergency-btn {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
  padding: var(--space-md) var(--space-lg);
  background: var(--safety-red);
  color: white;
  border: none;
  border-radius: var(--radius-lg);
  font-weight: var(--font-bold);
  cursor: pointer;
  transition: all 0.2s ease;
  box-shadow: var(--shadow-lg);
}

.emergency-btn:hover {
  transform: scale(1.05);
  box-shadow: var(--shadow-xl);
}

.emergency-btn.secondary {
  background: var(--warning-yellow);
  color: var(--industrial-gray-800);
}
```

---

**Changelog v2.0:**
- ✅ Created comprehensive navigation system guide
- ✅ Added responsive navigation patterns
- ✅ Included accessibility features
- ✅ Added safety navigation components
- ✅ Enhanced keyboard navigation support
- ✅ Added navigation guards và metrics

**🚨 Lưu ý:** Navigation system phải tuân thủ navigation-first approach và đảm bảo emergency controls accessible từ mọi page.
