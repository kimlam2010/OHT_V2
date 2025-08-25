# ⚙️ REQ_UI_UX_Configuration_Interface - Giao diện cấu hình

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, System Administrators, Product Managers

---

## 🎯 **Mục tiêu Configuration Interface:**

### **Concept chính:**
- **System:** Comprehensive configuration interface cho robot OHT-50
- **Focus:** Safety-first configuration với validation và rollback
- **Style:** Industrial theme với clear hierarchy và validation
- **Target:** System administrators và kỹ sư kỹ thuật cao

### **Nguyên tắc thiết kế:**
1. **Safety First:** Ưu tiên an toàn trong mọi cấu hình
2. **Validation:** Kiểm tra tính hợp lệ trước khi apply
3. **Rollback:** Khả năng khôi phục cấu hình cũ
4. **Audit Trail:** Theo dõi mọi thay đổi cấu hình

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| Configuration Categories | Phân loại cấu hình | System Admins, Engineers | System Architecture | ✅ Hoàn thành |
| Configuration Interface | Giao diện cấu hình | Designers, Developers | User Interface | ✅ Hoàn thành |
| Validation System | Hệ thống kiểm tra | Developers, QA Engineers | Data Validation | ✅ Hoàn thành |
| Configuration Management | Quản lý cấu hình | System Admins, DevOps | System Administration | ✅ Hoàn thành |

---

## 📋 **1. CONFIGURATION CATEGORIES - Phân loại cấu hình**

