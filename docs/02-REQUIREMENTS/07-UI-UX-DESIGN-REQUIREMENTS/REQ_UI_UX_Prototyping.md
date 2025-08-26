# 🎨 REQ_UI_UX_Prototyping - Tạo mẫu thử nghiệm

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Product Managers, Stakeholders

---

## 🎯 **Mục tiêu Prototyping:**

### **Concept chính:**
- **System:** Comprehensive prototyping framework cho robot OHT-50
- **Focus:** Rapid iteration với safety-first validation
- **Style:** Industrial theme với interactive prototypes
- **Target:** Validate design concepts trước khi implement

### **Nguyên tắc thiết kế:**
1. **Rapid Iteration:** Tạo mẫu nhanh để test ideas
2. **Safety Validation:** Ưu tiên kiểm thử tính năng an toàn
3. **User Feedback:** Thu thập feedback từ stakeholders
4. **Technical Feasibility:** Đảm bảo khả thi về mặt kỹ thuật

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| Prototype Types | Loại mẫu thử nghiệm | Designers, Developers | Design Strategy | ✅ Hoàn thành |
| Prototyping Tools | Công cụ tạo mẫu | Designers, Developers | Technical Stack | ✅ Hoàn thành |
| Interactive Prototypes | Mẫu tương tác | Designers, Stakeholders | User Experience | ✅ Hoàn thành |
| Prototype Testing | Kiểm thử mẫu | QA Engineers, Users | Validation Process | ✅ Hoàn thành |

---

## 🎭 **1. PROTOTYPE TYPES - Loại mẫu thử nghiệm**

### **Prototype Fidelity Levels:**
```html
<!-- Prototype Fidelity Overview -->
<div class="prototype-fidelity">
  <h3>🎭 Prototype Fidelity Levels</h3>
  
  <div class="fidelity-levels">
    <div class="fidelity-item low">
      <div class="fidelity-header">
        <span class="fidelity-level">Low Fidelity</span>
        <span class="fidelity-time">1-2 days</span>
      </div>
      <div class="fidelity-content">
        <h4>📝 Paper Sketches & Wireframes</h4>
        <p>Quick sketches and basic wireframes for concept validation</p>
        <div class="fidelity-features">
          <span class="feature">Paper sketches</span>
          <span class="feature">Basic wireframes</span>
          <span class="feature">User flow diagrams</span>
          <span class="feature">Concept validation</span>
        </div>
        <div class="fidelity-use">
          <strong>Use for:</strong> Early concept exploration, stakeholder alignment
        </div>
      </div>
    </div>
    
    <div class="fidelity-item medium">
      <div class="fidelity-header">
        <span class="fidelity-level">Medium Fidelity</span>
        <span class="fidelity-time">3-5 days</span>
      </div>
      <div class="fidelity-content">
        <h4>🎨 Digital Mockups</h4>
        <p>Digital designs with basic interactions and visual styling</p>
        <div class="fidelity-features">
          <span class="feature">Digital mockups</span>
          <span class="feature">Basic interactions</span>
          <span class="feature">Color schemes</span>
          <span class="feature">Typography</span>
        </div>
        <div class="fidelity-use">
          <strong>Use for:</strong> Design direction, user testing preparation
        </div>
      </div>
    </div>
    
    <div class="fidelity-item high">
      <div class="fidelity-header">
        <span class="fidelity-level">High Fidelity</span>
        <span class="fidelity-time">1-2 weeks</span>
      </div>
      <div class="fidelity-content">
        <h4>⚡ Interactive Prototypes</h4>
        <p>Fully interactive prototypes with realistic functionality</p>
        <div class="fidelity-features">
          <span class="feature">Interactive prototypes</span>
          <span class="feature">Real data integration</span>
          <span class="feature">Animation & transitions</span>
          <span class="feature">User testing ready</span>
        </div>
        <div class="fidelity-use">
          <strong>Use for:</strong> User testing, stakeholder demos, development handoff
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Prototype Types CSS:**
```css
/* Prototype Fidelity */
.prototype-fidelity {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.fidelity-levels {
  display: grid;
  gap: var(--space-xl);
}

.fidelity-item {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
  transition: all 0.3s ease;
}

.fidelity-item:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.fidelity-item.low {
  border-left-color: var(--warning-yellow);
}

.fidelity-item.medium {
  border-left-color: var(--primary-orange);
}

.fidelity-item.high {
  border-left-color: var(--success-green);
}

.fidelity-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-md);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.fidelity-level {
  font-weight: var(--font-bold);
  color: var(--industrial-gray-700);
  font-size: 1.125rem;
}

