# 🚨 REQ_UI_UX_Alert_System - Hệ thống cảnh báo

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Product Managers

---

## 🎯 **Mục tiêu Alert System:**

### **Concept chính:**
- **System:** Comprehensive alert và notification system cho robot OHT-50
- **Focus:** Safety-first với clear hierarchy và immediate response
- **Style:** Industrial theme với color-coded severity levels
- **Target:** Quản lý và Kỹ sư kỹ thuật cao

### **Nguyên tắc thiết kế:**
1. **Safety Priority:** Ưu tiên cảnh báo an toàn và khẩn cấp
2. **Clear Hierarchy:** Phân cấp rõ ràng theo mức độ nghiêm trọng
3. **Immediate Response:** Phản hồi ngay lập tức cho critical alerts
4. **Non-intrusive:** Không làm gián đoạn workflow thông thường

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| Alert Types | Phân loại cảnh báo | Designers, Developers | Safety System | ✅ Hoàn thành |
| Display Methods | Phương thức hiển thị | Designers, Developers | Visual Design | ✅ Hoàn thành |
| User Interaction | Tương tác người dùng | Designers, Developers | Interaction Design | ✅ Hoàn thành |
| Alert Management | Quản lý cảnh báo | Designers, Developers | Dashboard Design | ✅ Hoàn thành |

---

## 🚨 **1. ALERT TYPES - Phân loại cảnh báo**

### **Alert Severity Levels:**
```html
<!-- Alert Severity Definitions -->
<div class="alert-severity-definitions">
  <div class="severity-level critical">
    <div class="severity-icon">🚨</div>
    <div class="severity-info">
      <h4>CRITICAL</h4>
      <p>Immediate action required. System safety compromised.</p>
      <ul>
        <li>E-Stop activation</li>
        <li>System failure</li>
        <li>Safety violation</li>
        <li>Emergency shutdown</li>
      </ul>
    </div>
  </div>

  <div class="severity-level warning">
    <div class="severity-icon">⚠️</div>
    <div class="severity-info">
      <h4>WARNING</h4>
      <p>Attention required. Potential issue detected.</p>
      <ul>
        <li>Low battery</li>
        <li>Maintenance due</li>
        <li>Performance degradation</li>
        <li>Connection issues</li>
      </ul>
    </div>
  </div>

  <div class="severity-level info">
    <div class="severity-icon">ℹ️</div>
    <div class="severity-info">
      <h4>INFO</h4>
      <p>Informational message. No action required.</p>
      <ul>
        <li>Task completion</li>
        <li>Status updates</li>
        <li>System notifications</li>
        <li>Configuration changes</li>
      </ul>
    </div>
  </div>

  <div class="severity-level success">
    <div class="severity-icon">✅</div>
    <div class="severity-info">
      <h4>SUCCESS</h4>
      <p>Positive confirmation. Operation completed.</p>
      <ul>
        <li>Task successful</li>
        <li>Connection restored</li>
        <li>Maintenance completed</li>
        <li>System ready</li>
      </ul>
    </div>
  </div>
</div>
```

### **Alert Categories:**
```css
/* Alert Severity Definitions */
.alert-severity-definitions {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.severity-level {
  display: flex;
  gap: var(--space-md);
  padding: var(--space-lg);
  border-radius: 0.75rem;
  border-left: 4px solid;
}

.severity-level.critical {
  background: rgba(220, 53, 69, 0.1);
  border-left-color: var(--error-red);
}

.severity-level.warning {
  background: rgba(255, 193, 7, 0.1);
  border-left-color: var(--warning-yellow);
}

.severity-level.info {
  background: rgba(23, 162, 184, 0.1);
  border-left-color: var(--info-blue);
}

.severity-level.success {
  background: rgba(40, 167, 69, 0.1);
  border-left-color: var(--success-green);
}

.severity-icon {
  font-size: 2rem;
  flex-shrink: 0;
}

.severity-info h4 {
  margin-bottom: var(--space-sm);
  font-weight: var(--font-bold);
}

.severity-info p {
  margin-bottom: var(--space-md);
  color: var(--industrial-gray-600);
}

.severity-info ul {
  list-style: none;
  padding: 0;
}

.severity-info li {
  padding: var(--space-xs) 0;
  color: var(--industrial-gray-700);
  font-size: 0.875rem;
}

.severity-info li::before {
  content: "•";
  color: var(--primary-orange);
  font-weight: bold;
  margin-right: var(--space-sm);
}
```