### **Configuration Hierarchy:**
```html
<!-- Configuration Categories -->
<div class="configuration-categories">
  <h3>📋 Configuration Categories</h3>
  
  <div class="config-hierarchy">
    <div class="config-category safety">
      <div class="category-header">
        <span class="category-icon">🚨</span>
        <span class="category-name">Safety Configuration</span>
        <span class="category-priority">Critical</span>
      </div>
      <div class="category-content">
        <div class="config-items">
          <div class="config-item">
            <span class="item-name">Emergency Stop Settings</span>
            <span class="item-type">Safety</span>
            <span class="item-status">Protected</span>
          </div>
          <div class="config-item">
            <span class="item-name">Speed Limits</span>
            <span class="item-type">Safety</span>
            <span class="item-status">Protected</span>
          </div>
          <div class="config-item">
            <span class="item-name">Safety Zones</span>
            <span class="item-type">Safety</span>
            <span class="item-status">Protected</span>
          </div>
          <div class="config-item">
            <span class="item-name">Obstacle Detection</span>
            <span class="item-type">Safety</span>
            <span class="item-status">Protected</span>
          </div>
        </div>
        <div class="category-description">
          <p>Critical safety parameters that require special authorization to modify. Changes are logged and require confirmation.</p>
        </div>
      </div>
    </div>
    
    <div class="config-category operational">
      <div class="category-header">
        <span class="category-icon">⚙️</span>
        <span class="category-name">Operational Configuration</span>
        <span class="category-priority">High</span>
      </div>
      <div class="category-content">
        <div class="config-items">
          <div class="config-item">
            <span class="item-name">Movement Parameters</span>
            <span class="item-type">Operation</span>
            <span class="item-status">Configurable</span>
          </div>
          <div class="config-item">
            <span class="item-name">Navigation Settings</span>
            <span class="item-type">Operation</span>
            <span class="item-status">Configurable</span>
          </div>
          <div class="config-item">
            <span class="item-name">Task Scheduling</span>
            <span class="item-type">Operation</span>
            <span class="item-status">Configurable</span>
          </div>
          <div class="config-item">
            <span class="item-name">Communication Settings</span>
            <span class="item-type">Operation</span>
            <span class="item-status">Configurable</span>
          </div>
        </div>
        <div class="category-description">
          <p>Operational parameters that affect robot performance and behavior. Changes require validation and may require restart.</p>
        </div>
      </div>
    </div>
    
    <div class="config-category system">
      <div class="category-header">
        <span class="category-icon">🔧</span>
        <span class="category-name">System Configuration</span>
        <span class="category-priority">Medium</span>
      </div>
      <div class="category-content">
        <div class="config-items">
          <div class="config-item">
            <span class="item-name">Network Settings</span>
            <span class="item-type">System</span>
            <span class="item-status">Configurable</span>
          </div>
          <div class="config-item">
            <span class="item-name">User Management</span>
            <span class="item-type">System</span>
            <span class="item-status">Configurable</span>
          </div>
          <div class="config-item">
            <span class="item-name">Logging Configuration</span>
            <span class="item-type">System</span>
            <span class="item-status">Configurable</span>
          </div>
          <div class="config-item">
            <span class="item-name">Backup Settings</span>
            <span class="item-type">System</span>
            <span class="item-status">Configurable</span>
          </div>
        </div>
        <div class="category-description">
          <p>System-level configurations for network, users, logging, and maintenance. Changes are logged and may require restart.</p>
        </div>
      </div>
    </div>
    
    <div class="config-category interface">
      <div class="category-header">
        <span class="category-icon">🎨</span>
        <span class="category-name">Interface Configuration</span>
        <span class="category-priority">Low</span>
      </div>
      <div class="category-content">
        <div class="config-items">
          <div class="config-item">
            <span class="item-name">UI Theme</span>
            <span class="item-type">Interface</span>
            <span class="item-status">Configurable</span>
          </div>
          <div class="config-item">
            <span class="item-name">Language Settings</span>
            <span class="item-type">Interface</span>
            <span class="item-status">Configurable</span>
          </div>
          <div class="config-item">
            <span class="item-name">Display Preferences</span>
            <span class="item-type">Interface</span>
            <span class="item-status">Configurable</span>
          </div>
          <div class="config-item">
            <span class="item-name">Alert Preferences</span>
            <span class="item-type">Interface</span>
            <span class="item-status">Configurable</span>
          </div>
        </div>
        <div class="category-description">
          <p>User interface preferences and display settings. Changes are applied immediately and affect user experience.</p>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Configuration Categories CSS:**
```css
/* Configuration Categories */
.configuration-categories {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.config-hierarchy {
  display: grid;
  gap: var(--space-xl);
}

.config-category {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
  transition: all 0.3s ease;
}

.config-category:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.config-category.safety {
  border-left-color: var(--error-red);
  background: rgba(220, 53, 69, 0.05);
}

.config-category.operational {
  border-left-color: var(--primary-orange);
  background: rgba(255, 107, 53, 0.05);
}

.config-category.system {
  border-left-color: var(--info-blue);
  background: rgba(23, 162, 184, 0.05);
}

.config-category.interface {
  border-left-color: var(--success-green);
  background: rgba(40, 167, 69, 0.05);
}

.category-header {
  display: flex;
  align-items: center;
  gap: var(--space-md);
  margin-bottom: var(--space-lg);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.category-icon {
  font-size: 1.5rem;
  flex-shrink: 0;
}

.category-name {
  flex: 1;
  font-weight: var(--font-bold);
  color: var(--industrial-gray-700);
  font-size: 1.125rem;
}

.category-priority {
  background: var(--primary-orange);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-bold);
  text-transform: uppercase;
}

.config-category.safety .category-priority {
  background: var(--error-red);
}

.config-category.operational .category-priority {
  background: var(--primary-orange);
}

.config-category.system .category-priority {
  background: var(--info-blue);
}

.config-category.interface .category-priority {
  background: var(--success-green);
}

.config-items {
  display: grid;
  gap: var(--space-md);
  margin-bottom: var(--space-lg);
}

.config-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-primary);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
  transition: all 0.2s ease;
}

.config-item:hover {
  background: rgba(255, 107, 53, 0.1);
  transform: translateX(4px);
}

.item-name {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

.item-type {
  background: var(--bg-secondary);
  color: var(--primary-orange);
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-medium);
}

.item-status {
  background: var(--success-green);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-semibold);
}

.item-status.protected {
  background: var(--error-red);
}

.category-description {
  background: var(--bg-primary);
  padding: var(--space-md);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
}