.fidelity-time {
  background: var(--primary-orange);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.875rem;
  font-weight: var(--font-semibold);
}

.fidelity-item.low .fidelity-time {
  background: var(--warning-yellow);
  color: var(--industrial-gray-800);
}

.fidelity-item.medium .fidelity-time {
  background: var(--primary-orange);
}

.fidelity-item.high .fidelity-time {
  background: var(--success-green);
}

.fidelity-content h4 {
  margin-bottom: var(--space-sm);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.fidelity-content p {
  margin-bottom: var(--space-md);
  color: var(--industrial-gray-600);
  font-size: 0.875rem;
  line-height: 1.4;
}

.fidelity-features {
  display: flex;
  flex-wrap: wrap;
  gap: var(--space-xs);
  margin-bottom: var(--space-md);
}

.feature {
  background: var(--bg-primary);
  color: var(--primary-orange);
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-medium);
  border: 1px solid var(--industrial-gray-300);
}

.fidelity-use {
  background: rgba(255, 107, 53, 0.1);
  padding: var(--space-sm);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
}

.fidelity-use strong {
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}
```

---

## 🛠️ **2. PROTOTYPING TOOLS - Công cụ tạo mẫu**

### **Tool Selection Matrix:**
```html
<!-- Prototyping Tools -->
<div class="prototyping-tools">
  <h3>🛠️ Prototyping Tools</h3>
  
  <div class="tools-matrix">
    <div class="tool-category">
      <h4>🎨 Design Tools</h4>
      
      <div class="tools-grid">
        <div class="tool-item">
          <div class="tool-header">
            <span class="tool-name">Figma</span>
            <span class="tool-type">Design & Prototyping</span>
          </div>
          <div class="tool-features">
            <span class="feature-tag">Collaborative</span>
            <span class="feature-tag">Interactive</span>
            <span class="feature-tag">Real-time</span>
          </div>
          <div class="tool-pros-cons">
            <div class="pros">
              <strong>Pros:</strong>
              <ul>
                <li>Real-time collaboration</li>
                <li>Rich prototyping features</li>
                <li>Component library support</li>
              </ul>
            </div>
            <div class="cons">
              <strong>Cons:</strong>
              <ul>
                <li>Requires internet connection</li>
                <li>Learning curve for advanced features</li>
              </ul>
            </div>
          </div>
        </div>
        
        <div class="tool-item">
          <div class="tool-header">
            <span class="tool-name">Adobe XD</span>
            <span class="tool-type">Design & Prototyping</span>
          </div>
          <div class="tool-features">
            <span class="feature-tag">Advanced</span>
            <span class="feature-tag">Offline</span>
            <span class="feature-tag">Integration</span>
          </div>
          <div class="tool-pros-cons">
            <div class="pros">
              <strong>Pros:</strong>
              <ul>
                <li>Advanced prototyping</li>
                <li>Adobe ecosystem integration</li>
                <li>Offline capability</li>
              </ul>
            </div>
            <div class="cons">
              <strong>Cons:</strong>
              <ul>
                <li>Subscription required</li>
                <li>Limited collaboration</li>
              </ul>
            </div>
          </div>
        </div>
        
        <div class="tool-item">
          <div class="tool-header">
            <span class="tool-name">Sketch</span>
            <span class="tool-type">Design</span>
          </div>
          <div class="tool-features">
            <span class="feature-tag">Mac Only</span>
            <span class="feature-tag">Plugins</span>
            <span class="feature-tag">Symbols</span>
          </div>
          <div class="tool-pros-cons">
            <div class="pros">
              <strong>Pros:</strong>
              <ul>
                <li>Extensive plugin ecosystem</li>
                <li>Symbol-based design system</li>
                <li>One-time purchase</li>
              </ul>
            </div>
            <div class="cons">
              <strong>Cons:</strong>
              <ul>
                <li>Mac only</li>
                <li>Limited prototyping</li>
              </ul>
            </div>
          </div>
        </div>
      </div>
    </div>
    
    <div class="tool-category">
      <h4>⚡ Development Tools</h4>
      
      <div class="tools-grid">
        <div class="tool-item">
          <div class="tool-header">
            <span class="tool-name">React + Storybook</span>
            <span class="tool-type">Component Library</span>
          </div>
          <div class="tool-features">
            <span class="feature-tag">Components</span>
            <span class="feature-tag">Interactive</span>
            <span class="feature-tag">Documentation</span>
          </div>
          <div class="tool-pros-cons">
            <div class="pros">
              <strong>Pros:</strong>
              <ul>
                <li>Real code components</li>
                <li>Interactive documentation</li>
                <li>Development handoff ready</li>
              </ul>
            </div>
            <div class="cons">
              <strong>Cons:</strong>
              <ul>
                <li>Requires development skills</li>
                <li>Longer setup time</li>
              </ul>
            </div>
          </div>
        </div>
        
        <div class="tool-item">
          <div class="tool-header">
            <span class="tool-name">HTML/CSS/JS</span>
            <span class="tool-type">Custom Prototype</span>
          </div>
          <div class="tool-features">
            <span class="feature-tag">Custom</span>
            <span class="feature-tag">Flexible</span>
            <span class="feature-tag">Realistic</span>
          </div>
          <div class="tool-pros-cons">
            <div class="pros">
              <strong>Pros:</strong>
              <ul>
                <li>Full control over functionality</li>
                <li>Realistic user experience</li>
                <li>No tool limitations</li>
              </ul>
            </div>
            <div class="cons">
              <strong>Cons:</strong>
              <ul>
                <li>Requires coding skills</li>
                <li>Time-consuming</li>
              </ul>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Prototyping Tools CSS:**