---

## 📢 **2. DISPLAY METHODS - Phương thức hiển thị**

### **Alert Banner System:**
```html
<!-- Alert Banner Container -->
<div class="alert-banner-container" id="alertBannerContainer">
  <!-- Critical Alert Banner -->
  <div class="alert-banner critical" id="criticalAlert">
    <div class="alert-content">
      <div class="alert-icon">🚨</div>
      <div class="alert-message">
        <h4>EMERGENCY STOP ACTIVATED</h4>
        <p>Robot has been stopped due to safety violation. Immediate attention required.</p>
      </div>
      <div class="alert-actions">
        <button class="alert-btn primary" data-action="acknowledge">ACKNOWLEDGE</button>
        <button class="alert-btn secondary" data-action="details">DETAILS</button>
      </div>
    </div>
    <button class="alert-close" data-action="close">×</button>
  </div>

  <!-- Warning Alert Banner -->
  <div class="alert-banner warning" id="warningAlert">
    <div class="alert-content">
      <div class="alert-icon">⚠️</div>
      <div class="alert-message">
        <h4>LOW BATTERY WARNING</h4>
        <p>Battery level is below 20%. Consider returning to charging station.</p>
      </div>
      <div class="alert-actions">
        <button class="alert-btn primary" data-action="dismiss">DISMISS</button>
        <button class="alert-btn secondary" data-action="view">VIEW STATUS</button>
      </div>
    </div>
    <button class="alert-close" data-action="close">×</button>
  </div>

  <!-- Info Alert Banner -->
  <div class="alert-banner info" id="infoAlert">
    <div class="alert-content">
      <div class="alert-icon">ℹ️</div>
      <div class="alert-message">
        <h4>TASK COMPLETED</h4>
        <p>Delivery task #1234 has been completed successfully.</p>
      </div>
      <div class="alert-actions">
        <button class="alert-btn secondary" data-action="dismiss">DISMISS</button>
      </div>
    </div>
    <button class="alert-close" data-action="close">×</button>
  </div>
</div>
```

### **Alert Banner CSS:**
```css
/* Alert Banner Container */
.alert-banner-container {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  z-index: 1000;
  pointer-events: none;
}

.alert-banner {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: var(--space-md) var(--space-lg);
  margin: var(--space-md);
  border-radius: 0.75rem;
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.15);
  pointer-events: all;
  animation: slideInDown 0.3s ease-out;
  transform-origin: top;
}

.alert-banner.critical {
  background: linear-gradient(135deg, var(--error-red) 0%, #c82333 100%);
  color: white;
  border-left: 4px solid #dc3545;
}

.alert-banner.warning {
  background: linear-gradient(135deg, var(--warning-yellow) 0%, #e0a800 100%);
  color: var(--industrial-gray-800);
  border-left: 4px solid #ffc107;
}

.alert-banner.info {
  background: linear-gradient(135deg, var(--info-blue) 0%, #138496 100%);
  color: white;
  border-left: 4px solid #17a2b8;
}

.alert-banner.success {
  background: linear-gradient(135deg, var(--success-green) 0%, #1e7e34 100%);
  color: white;
  border-left: 4px solid #28a745;
}

.alert-content {
  display: flex;
  align-items: center;
  gap: var(--space-md);
  flex: 1;
}

.alert-icon {
  font-size: 1.5rem;
  flex-shrink: 0;
}

.alert-message {
  flex: 1;
}

.alert-message h4 {
  margin: 0 0 var(--space-xs) 0;
  font-weight: var(--font-bold);
  font-size: 1rem;
}

.alert-message p {
  margin: 0;
  font-size: 0.875rem;
  opacity: 0.9;
}

.alert-actions {
  display: flex;
  gap: var(--space-sm);
  flex-shrink: 0;
}

.alert-btn {
  padding: var(--space-xs) var(--space-md);
  border: none;
  border-radius: 0.5rem;
  cursor: pointer;
  font-size: 0.875rem;
  font-weight: var(--font-medium);
  transition: all 0.2s ease;
}

.alert-btn.primary {
  background: rgba(255, 255, 255, 0.2);
  color: inherit;
  border: 1px solid rgba(255, 255, 255, 0.3);
}

.alert-btn.primary:hover {
  background: rgba(255, 255, 255, 0.3);
  transform: translateY(-1px);
}

.alert-btn.secondary {
  background: transparent;
  color: inherit;
  border: 1px solid rgba(255, 255, 255, 0.3);
}

.alert-btn.secondary:hover {
  background: rgba(255, 255, 255, 0.1);
  transform: translateY(-1px);
}

.alert-close {
  background: none;
  border: none;
  color: inherit;
  font-size: 1.5rem;
  cursor: pointer;
  padding: var(--space-xs);
  border-radius: 0.25rem;
  transition: all 0.2s ease;
  opacity: 0.7;
}

.alert-close:hover {
  opacity: 1;
  background: rgba(255, 255, 255, 0.1);
}

/* Alert Animations */
@keyframes slideInDown {
  from {
    transform: translateY(-100%);
    opacity: 0;
  }
  to {
    transform: translateY(0);
    opacity: 1;
  }
}

@keyframes slideOutUp {
  from {
    transform: translateY(0);
    opacity: 1;
  }
  to {
    transform: translateY(-100%);
    opacity: 0;
  }
}

.alert-banner.hiding {
  animation: slideOutUp 0.3s ease-in forwards;
}
```