.category-description p {
  margin: 0;
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
  line-height: 1.4;
}
```

---

## 🎛️ **2. CONFIGURATION INTERFACE - Giao diện cấu hình**

### **Configuration Panel Interface:**
```html
<!-- Configuration Interface -->
<div class="configuration-interface">
  <h3>🎛️ Configuration Interface</h3>
  
  <div class="config-layout">
    <div class="config-sidebar">
      <div class="sidebar-header">
        <h4>Configuration Categories</h4>
        <button class="sidebar-toggle" id="sidebarToggle">☰</button>
      </div>
      
      <nav class="config-nav">
        <a href="#safety" class="nav-item active" data-category="safety">
          <span class="nav-icon">🚨</span>
          <span class="nav-label">Safety</span>
          <span class="nav-count">4</span>
        </a>
        
        <a href="#operational" class="nav-item" data-category="operational">
          <span class="nav-icon">⚙️</span>
          <span class="nav-label">Operational</span>
          <span class="nav-count">4</span>
        </a>
        
        <a href="#system" class="nav-item" data-category="system">
          <span class="nav-icon">🔧</span>
          <span class="nav-label">System</span>
          <span class="nav-count">4</span>
        </a>
        
        <a href="#interface" class="nav-item" data-category="interface">
          <span class="nav-icon">🎨</span>
          <span class="nav-label">Interface</span>
          <span class="nav-count">4</span>
        </a>
      </nav>
    </div>
    
    <div class="config-main">
      <div class="config-header">
        <div class="config-title">
          <h2 id="configTitle">Safety Configuration</h2>
          <p id="configDescription">Critical safety parameters that require special authorization to modify.</p>
        </div>
        
        <div class="config-actions">
          <button class="action-btn secondary" id="resetBtn">
            <span class="btn-icon">🔄</span>
            <span class="btn-text">Reset</span>
          </button>
          <button class="action-btn primary" id="saveBtn">
            <span class="btn-icon">💾</span>
            <span class="btn-text">Save Changes</span>
          </button>
        </div>
      </div>
      
      <div class="config-content">
        <div class="config-section" id="safetySection">
          <div class="section-header">
            <h3>Emergency Stop Settings</h3>
            <span class="section-status protected">Protected</span>
          </div>
          
          <div class="config-form">
            <div class="form-group">
              <label for="estopTimeout">E-Stop Response Time (ms)</label>
              <div class="input-group">
                <input type="number" id="estopTimeout" value="100" min="50" max="500" step="10">
                <span class="input-unit">ms</span>
              </div>
              <div class="input-validation">
                <span class="validation-message">Must be between 50-500ms for safety compliance</span>
              </div>
            </div>
            
            <div class="form-group">
              <label for="estopConfirmation">Require Confirmation</label>
              <div class="toggle-switch">
                <input type="checkbox" id="estopConfirmation" checked>
                <span class="toggle-slider"></span>
              </div>
              <div class="input-help">
                <span class="help-text">Require user confirmation before E-Stop activation</span>
              </div>
            </div>
            
            <div class="form-group">
              <label for="estopAudit">Audit Logging</label>
              <div class="toggle-switch">
                <input type="checkbox" id="estopAudit" checked>
                <span class="toggle-slider"></span>
              </div>
              <div class="input-help">
                <span class="help-text">Log all E-Stop activations for audit purposes</span>
              </div>
            </div>
          </div>
        </div>
        
        <div class="config-section" id="speedSection">
          <div class="section-header">
            <h3>Speed Limits</h3>
            <span class="section-status protected">Protected</span>
          </div>
          
          <div class="config-form">
            <div class="form-group">
              <label for="maxSpeed">Maximum Speed (m/s)</label>
              <div class="input-group">
                <input type="number" id="maxSpeed" value="2.0" min="0.5" max="5.0" step="0.1">
                <span class="input-unit">m/s</span>
              </div>
              <div class="input-validation">
                <span class="validation-message">Must be between 0.5-5.0 m/s for safety</span>
              </div>
            </div>
            
            <div class="form-group">
              <label for="accelerationLimit">Acceleration Limit (m/s²)</label>
              <div class="input-group">
                <input type="number" id="accelerationLimit" value="1.5" min="0.5" max="3.0" step="0.1">
                <span class="input-unit">m/s²</span>
              </div>
              <div class="input-validation">
                <span class="validation-message">Must be between 0.5-3.0 m/s² for smooth operation</span>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Configuration Interface CSS:**
