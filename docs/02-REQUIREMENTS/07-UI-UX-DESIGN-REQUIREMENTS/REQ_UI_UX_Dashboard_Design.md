# 📊 REQ_UI_UX_Dashboard_Design - Thiết kế Dashboard chính

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Product Managers

---

## 🎯 **Mục tiêu Dashboard:**

### **Concept chính:**
- **Layout:** Multi-panel dashboard như trung tâm điều khiển
- **Focus:** Real-time monitoring và emergency control
- **Style:** Industrial theme với orange accents
- **Target:** Quản lý và Kỹ sư kỹ thuật cao

### **Nguyên tắc thiết kế:**
1. **Real-time:** Hiển thị dữ liệu thời gian thực
2. **Safety:** Ưu tiên thông tin an toàn và cảnh báo
3. **Efficiency:** Tối ưu cho thao tác nhanh
4. **Clarity:** Rõ ràng trong môi trường ngoài trời

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| Layout Structure | Cấu trúc layout chính | Designers, Developers | Information Architecture | ✅ Hoàn thành |
| Status Panel | Panel trạng thái robot | Designers, Developers | Visual Design | ✅ Hoàn thành |
| Control Panel | Bảng điều khiển tay | Designers, Developers | Interaction Design | ✅ Hoàn thành |
| Map Interface | Giao diện bản đồ | Designers, Developers | Map Interface | ✅ Hoàn thành |

---

## 🏗️ **1. LAYOUT STRUCTURE - Cấu trúc layout**

### **Grid Layout (12-column):**
```css
/* Main Dashboard Grid */
.dashboard-container {
  display: grid;
  grid-template-columns: repeat(12, 1fr);
  grid-template-rows: auto 1fr auto;
  gap: var(--space-lg);
  height: 100vh;
  padding: var(--space-lg);
}

/* Header Section */
.dashboard-header {
  grid-column: 1 / -1;
  grid-row: 1;
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-primary);
  border-radius: 0.75rem;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

/* Main Content Area */
.dashboard-main {
  grid-column: 1 / -1;
  grid-row: 2;
  display: grid;
  grid-template-columns: 3fr 1fr;
  gap: var(--space-lg);
}

/* Footer Section */
.dashboard-footer {
  grid-column: 1 / -1;
  grid-row: 3;
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-secondary);
  border-radius: 0.75rem;
}
```

### **Responsive Breakpoints:**
```css
/* Mobile (0-768px) */
@media (max-width: 768px) {
  .dashboard-main {
    grid-template-columns: 1fr;
    grid-template-rows: auto auto;
  }
  
  .dashboard-header {
    flex-direction: column;
    gap: var(--space-sm);
  }
}

/* Tablet (768px-1200px) */
@media (min-width: 769px) and (max-width: 1200px) {
  .dashboard-main {
    grid-template-columns: 2fr 1fr;
  }
}

/* Desktop (1200px+) */
@media (min-width: 1201px) {
  .dashboard-main {
    grid-template-columns: 3fr 1fr;
  }
}
```

---

## 🤖 **2. STATUS PANEL - Panel trạng thái robot**

### **Robot Status Overview:**
```html
<!-- Robot Status Card -->
<div class="status-card robot-status">
  <div class="status-header">
    <h3>🤖 Robot OHT-50 Status</h3>
    <div class="status-indicator online"></div>
  </div>
  
  <div class="status-grid">
    <!-- Operating Status -->
    <div class="status-item">
      <span class="status-label">Operating Mode:</span>
      <span class="status-value active">AUTO</span>
    </div>
    
    <!-- Battery Level -->
    <div class="status-item">
      <span class="status-label">Battery:</span>
      <div class="battery-indicator">
        <div class="battery-level" style="width: 85%"></div>
        <span class="battery-text">85%</span>
      </div>
    </div>
    
    <!-- Speed -->
    <div class="status-item">
      <span class="status-label">Current Speed:</span>
      <span class="status-value">2.5 m/s</span>
    </div>
    
    <!-- Position -->
    <div class="status-item">
      <span class="status-label">Position:</span>
      <span class="status-value">X: 15.2m, Y: 8.7m</span>
    </div>
    
    <!-- Connection -->
    <div class="status-item">
      <span class="status-label">Connection:</span>
      <span class="status-value connected">WiFi Connected</span>
    </div>
    
    <!-- Temperature -->
    <div class="status-item">
      <span class="status-label">Temperature:</span>
      <span class="status-value">42°C</span>
    </div>
  </div>
</div>
```