---

## 🔔 **3. NOTIFICATION CENTER - Trung tâm thông báo**

### **Notification Center Interface:**
```html
<!-- Notification Center -->
<div class="notification-center" id="notificationCenter">
  <div class="notification-header">
    <h3>🔔 Notifications</h3>
    <div class="notification-controls">
      <button class="notification-btn" data-action="mark-all-read">Mark All Read</button>
      <button class="notification-btn" data-action="clear-all">Clear All</button>
    </div>
  </div>

  <div class="notification-filters">
    <button class="filter-btn active" data-filter="all">All</button>
    <button class="filter-btn" data-filter="critical">Critical</button>
    <button class="filter-btn" data-filter="warning">Warning</button>
    <button class="filter-btn" data-filter="info">Info</button>
    <button class="filter-btn" data-filter="success">Success</button>
  </div>

  <div class="notification-list" id="notificationList">
    <!-- Critical Notification -->
    <div class="notification-item critical unread" data-id="1">
      <div class="notification-icon">🚨</div>
      <div class="notification-content">
        <div class="notification-header">
          <h4>Emergency Stop Activated</h4>
          <span class="notification-time">2 min ago</span>
        </div>
        <p>Robot stopped due to obstacle detection in path.</p>
        <div class="notification-actions">
          <button class="action-btn" data-action="acknowledge">Acknowledge</button>
          <button class="action-btn" data-action="view-details">View Details</button>
        </div>
      </div>
      <button class="notification-close" data-action="close">×</button>
    </div>

    <!-- Warning Notification -->
    <div class="notification-item warning unread" data-id="2">
      <div class="notification-icon">⚠️</div>
      <div class="notification-content">
        <div class="notification-header">
          <h4>Low Battery Warning</h4>
          <span class="notification-time">5 min ago</span>
        </div>
        <p>Battery level is at 15%. Consider charging soon.</p>
        <div class="notification-actions">
          <button class="action-btn" data-action="dismiss">Dismiss</button>
          <button class="action-btn" data-action="view-status">View Status</button>
        </div>
      </div>
      <button class="notification-close" data-action="close">×</button>
    </div>

    <!-- Info Notification -->
    <div class="notification-item info" data-id="3">
      <div class="notification-icon">ℹ️</div>
      <div class="notification-content">
        <div class="notification-header">
          <h4>Task Completed</h4>
          <span class="notification-time">10 min ago</span>
        </div>
        <p>Delivery task #1234 completed successfully.</p>
        <div class="notification-actions">
          <button class="action-btn" data-action="dismiss">Dismiss</button>
        </div>
      </div>
      <button class="notification-close" data-action="close">×</button>
    </div>
  </div>

  <div class="notification-footer">
    <span class="notification-count">3 notifications</span>
    <button class="notification-btn" data-action="load-more">Load More</button>
  </div>
</div>
```