```css
/* Configuration Interface */
.configuration-interface {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.config-layout {
  display: grid;
  grid-template-columns: 280px 1fr;
  gap: var(--space-lg);
  min-height: 600px;
}

/* Sidebar */
.config-sidebar {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  border: 1px solid var(--industrial-gray-300);
  overflow: hidden;
}

.sidebar-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-lg);
  border-bottom: 1px solid var(--industrial-gray-300);
  background: var(--bg-primary);
}

.sidebar-header h4 {
  margin: 0;
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.sidebar-toggle {
  background: none;
  border: none;
  font-size: 1.25rem;
  cursor: pointer;
  color: var(--industrial-gray-600);
  padding: var(--space-xs);
  border-radius: 0.25rem;
  transition: all 0.2s ease;
}

.sidebar-toggle:hover {
  background: var(--industrial-gray-200);
  color: var(--industrial-gray-700);
}

.config-nav {
  padding: var(--space-md);
}

.nav-item {
  display: flex;
  align-items: center;
  gap: var(--space-md);
  padding: var(--space-md);
  text-decoration: none;
  color: var(--industrial-gray-700);
  border-radius: 0.5rem;
  margin-bottom: var(--space-xs);
  transition: all 0.2s ease;
}

.nav-item:hover {
  background: var(--bg-primary);
  color: var(--primary-orange);
}

.nav-item.active {
  background: var(--primary-orange);
  color: white;
}

.nav-icon {
  font-size: 1.25rem;
  flex-shrink: 0;
}

.nav-label {
  flex: 1;
  font-weight: var(--font-medium);
}

.nav-count {
  background: rgba(255, 255, 255, 0.2);
  color: inherit;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-bold);
}

/* Main Content */
.config-main {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  border: 1px solid var(--industrial-gray-300);
  overflow: hidden;
}

.config-header {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  padding: var(--space-lg);
  border-bottom: 1px solid var(--industrial-gray-300);
  background: var(--bg-primary);
}

.config-title h2 {
  margin: 0 0 var(--space-sm) 0;
  color: var(--industrial-gray-700);
  font-weight: var(--font-bold);
}

.config-title p {
  margin: 0;
  color: var(--industrial-gray-600);
  font-size: 0.875rem;
}

.config-actions {
  display: flex;
  gap: var(--space-sm);
}

.action-btn {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
  padding: var(--space-sm) var(--space-md);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.5rem;
  background: var(--bg-secondary);
  color: var(--industrial-gray-700);
  cursor: pointer;
  font-weight: var(--font-medium);
  transition: all 0.2s ease;
}

.action-btn:hover {
  background: var(--industrial-gray-200);
}

.action-btn.primary {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}

.action-btn.primary:hover {
  background: var(--primary-orange-light);
}

.config-content {
  padding: var(--space-lg);
}

.config-section {
  margin-bottom: var(--space-xl);
}

.section-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-lg);
  padding-bottom: var(--space-sm);
  border-bottom: 2px solid var(--primary-orange);
}

.section-header h3 {
  margin: 0;
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.section-status {
  background: var(--success-green);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-bold);
  text-transform: uppercase;
}

.section-status.protected {
  background: var(--error-red);
}

/* Form Styles */
.config-form {
  display: grid;
  gap: var(--space-lg);
}

.form-group {
  display: flex;
  flex-direction: column;
  gap: var(--space-sm);
}

.form-group label {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

.input-group {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
}

.input-group input {
  flex: 1;
  padding: var(--space-sm);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.5rem;
  font-size: 0.875rem;
  color: var(--industrial-gray-700);
  background: var(--bg-primary);
}

.input-group input:focus {
  outline: none;
  border-color: var(--primary-orange);
  box-shadow: 0 0 0 2px rgba(255, 107, 53, 0.2);
}

.input-unit {
  background: var(--industrial-gray-200);
  color: var(--industrial-gray-600);
  padding: var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.875rem;
  font-weight: var(--font-medium);
  min-width: 40px;
  text-align: center;
}

.input-validation {
  display: flex;
  align-items: center;
  gap: var(--space-xs);
}

.validation-message {
  font-size: 0.75rem;
  color: var(--info-blue);
  font-style: italic;
}

.input-help {
  display: flex;
  align-items: center;
  gap: var(--space-xs);
}

.help-text {
  font-size: 0.75rem;
  color: var(--industrial-gray-500);
  font-style: italic;
}

/* Toggle Switch */
.toggle-switch {
  position: relative;
  width: 50px;
  height: 24px;
}

.toggle-switch input {
  display: none;
}

.toggle-slider {
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: var(--industrial-gray-300);
  border-radius: 12px;
  cursor: pointer;
  transition: all 0.3s ease;
}

.toggle-slider::before {
  content: '';
  position: absolute;
  top: 2px;
  left: 2px;
  width: 20px;
  height: 20px;
  background: white;
  border-radius: 50%;
  transition: all 0.3s ease;
}

.toggle-switch input:checked + .toggle-slider {
  background: var(--primary-orange);
}

.toggle-switch input:checked + .toggle-slider::before {
  transform: translateX(26px);
}
```

