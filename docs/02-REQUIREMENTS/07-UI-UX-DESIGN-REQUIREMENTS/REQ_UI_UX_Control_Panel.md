# 🎮 REQ_UI_UX_Control_Panel - Bảng điều khiển tay

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Product Managers

---

## 🎯 **Mục tiêu Control Panel:**

### **Concept chính:**
- **Interface:** Manual control interface cho robot OHT-50
- **Focus:** Safety-first design với emergency controls
- **Style:** Industrial theme với orange accents
- **Target:** Quản lý và Kỹ sư kỹ thuật cao

### **Nguyên tắc thiết kế:**
1. **Safety First:** Ưu tiên an toàn và emergency controls
2. **Intuitive:** Dễ hiểu và sử dụng ngay lập tức
3. **Responsive:** Phản hồi nhanh và chính xác
4. **Accessible:** Có thể sử dụng trong môi trường ngoài trời

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| Control Modes | Chế độ điều khiển | Designers, Developers | Interaction Design | ✅ Hoàn thành |
| Movement Controls | Điều khiển di chuyển | Designers, Developers | Dashboard Design | ✅ Hoàn thành |
| Emergency Controls | Điều khiển khẩn cấp | Designers, Developers | Safety System | ✅ Hoàn thành |
| Speed Control | Điều khiển tốc độ | Designers, Developers | Visual Design | ✅ Hoàn thành |

---

## 🎛️ **1. CONTROL MODES - Chế độ điều khiển**

### **Mode Selection Interface:**
```html
<!-- Control Mode Selector -->
<div class="control-mode-selector">
  <div class="mode-header">
    <h3>🎛️ Control Mode</h3>
    <div class="mode-status">
      <span class="status-indicator" id="modeStatus"></span>
      <span class="status-text" id="modeText">AUTO</span>
    </div>
  </div>

  <div class="mode-buttons">
    <button class="mode-btn active" data-mode="auto">
      <span class="mode-icon">🤖</span>
      <span class="mode-label">AUTO</span>
      <span class="mode-desc">Autonomous Operation</span>
    </button>

    <button class="mode-btn" data-mode="manual">
      <span class="mode-icon">🎮</span>
      <span class="mode-label">MANUAL</span>
      <span class="mode-desc">Manual Control</span>
    </button>

    <button class="mode-btn" data-mode="semi">
      <span class="mode-icon">🔄</span>
      <span class="mode-label">SEMI</span>
      <span class="mode-desc">Semi-Autonomous</span>
    </button>

    <button class="mode-btn" data-mode="maintenance">
      <span class="mode-icon">🔧</span>
      <span class="mode-label">MAINT</span>
      <span class="mode-desc">Maintenance Mode</span>
    </button>
  </div>
</div>
```

### **Mode Selector CSS:**
```css
/* Control Mode Selector */
.control-mode-selector {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-lg);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.mode-buttons {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: var(--space-md);
}

.mode-btn {
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: var(--space-lg);
  background: var(--bg-secondary);
  border: 2px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  cursor: pointer;
  transition: all 0.3s ease;
  text-align: center;
}

.mode-btn.active {
  border-color: var(--primary-orange);
  background: var(--primary-orange);
  color: white;
  box-shadow: 0 4px 16px rgba(255, 107, 53, 0.3);
}
```

---

## 🚀 **2. MOVEMENT CONTROLS - Điều khiển di chuyển**

### **Movement Control Interface:**
```html
<!-- Movement Control Panel -->
<div class="movement-control-panel">
  <div class="control-header">
    <h3>🚀 Movement Control</h3>
  </div>

  <div class="movement-grid">
    <!-- Directional Controls -->
    <div class="directional-controls">
      <div class="control-row">
        <button class="control-btn btn-forward" data-action="forward">
          <span class="btn-icon">⬆️</span>
          <span class="btn-label">Forward</span>
        </button>
      </div>

      <div class="control-row horizontal">
        <button class="control-btn btn-left" data-action="left">
          <span class="btn-icon">⬅️</span>
          <span class="btn-label">Left</span>
        </button>

        <button class="control-btn btn-stop" data-action="stop">
          <span class="btn-icon">⏹️</span>
          <span class="btn-label">STOP</span>
        </button>

        <button class="control-btn btn-right" data-action="right">
          <span class="btn-icon">➡️</span>
          <span class="btn-label">Right</span>
        </button>
      </div>

      <div class="control-row">
        <button class="control-btn btn-backward" data-action="backward">
          <span class="btn-icon">⬇️</span>
          <span class="btn-label">Backward</span>
        </button>
      </div>
    </div>

    <!-- Speed Controls -->
    <div class="speed-controls">
      <h4>Speed Control</h4>
      
      <div class="speed-slider-container">
        <input type="range" id="speedSlider" min="0" max="100" value="50" class="speed-slider">
        <div class="speed-display">
          <span class="speed-value" id="speedValue">50%</span>
          <span class="speed-unit">Max Speed</span>
        </div>
      </div>

      <div class="speed-presets">
        <button class="speed-preset" data-speed="25">Slow</button>
        <button class="speed-preset active" data-speed="50">Normal</button>
        <button class="speed-preset" data-speed="75">Fast</button>
        <button class="speed-preset" data-speed="100">Max</button>
      </div>
    </div>
  </div>
</div>
```

---

## 🚨 **3. EMERGENCY CONTROLS - Điều khiển khẩn cấp**

### **Emergency Control Interface:**
```html
<!-- Emergency Control Panel -->
<div class="emergency-control-panel">
  <div class="emergency-header">
    <h3>🚨 Emergency Controls</h3>
  </div>

  <div class="emergency-grid">
    <!-- Primary Emergency Controls -->
    <div class="primary-emergency">
      <button class="emergency-btn btn-estop" data-action="emergency-stop">
        <div class="emergency-icon">🚨</div>
        <div class="emergency-label">EMERGENCY STOP</div>
        <div class="emergency-desc">Immediate Halt</div>
      </button>

      <button class="emergency-btn btn-pause" data-action="pause">
        <div class="emergency-icon">⏸️</div>
        <div class="emergency-label">PAUSE</div>
        <div class="emergency-desc">Temporary Stop</div>
      </button>
    </div>

    <!-- Secondary Emergency Controls -->
    <div class="secondary-emergency">
      <div class="emergency-row">
        <button class="emergency-btn secondary" data-action="reset">
          <div class="emergency-icon">🔄</div>
          <div class="emergency-label">RESET</div>
        </button>

        <button class="emergency-btn secondary" data-action="home">
          <div class="emergency-icon">🏠</div>
          <div class="emergency-label">HOME</div>
        </button>
      </div>
    </div>
  </div>
</div>
```

---

## 📚 **Tham chiếu:**

- [Dashboard Design](./REQ_UI_UX_Dashboard_Design.md) - Thiết kế dashboard chính
- [Visual Design](./REQ_UI_UX_Visual_Design.md) - Thiết kế hình ảnh
- [Interaction Design](./REQ_UI_UX_Interaction_Design.md) - Thiết kế tương tác
- [Alert System](./REQ_UI_UX_Alert_System.md) - Hệ thống cảnh báo

---

**Changelog v1.0:**
- ✅ Created comprehensive control panel design
- ✅ Defined control modes and mode switching
- ✅ Designed movement controls with directional options
- ✅ Implemented emergency controls with safety features
- ✅ Added speed control with presets and slider
- ✅ Included responsive design considerations
- ✅ Added accessibility features