### **Notification Center CSS:**
```css
/* Notification Center */
.notification-center {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.1);
  max-width: 500px;
  max-height: 600px;
  overflow: hidden;
}

.notification-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-lg);
  border-bottom: 1px solid var(--industrial-gray-300);
  background: var(--bg-secondary);
}

.notification-header h3 {
  margin: 0;
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.notification-controls {
  display: flex;
  gap: var(--space-sm);
}

.notification-btn {
  padding: var(--space-xs) var(--space-sm);
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.25rem;
  cursor: pointer;
  font-size: 0.875rem;
  transition: all 0.2s ease;
}

.notification-btn:hover {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}

/* Notification Filters */
.notification-filters {
  display: flex;
  padding: var(--space-md);
  gap: var(--space-xs);
  border-bottom: 1px solid var(--industrial-gray-300);
  background: var(--bg-secondary);
}

.filter-btn {
  padding: var(--space-xs) var(--space-sm);
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.25rem;
  cursor: pointer;
  font-size: 0.875rem;
  transition: all 0.2s ease;
}

.filter-btn.active {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}

.filter-btn:hover:not(.active) {
  background: var(--industrial-gray-200);
}

/* Notification List */
.notification-list {
  max-height: 400px;
  overflow-y: auto;
}

.notification-item {
  display: flex;
  gap: var(--space-md);
  padding: var(--space-md);
  border-bottom: 1px solid var(--industrial-gray-200);
  transition: all 0.2s ease;
  position: relative;
}

.notification-item:hover {
  background: var(--bg-secondary);
}

.notification-item.unread {
  background: rgba(255, 107, 53, 0.05);
  border-left: 3px solid var(--primary-orange);
}

.notification-item.critical {
  border-left: 3px solid var(--error-red);
}

.notification-item.warning {
  border-left: 3px solid var(--warning-yellow);
}

.notification-item.info {
  border-left: 3px solid var(--info-blue);
}

.notification-item.success {
  border-left: 3px solid var(--success-green);
}

.notification-icon {
  font-size: 1.5rem;
  flex-shrink: 0;
  margin-top: var(--space-xs);
}

.notification-content {
  flex: 1;
}

.notification-header {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  margin-bottom: var(--space-xs);
}

.notification-header h4 {
  margin: 0;
  font-size: 1rem;
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

.notification-time {
  font-size: 0.75rem;
  color: var(--industrial-gray-500);
  flex-shrink: 0;
}

.notification-content p {
  margin: 0 0 var(--space-sm) 0;
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
  line-height: 1.4;
}

.notification-actions {
  display: flex;
  gap: var(--space-xs);
}

.action-btn {
  padding: var(--space-xs) var(--space-sm);
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.25rem;
  cursor: pointer;
  font-size: 0.75rem;
  transition: all 0.2s ease;
}

.action-btn:hover {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}

.notification-close {
  background: none;
  border: none;
  color: var(--industrial-gray-500);
  font-size: 1.25rem;
  cursor: pointer;
  padding: var(--space-xs);
  border-radius: 0.25rem;
  transition: all 0.2s ease;
  align-self: flex-start;
}

.notification-close:hover {
  background: var(--industrial-gray-200);
  color: var(--industrial-gray-700);
}

/* Notification Footer */
.notification-footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  border-top: 1px solid var(--industrial-gray-300);
  background: var(--bg-secondary);
}

.notification-count {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
}
```

---

## 🎵 **4. AUDIO NOTIFICATIONS - Thông báo âm thanh**

### **Audio Alert System:**
```html
<!-- Audio Notification System -->
<div class="audio-notification-system">
  <div class="audio-controls">
    <h4>🔊 Audio Alerts</h4>
    <div class="audio-settings">
      <label class="audio-toggle">
        <input type="checkbox" id="audioEnabled" checked>
        <span class="toggle-slider"></span>
        <span class="toggle-label">Enable Audio</span>
      </label>
      
      <div class="volume-control">
        <label for="audioVolume">Volume:</label>
        <input type="range" id="audioVolume" min="0" max="100" value="70">
        <span class="volume-value">70%</span>
      </div>
    </div>
  </div>

  <div class="audio-test">
    <h5>Test Audio Alerts:</h5>
    <div class="test-buttons">
      <button class="test-btn critical" data-sound="critical">Test Critical</button>
      <button class="test-btn warning" data-sound="warning">Test Warning</button>
      <button class="test-btn info" data-sound="info">Test Info</button>
      <button class="test-btn success" data-sound="success">Test Success</button>
    </div>
  </div>
</div>
```