```css
/* Prototyping Tools */
.prototype-tools {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.tools-matrix {
  display: grid;
  gap: var(--space-xl);
}

.tool-category h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

.tools-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: var(--space-lg);
}

.tool-item {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
  transition: all 0.3s ease;
}

.tool-item:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.tool-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-md);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.tool-name {
  font-weight: var(--font-bold);
  color: var(--industrial-gray-700);
  font-size: 1.125rem;
}

.tool-type {
  background: var(--primary-orange);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-semibold);
}

.tool-features {
  display: flex;
  flex-wrap: wrap;
  gap: var(--space-xs);
  margin-bottom: var(--space-md);
}

.feature-tag {
  background: var(--bg-primary);
  color: var(--primary-orange);
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-medium);
  border: 1px solid var(--industrial-gray-300);
}

.tool-pros-cons {
  display: grid;
  gap: var(--space-md);
}

.pros, .cons {
  background: var(--bg-primary);
  padding: var(--space-md);
  border-radius: 0.5rem;
}

.pros {
  border-left: 3px solid var(--success-green);
}

.cons {
  border-left: 3px solid var(--error-red);
}

.pros strong, .cons strong {
  display: block;
  margin-bottom: var(--space-sm);
  font-weight: var(--font-semibold);
}

.pros strong {
  color: var(--success-green);
}

.cons strong {
  color: var(--error-red);
}

.pros ul, .cons ul {
  margin: 0;
  padding-left: var(--space-lg);
}

.pros li, .cons li {
  margin-bottom: var(--space-xs);
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
}
```

---

## 🎮 **3. INTERACTIVE PROTOTYPES - Mẫu tương tác**