### **Status Indicators CSS:**
```css
/* Status Card Styles */
.status-card {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.status-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-md);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.status-indicator {
  width: 12px;
  height: 12px;
  border-radius: 50%;
  animation: pulse 2s infinite;
}

.status-indicator.online {
  background: var(--success-green);
  box-shadow: 0 0 8px rgba(40, 167, 69, 0.5);
}

.status-indicator.offline {
  background: var(--error-red);
  box-shadow: 0 0 8px rgba(220, 53, 69, 0.5);
}

.status-indicator.warning {
  background: var(--warning-yellow);
  box-shadow: 0 0 8px rgba(255, 193, 7, 0.5);
}

/* Status Grid */
.status-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: var(--space-md);
}

.status-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-sm);
  background: var(--bg-secondary);
  border-radius: 0.5rem;
}

.status-label {
  font-weight: var(--font-medium);
  color: var(--industrial-gray-600);
}

.status-value {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

.status-value.connected {
  color: var(--success-green);
}

.status-value.error {
  color: var(--error-red);
}

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
  font-size: 0.75rem;
  font-weight: var(--font-bold);
  color: white;
  text-shadow: 0 1px 2px rgba(0, 0, 0, 0.5);
}

/* Pulse Animation */
@keyframes pulse {
  0% { opacity: 1; }
  50% { opacity: 0.5; }
  100% { opacity: 1; }
}
```

---

## 🎮 **3. CONTROL PANEL - Bảng điều khiển tay**

### **Manual Control Interface:**
```html
<!-- Manual Control Panel -->
<div class="control-panel">
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
          <span class="btn-icon">⬆️</span>
          <span class="btn-label">Forward</span>
        </button>
        
        <div class="horizontal-controls">
          <button class="control-btn btn-left" data-action="left">
            <span class="btn-icon">⬅️</span>
            <span class="btn-label">Left</span>
          </button>
          
          <button class="control-btn btn-stop" data-action="stop">
            <span class="btn-icon">⏹️</span>
            <span class="btn-label">Stop</span>
          </button>
          
          <button class="control-btn btn-right" data-action="right">
            <span class="btn-icon">➡️</span>
            <span class="btn-label">Right</span>
          </button>
        </div>
        
        <button class="control-btn btn-backward" data-action="backward">
          <span class="btn-icon">⬇️</span>
          <span class="btn-label">Backward</span>
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
      </div>
    </div>
    
    <!-- Emergency Controls -->
    <div class="control-section emergency">
      <h4>Emergency</h4>
      <div class="emergency-controls">
        <button class="control-btn btn-emergency" data-action="emergency-stop">
          <span class="btn-icon">🚨</span>
          <span class="btn-label">E-STOP</span>
        </button>
        
        <button class="control-btn btn-reset" data-action="reset">
          <span class="btn-icon">🔄</span>
          <span class="btn-label">Reset</span>
        </button>
      </div>
    </div>
  </div>
</div>
```