### **Audio System CSS:**
```css
/* Audio Notification System */
.audio-notification-system {
  background: var(--bg-secondary);
  padding: var(--space-lg);
  border-radius: 0.75rem;
  margin-top: var(--space-lg);
}

.audio-controls h4 {
  margin-bottom: var(--space-md);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.audio-settings {
  display: flex;
  flex-direction: column;
  gap: var(--space-md);
}

.audio-toggle {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
  cursor: pointer;
}

.audio-toggle input {
  display: none;
}

.toggle-slider {
  position: relative;
  width: 50px;
  height: 24px;
  background: var(--industrial-gray-300);
  border-radius: 12px;
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

.audio-toggle input:checked + .toggle-slider {
  background: var(--primary-orange);
}

.audio-toggle input:checked + .toggle-slider::before {
  transform: translateX(26px);
}

.toggle-label {
  font-size: 0.875rem;
  color: var(--industrial-gray-700);
  font-weight: var(--font-medium);
}

.volume-control {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
}

.volume-control label {
  font-size: 0.875rem;
  color: var(--industrial-gray-700);
  font-weight: var(--font-medium);
  min-width: 60px;
}

.volume-control input[type="range"] {
  flex: 1;
  height: 6px;
  border-radius: 3px;
  background: var(--industrial-gray-300);
  outline: none;
  -webkit-appearance: none;
}

.volume-control input[type="range"]::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 16px;
  height: 16px;
  border-radius: 50%;
  background: var(--primary-orange);
  cursor: pointer;
}

.volume-value {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
  min-width: 40px;
  text-align: right;
}

/* Audio Test */
.audio-test {
  margin-top: var(--space-lg);
  padding-top: var(--space-lg);
  border-top: 1px solid var(--industrial-gray-300);
}

.audio-test h5 {
  margin-bottom: var(--space-md);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.test-buttons {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
  gap: var(--space-sm);
}

.test-btn {
  padding: var(--space-sm) var(--space-md);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.5rem;
  background: var(--bg-primary);
  cursor: pointer;
  font-size: 0.875rem;
  font-weight: var(--font-medium);
  transition: all 0.2s ease;
}

.test-btn:hover {
  transform: translateY(-1px);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.test-btn.critical {
  border-color: var(--error-red);
  color: var(--error-red);
}

.test-btn.critical:hover {
  background: var(--error-red);
  color: white;
}

.test-btn.warning {
  border-color: var(--warning-yellow);
  color: var(--warning-yellow);
}

.test-btn.warning:hover {
  background: var(--warning-yellow);
  color: var(--industrial-gray-800);
}

.test-btn.info {
  border-color: var(--info-blue);
  color: var(--info-blue);
}

.test-btn.info:hover {
  background: var(--info-blue);
  color: white;
}

.test-btn.success {
  border-color: var(--success-green);
  color: var(--success-green);
}

.test-btn.success:hover {
  background: var(--success-green);
  color: white;
}
```

---

## 📚 **Tham chiếu:**

- [Dashboard Design](./REQ_UI_UX_Dashboard_Design.md) - Thiết kế dashboard chính
- [Control Panel](./REQ_UI_UX_Control_Panel.md) - Bảng điều khiển tay
- [Visual Design](./REQ_UI_UX_Visual_Design.md) - Thiết kế hình ảnh
- [Interaction Design](./REQ_UI_UX_Interaction_Design.md) - Thiết kế tương tác

---

**Changelog v1.0:**
- ✅ Created comprehensive alert system design
- ✅ Defined alert severity levels and categories
- ✅ Designed alert banner system with animations
- ✅ Implemented notification center with filtering
- ✅ Added audio notification system
- ✅ Included user interaction patterns
- ✅ Added accessibility considerations
- ✅ Created responsive design for all alert components