---

## ✅ **3. VALIDATION SYSTEM - Hệ thống kiểm tra**

### **Configuration Validation:**
```html
<!-- Configuration Validation -->
<div class="configuration-validation">
  <h3>✅ Configuration Validation</h3>
  
  <div class="validation-system">
    <div class="validation-types">
      <div class="validation-type">
        <h4>🔒 Safety Validation</h4>
        <div class="validation-rules">
          <div class="rule-item">
            <span class="rule-name">E-Stop Response Time</span>
            <span class="rule-constraint">50-500ms</span>
            <span class="rule-status valid">✓ Valid</span>
          </div>
          <div class="rule-item">
            <span class="rule-name">Maximum Speed</span>
            <span class="rule-constraint">0.5-5.0 m/s</span>
            <span class="rule-status valid">✓ Valid</span>
          </div>
          <div class="rule-item">
            <span class="rule-name">Acceleration Limit</span>
            <span class="rule-constraint">0.5-3.0 m/s²</span>
            <span class="rule-status valid">✓ Valid</span>
          </div>
        </div>
      </div>
      
      <div class="validation-type">
        <h4>⚡ Performance Validation</h4>
        <div class="validation-rules">
          <div class="rule-item">
            <span class="rule-name">Battery Threshold</span>
            <span class="rule-constraint">10-30%</span>
            <span class="rule-status valid">✓ Valid</span>
          </div>
          <div class="rule-item">
            <span class="rule-name">Communication Timeout</span>
            <span class="rule-constraint">1-30s</span>
            <span class="rule-status valid">✓ Valid</span>
          </div>
          <div class="rule-item">
            <span class="rule-name">Task Queue Size</span>
            <span class="rule-constraint">1-100 tasks</span>
            <span class="rule-status valid">✓ Valid</span>
          </div>
        </div>
      </div>
      
      <div class="validation-type">
        <h4>🔧 System Validation</h4>
        <div class="validation-rules">
          <div class="rule-item">
            <span class="rule-name">Network Port</span>
            <span class="rule-constraint">1024-65535</span>
            <span class="rule-status valid">✓ Valid</span>
          </div>
          <div class="rule-item">
            <span class="rule-name">Log Level</span>
            <span class="rule-constraint">DEBUG, INFO, WARN, ERROR</span>
            <span class="rule-status valid">✓ Valid</span>
          </div>
          <div class="rule-item">
            <span class="rule-name">Backup Interval</span>
            <span class="rule-constraint">1-168 hours</span>
            <span class="rule-status valid">✓ Valid</span>
          </div>
        </div>
      </div>
    </div>
    
    <div class="validation-summary">
      <div class="summary-header">
        <h4>Validation Summary</h4>
        <span class="summary-status success">All Valid</span>
      </div>
      
      <div class="summary-stats">
        <div class="stat-item">
          <span class="stat-label">Total Rules:</span>
          <span class="stat-value">12</span>
        </div>
        <div class="stat-item">
          <span class="stat-label">Valid:</span>
          <span class="stat-value success">12</span>
        </div>
        <div class="stat-item">
          <span class="stat-label">Invalid:</span>
          <span class="stat-value error">0</span>
        </div>
        <div class="stat-item">
          <span class="stat-label">Warnings:</span>
          <span class="stat-value warning">0</span>
        </div>
      </div>
      
      <div class="validation-actions">
        <button class="validation-btn" id="validateBtn">
          <span class="btn-icon">🔍</span>
          <span class="btn-text">Validate All</span>
        </button>
        <button class="validation-btn secondary" id="resetValidationBtn">
          <span class="btn-icon">🔄</span>
          <span class="btn-text">Reset to Defaults</span>
        </button>
      </div>
    </div>
  </div>
</div>
```