### **Control Panel CSS:**
```css
/* Control Panel Styles */
.control-panel {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
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

/* Control Grid */
.control-grid {
  display: grid;
  gap: var(--space-lg);
}

.control-section {
  padding: var(--space-md);
  background: var(--bg-secondary);
  border-radius: 0.5rem;
}

.control-section h4 {
  margin-bottom: var(--space-md);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
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
  border-radius: 0.5rem;
  cursor: pointer;
  transition: all 0.2s ease;
  font-weight: var(--font-medium);
}

.control-btn:hover {
  border-color: var(--primary-orange);
  background: rgba(255, 107, 53, 0.1);
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.control-btn:active {
  transform: translateY(0);
  box-shadow: 0 2px 6px rgba(255, 107, 53, 0.3);
}

.btn-icon {
  font-size: 1.5rem;
  margin-bottom: var(--space-xs);
}

.btn-label {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
}

/* Emergency Button */
.btn-emergency {
  background: var(--error-red) !important;
  color: white !important;
  border-color: var(--error-red) !important;
  animation: emergency-pulse 1s infinite;
}

.btn-emergency:hover {
  background: var(--error-red-light) !important;
  border-color: var(--error-red-light) !important;
}

@keyframes emergency-pulse {
  0% { box-shadow: 0 0 0 0 rgba(220, 53, 69, 0.7); }
  70% { box-shadow: 0 0 0 10px rgba(220, 53, 69, 0); }
  100% { box-shadow: 0 0 0 0 rgba(220, 53, 69, 0); }
}

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
  box-shadow: 0 2px 6px rgba(255, 107, 53, 0.3);
}

.speed-display {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.speed-value {
  font-size: 1.5rem;
  font-weight: var(--font-bold);
  color: var(--primary-orange);
}

.speed-unit {
  font-size: 0.875rem;
  color: var(--industrial-gray-500);
}
```

---

## 🗺️ **4. MAP INTERFACE - Giao diện bản đồ**

### **LiDAR 360° Visualization:**
```html
<!-- Map Interface -->
<div class="map-interface">
  <div class="map-header">
    <h3>🗺️ Robot Navigation Map</h3>
    <div class="map-controls">
      <button class="map-btn" data-action="zoom-in">🔍+</button>
      <button class="map-btn" data-action="zoom-out">🔍-</button>
      <button class="map-btn" data-action="reset-view">🔄</button>
      <button class="map-btn" data-action="fullscreen">⛶</button>
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
      <!-- Dynamic obstacle markers will be added here -->
    </div>
    
    <!-- Path Visualization -->
    <svg class="path-overlay" id="pathOverlay">
      <!-- Dynamic path lines will be added here -->
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
</div>
```

### **Map Interface CSS:**
```css
/* Map Interface Styles */
.map-interface {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  overflow: hidden;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
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
  border-radius: 0.5rem;
  cursor: pointer;
  transition: all 0.2s ease;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1rem;
}

.map-btn:hover {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}

/* Map Container */
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

/* Robot Indicator */
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

.robot-direction {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  width: 0;
  height: 0;
  border-left: 8px solid transparent;
  border-right: 8px solid transparent;
  border-bottom: 16px solid var(--primary-orange);
  margin-top: -20px;
}

/* Obstacle Markers */
.obstacle-markers {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  pointer-events: none;
}

.obstacle-marker {
  position: absolute;
  width: 8px;
  height: 8px;
  background: var(--error-red);
  border-radius: 50%;
  border: 2px solid white;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);
}

/* Path Overlay */
.path-overlay {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  pointer-events: none;
}

.path-line {
  stroke: var(--primary-orange);
  stroke-width: 3;
  stroke-dasharray: 5,5;
  animation: path-animation 2s linear infinite;
  fill: none;
}

@keyframes path-animation {
  0% { stroke-dashoffset: 0; }
  100% { stroke-dashoffset: 10; }
}

/* Map Info */
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
  font-size: 0.875rem;
  color: var(--industrial-gray-500);
  font-weight: var(--font-medium);
}

.info-value {
  font-size: 1rem;
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}
```

---

## 📚 **Tham chiếu:**

- [Visual Design](./REQ_UI_UX_Visual_Design.md) - Thiết kế hình ảnh
- [Information Architecture](./REQ_UI_UX_Information_Architecture.md) - Kiến trúc thông tin
- [Map Interface](./REQ_UI_UX_Map_Interface.md) - Giao diện bản đồ chi tiết
- [Control Panel](./REQ_UI_UX_Control_Panel.md) - Bảng điều khiển chi tiết

---

**Changelog v1.0:**
- ✅ Created comprehensive dashboard design
- ✅ Defined layout structure with responsive grid
- ✅ Designed robot status panel with real-time indicators
- ✅ Created manual control panel with emergency controls
- ✅ Integrated LiDAR 360° map interface
- ✅ Added accessibility and touch-friendly design
- ✅ Included responsive breakpoints for all devices