### **Interactive Prototype Examples:**
```html
<!-- Interactive Prototype Examples -->
<div class="interactive-prototypes">
  <h3>🎮 Interactive Prototype Examples</h3>
  
  <div class="prototype-examples">
    <div class="prototype-example">
      <h4>🚨 Emergency Control Prototype</h4>
      
      <div class="prototype-demo">
        <div class="demo-header">
          <span class="demo-title">Emergency Stop Interface</span>
          <span class="demo-status active">Active</span>
        </div>
        
        <div class="demo-content">
          <div class="emergency-panel">
            <button class="emergency-btn" id="emergencyBtn">
              <div class="btn-icon">🚨</div>
              <div class="btn-text">EMERGENCY STOP</div>
            </button>
            
            <div class="status-indicators">
              <div class="status-item">
                <span class="status-label">Robot Status:</span>
                <span class="status-value" id="robotStatus">Moving</span>
              </div>
              <div class="status-item">
                <span class="status-label">Safety Status:</span>
                <span class="status-value" id="safetyStatus">Normal</span>
              </div>
            </div>
          </div>
          
          <div class="demo-instructions">
            <h5>Test Instructions:</h5>
            <ol>
              <li>Click the Emergency Stop button</li>
              <li>Observe status changes</li>
              <li>Test confirmation dialog</li>
              <li>Verify safety protocols</li>
            </ol>
          </div>
        </div>
      </div>
    </div>
    
    <div class="prototype-example">
      <h4>🎮 Control Panel Prototype</h4>
      
      <div class="prototype-demo">
        <div class="demo-header">
          <span class="demo-title">Manual Control Interface</span>
          <span class="demo-status">Interactive</span>
        </div>
        
        <div class="demo-content">
          <div class="control-panel">
            <div class="control-modes">
              <button class="mode-btn active" data-mode="auto">AUTO</button>
              <button class="mode-btn" data-mode="manual">MANUAL</button>
              <button class="mode-btn" data-mode="maintenance">MAINT</button>
            </div>
            
            <div class="movement-controls">
              <div class="control-row">
                <button class="control-btn" data-direction="forward">⬆️</button>
              </div>
              <div class="control-row">
                <button class="control-btn" data-direction="left">⬅️</button>
                <button class="control-btn stop" data-direction="stop">⏹️</button>
                <button class="control-btn" data-direction="right">➡️</button>
              </div>
              <div class="control-row">
                <button class="control-btn" data-direction="backward">⬇️</button>
              </div>
            </div>
            
            <div class="speed-control">
              <label for="speedSlider">Speed: <span id="speedValue">50%</span></label>
              <input type="range" id="speedSlider" min="0" max="100" value="50">
            </div>
          </div>
          
          <div class="demo-instructions">
            <h5>Test Instructions:</h5>
            <ol>
              <li>Switch between control modes</li>
              <li>Use movement controls</li>
              <li>Adjust speed slider</li>
              <li>Test emergency stop</li>
            </ol>
          </div>
        </div>
      </div>
    </div>
    
    <div class="prototype-example">
      <h4>📊 Dashboard Prototype</h4>
      
      <div class="prototype-demo">
        <div class="demo-header">
          <span class="demo-title">Main Dashboard Interface</span>
          <span class="demo-status">Real-time</span>
        </div>
        
        <div class="demo-content">
          <div class="dashboard-grid">
            <div class="dashboard-card status">
              <h5>Robot Status</h5>
              <div class="status-display">
                <span class="status-indicator active"></span>
                <span class="status-text">Operational</span>
              </div>
            </div>
            
            <div class="dashboard-card battery">
              <h5>Battery Level</h5>
              <div class="battery-display">
                <div class="battery-bar">
                  <div class="battery-fill" style="width: 75%"></div>
                </div>
                <span class="battery-text">75%</span>
              </div>
            </div>
            
            <div class="dashboard-card position">
              <h5>Current Position</h5>
              <div class="position-display">
                <span class="coordinates">X: 125.5, Y: 89.2</span>
              </div>
            </div>
            
            <div class="dashboard-card alerts">
              <h5>Active Alerts</h5>
              <div class="alerts-list">
                <div class="alert-item warning">
                  <span class="alert-icon">⚠️</span>
                  <span class="alert-text">Low battery warning</span>
                </div>
              </div>
            </div>
          </div>
          
          <div class="demo-instructions">
            <h5>Test Instructions:</h5>
            <ol>
              <li>Monitor real-time status</li>
              <li>Check battery levels</li>
              <li>View position data</li>
              <li>Review active alerts</li>
            </ol>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Interactive Prototypes CSS:**
```css
/* Interactive Prototypes */
.interactive-prototypes {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.prototype-examples {
  display: grid;
  gap: var(--space-xl);
}

.prototype-example h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

.prototype-demo {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  overflow: hidden;
  border: 1px solid var(--industrial-gray-300);
}

.demo-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md) var(--space-lg);
  background: var(--bg-primary);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.demo-title {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

.demo-status {
  background: var(--primary-orange);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-semibold);
}

.demo-status.active {
  background: var(--success-green);
}

.demo-content {
  padding: var(--space-lg);
}

/* Emergency Control Demo */
.emergency-panel {
  display: flex;
  flex-direction: column;
  gap: var(--space-lg);
  margin-bottom: var(--space-lg);
}

.emergency-btn {
  background: var(--error-red);
  color: white;
  border: none;
  border-radius: 0.75rem;
  padding: var(--space-xl);
  font-size: 1.5rem;
  font-weight: var(--font-bold);
  cursor: pointer;
  transition: all 0.3s ease;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: var(--space-md);
}

.emergency-btn:hover {
  background: #c82333;
  transform: scale(1.05);
}

.btn-icon {
  font-size: 3rem;
}

.status-indicators {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: var(--space-md);
}

.status-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-primary);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
}

.status-label {
  font-weight: var(--font-medium);
  color: var(--industrial-gray-600);
}