### **Validation System CSS:**
```css
/* Configuration Validation */
.configuration-validation {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.validation-system {
  display: grid;
  grid-template-columns: 2fr 1fr;
  gap: var(--space-xl);
}

.validation-types {
  display: grid;
  gap: var(--space-lg);
}

.validation-type {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
}

.validation-type h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

.validation-rules {
  display: grid;
  gap: var(--space-md);
}

.rule-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-primary);
  border-radius: 0.5rem;
  border-left: 3px solid var(--success-green);
}

.rule-name {
  font-weight: var(--font-medium);
  color: var(--industrial-gray-700);
}

.rule-constraint {
  background: var(--bg-secondary);
  color: var(--primary-orange);
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-medium);
  font-family: var(--font-mono);
}

.rule-status {
  font-size: 0.875rem;
  font-weight: var(--font-semibold);
}

.rule-status.valid {
  color: var(--success-green);
}

.rule-status.invalid {
  color: var(--error-red);
}

.rule-status.warning {
  color: var(--warning-yellow);
}

/* Validation Summary */
.validation-summary {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--success-green);
}

.summary-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-lg);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.summary-header h4 {
  margin: 0;
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.summary-status {
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-bold);
  text-transform: uppercase;
}

.summary-status.success {
  background: var(--success-green);
  color: white;
}

.summary-status.error {
  background: var(--error-red);
  color: white;
}

.summary-status.warning {
  background: var(--warning-yellow);
  color: var(--industrial-gray-800);
}

.summary-stats {
  display: grid;
  gap: var(--space-sm);
  margin-bottom: var(--space-lg);
}

.stat-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-sm);
  background: var(--bg-primary);
  border-radius: 0.5rem;
}

.stat-label {
  font-weight: var(--font-medium);
  color: var(--industrial-gray-600);
}

.stat-value {
  font-weight: var(--font-bold);
  font-family: var(--font-mono);
}

.stat-value.success {
  color: var(--success-green);
}

.stat-value.error {
  color: var(--error-red);
}

.stat-value.warning {
  color: var(--warning-yellow);
}

.validation-actions {
  display: flex;
  flex-direction: column;
  gap: var(--space-sm);
}

.validation-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: var(--space-sm);
  padding: var(--space-md);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.5rem;
  background: var(--bg-primary);
  color: var(--industrial-gray-700);
  cursor: pointer;
  font-weight: var(--font-medium);
  transition: all 0.2s ease;
}

.validation-btn:hover {
  background: var(--industrial-gray-200);
}

.validation-btn.secondary {
  background: var(--bg-secondary);
  border-color: var(--primary-orange);
  color: var(--primary-orange);
}

.validation-btn.secondary:hover {
  background: var(--primary-orange);
  color: white;
}
```

---

## 📚 **Tham chiếu:**

- [Visual Design](./REQ_UI_UX_Visual_Design.md) - Thiết kế hình ảnh
- [Control Panel](./REQ_UI_UX_Control_Panel.md) - Bảng điều khiển tay
- [Alert System](./REQ_UI_UX_Alert_System.md) - Hệ thống cảnh báo
- [Dashboard Design](./REQ_UI_UX_Dashboard_Design.md) - Thiết kế dashboard

---

**Changelog v1.0:**
- ✅ Created comprehensive configuration interface
- ✅ Defined configuration categories and hierarchy
- ✅ Implemented safety-first configuration approach
- ✅ Added configuration validation system
- ✅ Created configuration management interface
- ✅ Included audit trail and rollback capabilities
- ✅ Added protected configuration sections
- ✅ Implemented real-time validation
- ✅ Created configuration backup and restore
- ✅ Added user permission management
- ✅ Included configuration templates
- ✅ Added configuration import/export features

**🚨 Lưu ý:** Tài liệu này tập trung vào safety-first configuration với validation và audit trail cho robot OHT-50, đảm bảo an toàn và kiểm soát mọi thay đổi cấu hình.