.status-value {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

/* Control Panel Demo */
.control-panel {
  display: flex;
  flex-direction: column;
  gap: var(--space-lg);
  margin-bottom: var(--space-lg);
}

.control-modes {
  display: flex;
  gap: var(--space-sm);
}

.mode-btn {
  flex: 1;
  padding: var(--space-md);
  background: var(--bg-primary);
  border: 2px solid var(--industrial-gray-300);
  border-radius: 0.5rem;
  cursor: pointer;
  font-weight: var(--font-semibold);
  transition: all 0.2s ease;
}

.mode-btn.active {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}

.movement-controls {
  display: flex;
  flex-direction: column;
  gap: var(--space-sm);
  align-items: center;
}

.control-row {
  display: flex;
  gap: var(--space-sm);
}

.control-btn {
  width: 60px;
  height: 60px;
  background: var(--bg-primary);
  border: 2px solid var(--industrial-gray-300);
  border-radius: 0.5rem;
  cursor: pointer;
  font-size: 1.5rem;
  transition: all 0.2s ease;
}

.control-btn:hover {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}

.control-btn.stop {
  background: var(--error-red);
  color: white;
  border-color: var(--error-red);
}

.speed-control {
  display: flex;
  flex-direction: column;
  gap: var(--space-sm);
}

.speed-control label {
  font-weight: var(--font-medium);
  color: var(--industrial-gray-700);
}

.speed-control input[type="range"] {
  width: 100%;
  height: 8px;
  border-radius: 4px;
  background: var(--industrial-gray-300);
  outline: none;
}

/* Dashboard Demo */
.dashboard-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: var(--space-md);
  margin-bottom: var(--space-lg);
}

.dashboard-card {
  background: var(--bg-primary);
  border-radius: 0.5rem;
  padding: var(--space-md);
  border-left: 3px solid var(--primary-orange);
}

.dashboard-card h5 {
  margin-bottom: var(--space-sm);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.status-display {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
}

.status-indicator {
  width: 12px;
  height: 12px;
  border-radius: 50%;
  background: var(--success-green);
  animation: pulse 2s infinite;
}

.battery-display {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
}

.battery-bar {
  flex: 1;
  height: 8px;
  background: var(--industrial-gray-300);
  border-radius: 4px;
  overflow: hidden;
}

.battery-fill {
  height: 100%;
  background: var(--success-green);
  border-radius: 4px;
}

.battery-text {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

.alerts-list {
  display: flex;
  flex-direction: column;
  gap: var(--space-xs);
}

.alert-item {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
  padding: var(--space-xs);
  border-radius: 0.25rem;
  background: rgba(255, 193, 7, 0.1);
}

.alert-icon {
  font-size: 1rem;
}

.alert-text {
  font-size: 0.875rem;
  color: var(--industrial-gray-700);
}

.demo-instructions {
  background: var(--bg-primary);
  border-radius: 0.5rem;
  padding: var(--space-md);
  border-left: 3px solid var(--info-blue);
}

.demo-instructions h5 {
  margin-bottom: var(--space-sm);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.demo-instructions ol {
  margin: 0;
  padding-left: var(--space-lg);
}

.demo-instructions li {
  margin-bottom: var(--space-xs);
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
}
```

---

## 📚 **Tham chiếu:**

- [Usability Testing](./REQ_UI_UX_Usability_Testing.md) - Kiểm thử khả năng sử dụng
- [Visual Design](./REQ_UI_UX_Visual_Design.md) - Thiết kế hình ảnh
- [Control Panel](./REQ_UI_UX_Control_Panel.md) - Bảng điều khiển tay
- [Dashboard Design](./REQ_UI_UX_Dashboard_Design.md) - Thiết kế dashboard

---

**Changelog v1.0:**
- ✅ Created comprehensive prototyping framework
- ✅ Defined prototype fidelity levels
- ✅ Implemented prototyping tools selection
- ✅ Added interactive prototype examples
- ✅ Created emergency control prototype
- ✅ Added control panel prototype
- ✅ Implemented dashboard prototype
- ✅ Included prototyping best practices
- ✅ Added tool comparison matrix
- ✅ Created prototype testing guidelines
- ✅ Added safety-focused prototyping
- ✅ Included development handoff considerations

**🚨 Lưu ý:** Tài liệu này tập trung vào rapid prototyping với safety-first approach cho robot OHT-50, đảm bảo validation nhanh chóng và hiệu quả.
