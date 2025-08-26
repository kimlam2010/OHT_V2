# 🧪 REQ_UI_UX_Usability_Testing - Kiểm thử khả năng sử dụng

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, QA Engineers, Product Managers, Test Coordinators

---

## 🎯 **Mục tiêu Usability Testing:**

### **Concept chính:**
- **System:** Comprehensive usability testing framework cho robot OHT-50
- **Focus:** Safety-first testing với real-world scenarios
- **Style:** Industrial theme với systematic evaluation approach
- **Target:** Đảm bảo interface an toàn và hiệu quả cho môi trường công nghiệp

### **Nguyên tắc thiết kế:**
1. **Safety Priority:** Ưu tiên kiểm thử các tính năng an toàn
2. **Real-world Scenarios:** Mô phỏng tình huống thực tế
3. **Systematic Approach:** Quy trình kiểm thử có cấu trúc
4. **Continuous Improvement:** Cải thiện liên tục dựa trên feedback

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| Test Planning | Lập kế hoạch kiểm thử | QA Engineers, PMs | Test Strategy | ✅ Hoàn thành |
| Test Scenarios | Kịch bản kiểm thử | Test Coordinators | User Stories | ✅ Hoàn thành |
| Test Methods | Phương pháp kiểm thử | UX Researchers | Research Methods | ✅ Hoàn thành |
| Test Metrics | Chỉ số đánh giá | Data Analysts | Performance Metrics | ✅ Hoàn thành |

---

## 📋 **1. TEST PLANNING - Lập kế hoạch kiểm thử**

### **Testing Strategy Framework:**
```html
<!-- Testing Strategy Overview -->
<div class="testing-strategy">
  <div class="strategy-overview">
    <h3>📋 Testing Strategy Overview</h3>
    
    <div class="strategy-grid">
      <div class="strategy-item safety">
        <div class="strategy-icon">🚨</div>
        <h4>Safety Testing</h4>
        <p>Critical safety features and emergency controls</p>
        <div class="priority">Priority: Critical</div>
      </div>
      
      <div class="strategy-item usability">
        <div class="strategy-icon">🎯</div>
        <h4>Usability Testing</h4>
        <p>User interface efficiency and learnability</p>
        <div class="priority">Priority: High</div>
      </div>
      
      <div class="strategy-item accessibility">
        <div class="strategy-icon">♿</div>
        <h4>Accessibility Testing</h4>
        <p>WCAG compliance and assistive technology</p>
        <div class="priority">Priority: High</div>
      </div>
      
      <div class="strategy-item performance">
        <div class="strategy-icon">⚡</div>
        <h4>Performance Testing</h4>
        <p>Response time and system reliability</p>
        <div class="priority">Priority: Medium</div>
      </div>
    </div>
  </div>
</div>
```

### **Test Planning CSS:**
```css
/* Testing Strategy */
.testing-strategy {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.strategy-overview h3 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-bold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

.strategy-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: var(--space-lg);
}

.strategy-item {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
  transition: all 0.3s ease;
}

.strategy-item:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.strategy-item.safety {
  border-left-color: var(--error-red);
}

.strategy-item.usability {
  border-left-color: var(--primary-orange);
}

.strategy-item.accessibility {
  border-left-color: var(--info-blue);
}

.strategy-item.performance {
  border-left-color: var(--success-green);
}

.strategy-icon {
  font-size: 2rem;
  margin-bottom: var(--space-md);
}

.strategy-item h4 {
  margin-bottom: var(--space-sm);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.strategy-item p {
  margin-bottom: var(--space-md);
  color: var(--industrial-gray-600);
  font-size: 0.875rem;
  line-height: 1.4;
}

.priority {
  background: var(--primary-orange);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-bold);
  text-align: center;
  text-transform: uppercase;
}

.strategy-item.safety .priority {
  background: var(--error-red);
}

.strategy-item.usability .priority {
  background: var(--primary-orange);
}

.strategy-item.accessibility .priority {
  background: var(--info-blue);
}

.strategy-item.performance .priority {
  background: var(--success-green);
}
```

### **Test Timeline Planning:**
```css
/* Test Timeline */
.test-timeline {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-top: var(--space-lg);
}

.timeline-phases {
  display: grid;
  gap: var(--space-lg);
}

.phase-item {
  display: flex;
  gap: var(--space-lg);
  padding: var(--space-lg);
  background: var(--bg-primary);
  border-radius: 0.5rem;
  border-left: 4px solid var(--primary-orange);
}

.phase-number {
  background: var(--primary-orange);
  color: white;
  width: 40px;
  height: 40px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  font-weight: var(--font-bold);
  flex-shrink: 0;
}

.phase-content {
  flex: 1;
}

.phase-title {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
  margin-bottom: var(--space-sm);
}

.phase-duration {
  font-size: 0.875rem;
  color: var(--primary-orange);
  font-weight: var(--font-medium);
  margin-bottom: var(--space-sm);
}

.phase-description {
  color: var(--industrial-gray-600);
  font-size: 0.875rem;
  line-height: 1.4;
}
```

---

## 🎭 **2. TEST SCENARIOS - Kịch bản kiểm thử**

### **Safety-Critical Scenarios:**
```html
<!-- Safety Test Scenarios -->
<div class="test-scenarios">
  <h3>🎭 Test Scenarios</h3>
  
  <div class="scenario-categories">
    <div class="scenario-category safety">
      <h4>🚨 Safety-Critical Scenarios</h4>
      
      <div class="scenario-list">
        <div class="scenario-item critical">
          <div class="scenario-header">
            <span class="scenario-id">S-001</span>
            <span class="scenario-title">Emergency Stop Activation</span>
            <span class="scenario-priority">Critical</span>
          </div>
          <div class="scenario-content">
            <p><strong>Objective:</strong> Verify emergency stop functionality</p>
            <p><strong>Steps:</strong></p>
            <ol>
              <li>Robot is in motion (auto/manual mode)</li>
              <li>User presses E-Stop button</li>
              <li>Robot must stop immediately (&lt; 100ms)</li>
              <li>System shows emergency stop status</li>
              <li>All movement controls disabled</li>
            </ol>
            <p><strong>Success Criteria:</strong> Robot stops within 100ms, status clear, controls locked</p>
          </div>
        </div>
        
        <div class="scenario-item critical">
          <div class="scenario-header">
            <span class="scenario-id">S-002</span>
            <span class="scenario-title">Obstacle Detection Response</span>
            <span class="scenario-priority">Critical</span>
          </div>
          <div class="scenario-content">
            <p><strong>Objective:</strong> Test obstacle detection and response</p>
            <p><strong>Steps:</strong></p>
            <ol>
              <li>Robot navigating autonomously</li>
              <li>Obstacle placed in path</li>
              <li>System detects obstacle</li>
              <li>Robot stops and alerts user</li>
              <li>User acknowledges and resolves</li>
            </ol>
            <p><strong>Success Criteria:</strong> Detection within 500ms, clear alert, safe stop</p>
          </div>
        </div>
        
        <div class="scenario-item warning">
          <div class="scenario-header">
            <span class="scenario-id">S-003</span>
            <span class="scenario-title">Low Battery Handling</span>
            <span class="scenario-priority">High</span>
          </div>
          <div class="scenario-content">
            <p><strong>Objective:</strong> Verify low battery warning system</p>
            <p><strong>Steps:</strong></p>
            <ol>
              <li>Battery level drops to 20%</li>
              <li>System shows warning alert</li>
              <li>User acknowledges warning</li>
              <li>Robot continues operation</li>
              <li>Battery reaches 10% - critical warning</li>
            </ol>
            <p><strong>Success Criteria:</strong> Clear warnings, user acknowledgment, graceful degradation</p>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Usability Scenarios:**
```html
<!-- Usability Test Scenarios -->
<div class="usability-scenarios">
  <div class="scenario-category usability">
    <h4>🎯 Usability Scenarios</h4>
    
    <div class="scenario-list">
      <div class="scenario-item normal">
        <div class="scenario-header">
          <span class="scenario-id">U-001</span>
          <span class="scenario-title">First-Time User Onboarding</span>
          <span class="scenario-priority">High</span>
        </div>
        <div class="scenario-content">
          <p><strong>Objective:</strong> Test interface learnability for new users</p>
          <p><strong>Steps:</strong></p>
          <ol>
            <li>New user opens interface</li>
            <li>Complete onboarding tutorial</li>
            <li>Navigate to main dashboard</li>
            <li>Perform basic operations</li>
            <li>Access help documentation</li>
          </ol>
          <p><strong>Success Criteria:</strong> Complete tasks within 5 minutes, no errors</p>
        </div>
      </div>
      
      <div class="scenario-item normal">
        <div class="scenario-header">
          <span class="scenario-id">U-002</span>
          <span class="scenario-title">Control Panel Operation</span>
          <span class="scenario-priority">High</span>
        </div>
        <div class="scenario-content">
          <p><strong>Objective:</strong> Test manual control functionality</p>
          <p><strong>Steps:</strong></p>
          <ol>
            <li>Switch to manual mode</li>
            <li>Use directional controls</li>
            <li>Adjust speed settings</li>
            <li>Test emergency controls</li>
            <li>Return to auto mode</li>
          </ol>
          <p><strong>Success Criteria:</strong> Intuitive controls, immediate response, clear feedback</p>
        </div>
      </div>
      
      <div class="scenario-item normal">
        <div class="scenario-header">
          <span class="scenario-id">U-003</span>
          <span class="scenario-title">Alert System Interaction</span>
          <span class="scenario-priority">Medium</span>
        </div>
        <div class="scenario-content">
          <p><strong>Objective:</strong> Test alert handling and management</p>
          <p><strong>Steps:</strong></p>
          <ol>
            <li>Receive various alert types</li>
            <li>Acknowledge alerts</li>
            <li>Access alert details</li>
            <li>Filter alert history</li>
            <li>Configure alert preferences</li>
          </ol>
          <p><strong>Success Criteria:</strong> Clear alert hierarchy, easy management, no missed alerts</p>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Test Scenarios CSS:**
```css
/* Test Scenarios */
.test-scenarios {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.scenario-categories {
  display: grid;
  gap: var(--space-xl);
}

.scenario-category h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

.scenario-list {
  display: grid;
  gap: var(--space-lg);
}

.scenario-item {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
  transition: all 0.3s ease;
}

.scenario-item:hover {
  transform: translateX(4px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.scenario-item.critical {
  border-left-color: var(--error-red);
  background: rgba(220, 53, 69, 0.05);
}

.scenario-item.warning {
  border-left-color: var(--warning-yellow);
  background: rgba(255, 193, 7, 0.05);
}

.scenario-item.normal {
  border-left-color: var(--info-blue);
  background: rgba(23, 162, 184, 0.05);
}

.scenario-header {
  display: flex;
  align-items: center;
  gap: var(--space-md);
  margin-bottom: var(--space-md);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.scenario-id {
  background: var(--primary-orange);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-bold);
  font-family: var(--font-mono);
}

.scenario-title {
  flex: 1;
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

.scenario-priority {
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-bold);
  text-transform: uppercase;
}

.scenario-item.critical .scenario-priority {
  background: var(--error-red);
  color: white;
}

.scenario-item.warning .scenario-priority {
  background: var(--warning-yellow);
  color: var(--industrial-gray-800);
}

.scenario-item.normal .scenario-priority {
  background: var(--info-blue);
  color: white;
}

.scenario-content p {
  margin-bottom: var(--space-sm);
  color: var(--industrial-gray-600);
  font-size: 0.875rem;
  line-height: 1.4;
}

.scenario-content strong {
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.scenario-content ol {
  margin: var(--space-sm) 0;
  padding-left: var(--space-lg);
}

.scenario-content li {
  margin-bottom: var(--space-xs);
  color: var(--industrial-gray-600);
  font-size: 0.875rem;
}
```

---

## 🔬 **3. TEST METHODS - Phương pháp kiểm thử**

### **Testing Methodologies:**
```html
<!-- Testing Methods -->
<div class="testing-methods">
  <h3>🔬 Testing Methods</h3>
  
  <div class="method-categories">
    <div class="method-category">
      <h4>👥 User Testing Methods</h4>
      
      <div class="method-grid">
        <div class="method-item">
          <div class="method-icon">🎯</div>
          <h5>Think-Aloud Protocol</h5>
          <p>Users verbalize thoughts while using interface</p>
          <div class="method-details">
            <span class="detail-label">Duration:</span>
            <span class="detail-value">30-60 minutes</span>
          </div>
          <div class="method-details">
            <span class="detail-label">Participants:</span>
            <span class="detail-value">5-8 users</span>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-icon">📊</div>
          <h5>Task-Based Testing</h5>
          <p>Users complete specific tasks while being observed</p>
          <div class="method-details">
            <span class="detail-label">Duration:</span>
            <span class="detail-value">45-90 minutes</span>
          </div>
          <div class="method-details">
            <span class="detail-label">Participants:</span>
            <span class="detail-value">8-12 users</span>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-icon">🔍</div>
          <h5>Heuristic Evaluation</h5>
          <p>Expert review using established usability principles</p>
          <div class="method-details">
            <span class="detail-label">Duration:</span>
            <span class="detail-value">2-4 hours</span>
          </div>
          <div class="method-details">
            <span class="detail-label">Evaluators:</span>
            <span class="detail-value">3-5 experts</span>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-icon">📱</div>
          <h5>Remote Testing</h5>
          <p>Testing conducted remotely with screen sharing</p>
          <div class="method-details">
            <span class="detail-label">Duration:</span>
            <span class="detail-value">30-45 minutes</span>
          </div>
          <div class="method-details">
            <span class="detail-label">Participants:</span>
            <span class="detail-value">10-15 users</span>
          </div>
        </div>
      </div>
    </div>
    
    <div class="method-category">
      <h4>🔧 Technical Testing Methods</h4>
      
      <div class="method-grid">
        <div class="method-item">
          <div class="method-icon">⚡</div>
          <h5>Performance Testing</h5>
          <p>Measure response times and system performance</p>
          <div class="method-details">
            <span class="detail-label">Tools:</span>
            <span class="detail-value">Lighthouse, WebPageTest</span>
          </div>
          <div class="method-details">
            <span class="detail-label">Metrics:</span>
            <span class="detail-value">Load time, FCP, LCP</span>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-icon">♿</div>
          <h5>Accessibility Testing</h5>
          <p>Automated and manual accessibility evaluation</p>
          <div class="method-details">
            <span class="detail-label">Tools:</span>
            <span class="detail-value">axe-core, WAVE</span>
          </div>
          <div class="method-details">
            <span class="detail-label">Standards:</span>
            <span class="detail-value">WCAG 2.1 AA</span>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-icon">📱</div>
          <h5>Cross-Device Testing</h5>
          <p>Test across different devices and screen sizes</p>
          <div class="method-details">
            <span class="detail-label">Devices:</span>
            <span class="detail-value">Desktop, Tablet, Mobile</span>
          </div>
          <div class="method-details">
            <span class="detail-label">Browsers:</span>
            <span class="detail-value">Chrome, Firefox, Safari</span>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-icon">🔒</div>
          <h5>Security Testing</h5>
          <p>Evaluate security vulnerabilities and data protection</p>
          <div class="method-details">
            <span class="detail-label">Tools:</span>
            <span class="detail-value">OWASP ZAP, Burp Suite</span>
          </div>
          <div class="method-details">
            <span class="detail-label">Focus:</span>
            <span class="detail-value">Authentication, Data integrity</span>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Testing Methods CSS:**
```css
/* Testing Methods */
.testing-methods {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.method-categories {
  display: grid;
  gap: var(--space-xl);
}

.method-category h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

.method-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: var(--space-lg);
}

.method-item {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
  transition: all 0.3s ease;
}

.method-item:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.method-icon {
  font-size: 2rem;
  margin-bottom: var(--space-md);
}

.method-item h5 {
  margin-bottom: var(--space-sm);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.method-item p {
  margin-bottom: var(--space-md);
  color: var(--industrial-gray-600);
  font-size: 0.875rem;
  line-height: 1.4;
}

.method-details {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-xs);
}

.detail-label {
  font-size: 0.75rem;
  color: var(--industrial-gray-500);
  font-weight: var(--font-medium);
}

.detail-value {
  font-size: 0.75rem;
  color: var(--primary-orange);
  font-weight: var(--font-semibold);
}
```

---

## 📈 **4. TEST METRICS - Chỉ số đánh giá**

### **Usability Metrics:**
```html
<!-- Test Metrics -->
<div class="test-metrics">
  <h3>📈 Test Metrics & KPIs</h3>
  
  <div class="metrics-categories">
    <div class="metrics-category">
      <h4>⏱️ Performance Metrics</h4>
      
      <div class="metrics-grid">
        <div class="metric-item">
          <div class="metric-header">
            <span class="metric-name">Task Completion Rate</span>
            <span class="metric-target">≥ 95%</span>
          </div>
          <div class="metric-bar">
            <div class="metric-fill" style="width: 92%"></div>
          </div>
          <div class="metric-details">
            <span>Current: 92%</span>
            <span>Target: 95%</span>
          </div>
        </div>
        
        <div class="metric-item">
          <div class="metric-header">
            <span class="metric-name">Task Completion Time</span>
            <span class="metric-target">≤ 30s</span>
          </div>
          <div class="metric-bar">
            <div class="metric-fill" style="width: 85%"></div>
          </div>
          <div class="metric-details">
            <span>Current: 25.5s</span>
            <span>Target: 30s</span>
          </div>
        </div>
        
        <div class="metric-item">
          <div class="metric-header">
            <span class="metric-name">Error Rate</span>
            <span class="metric-target">≤ 5%</span>
          </div>
          <div class="metric-bar">
            <div class="metric-fill error" style="width: 8%"></div>
          </div>
          <div class="metric-details">
            <span>Current: 8%</span>
            <span>Target: 5%</span>
          </div>
        </div>
        
        <div class="metric-item">
          <div class="metric-header">
            <span class="metric-name">System Usability Scale</span>
            <span class="metric-target">≥ 70</span>
          </div>
          <div class="metric-bar">
            <div class="metric-fill" style="width: 78%"></div>
          </div>
          <div class="metric-details">
            <span>Current: 78</span>
            <span>Target: 70</span>
          </div>
        </div>
      </div>
    </div>
    
    <div class="metrics-category">
      <h4>🚨 Safety Metrics</h4>
      
      <div class="metrics-grid">
        <div class="metric-item critical">
          <div class="metric-header">
            <span class="metric-name">Emergency Response Time</span>
            <span class="metric-target">≤ 100ms</span>
          </div>
          <div class="metric-bar">
            <div class="metric-fill" style="width: 95%"></div>
          </div>
          <div class="metric-details">
            <span>Current: 95ms</span>
            <span>Target: 100ms</span>
          </div>
        </div>
        
        <div class="metric-item critical">
          <div class="metric-header">
            <span class="metric-name">Safety Violation Rate</span>
            <span class="metric-target">0%</span>
          </div>
          <div class="metric-bar">
            <div class="metric-fill perfect" style="width: 100%"></div>
          </div>
          <div class="metric-details">
            <span>Current: 0%</span>
            <span>Target: 0%</span>
          </div>
        </div>
        
        <div class="metric-item critical">
          <div class="metric-header">
            <span class="metric-name">Alert Recognition Rate</span>
            <span class="metric-target">≥ 98%</span>
          </div>
          <div class="metric-bar">
            <div class="metric-fill" style="width: 96%"></div>
          </div>
          <div class="metric-details">
            <span>Current: 96%</span>
            <span>Target: 98%</span>
          </div>
        </div>
      </div>
    </div>
    
    <div class="metrics-category">
      <h4>♿ Accessibility Metrics</h4>
      
      <div class="metrics-grid">
        <div class="metric-item">
          <div class="metric-header">
            <span class="metric-name">WCAG 2.1 AA Compliance</span>
            <span class="metric-target">100%</span>
          </div>
          <div class="metric-bar">
            <div class="metric-fill" style="width: 98%"></div>
          </div>
          <div class="metric-details">
            <span>Current: 98%</span>
            <span>Target: 100%</span>
          </div>
        </div>
        
        <div class="metric-item">
          <div class="metric-header">
            <span class="metric-name">Keyboard Navigation Success</span>
            <span class="metric-target">≥ 95%</span>
          </div>
          <div class="metric-bar">
            <div class="metric-fill" style="width: 97%"></div>
          </div>
          <div class="metric-details">
            <span>Current: 97%</span>
            <span>Target: 95%</span>
          </div>
        </div>
        
        <div class="metric-item">
          <div class="metric-header">
            <span class="metric-name">Screen Reader Compatibility</span>
            <span class="metric-target">≥ 90%</span>
          </div>
          <div class="metric-bar">
            <div class="metric-fill" style="width: 88%"></div>
          </div>
          <div class="metric-details">
            <span>Current: 88%</span>
            <span>Target: 90%</span>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Test Metrics CSS:**
```css
/* Test Metrics */
.test-metrics {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.metrics-categories {
  display: grid;
  gap: var(--space-xl);
}

.metrics-category h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

.metrics-grid {
  display: grid;
  gap: var(--space-lg);
}

.metric-item {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
}

.metric-item.critical {
  border-left-color: var(--error-red);
  background: rgba(220, 53, 69, 0.05);
}

.metric-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-md);
}

.metric-name {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

.metric-target {
  background: var(--primary-orange);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-bold);
}

.metric-item.critical .metric-target {
  background: var(--error-red);
}

.metric-bar {
  height: 8px;
  background: var(--industrial-gray-300);
  border-radius: 4px;
  overflow: hidden;
  margin-bottom: var(--space-sm);
}

.metric-fill {
  height: 100%;
  background: var(--primary-orange);
  border-radius: 4px;
  transition: width 0.3s ease;
}

.metric-fill.error {
  background: var(--error-red);
}

.metric-fill.perfect {
  background: var(--success-green);
}

.metric-details {
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 0.875rem;
}

.metric-details span:first-child {
  color: var(--industrial-gray-600);
}

.metric-details span:last-child {
  color: var(--primary-orange);
  font-weight: var(--font-semibold);
}

.metric-item.critical .metric-details span:last-child {
  color: var(--error-red);
}
```

---

## 📚 **Tham chiếu:**

- [Accessibility](./REQ_UI_UX_Accessibility.md) - Khả năng tiếp cận
- [Visual Design](./REQ_UI_UX_Visual_Design.md) - Thiết kế hình ảnh
- [Control Panel](./REQ_UI_UX_Control_Panel.md) - Bảng điều khiển tay
- [Alert System](./REQ_UI_UX_Alert_System.md) - Hệ thống cảnh báo

---

**Changelog v1.0:**
- ✅ Created comprehensive usability testing framework
- ✅ Defined safety-critical test scenarios
- ✅ Implemented usability testing methods
- ✅ Added performance and accessibility metrics
- ✅ Created test planning and timeline structure
- ✅ Included user testing methodologies
- ✅ Added technical testing approaches
- ✅ Defined success criteria and KPIs
- ✅ Created metric visualization system
- ✅ Added safety-focused testing priorities
- ✅ Included cross-device testing methods
- ✅ Added security testing considerations

**🚨 Lưu ý:** Tài liệu này tập trung vào safety-first testing cho robot OHT-50, đảm bảo interface an toàn và hiệu quả trong môi trường công nghiệp.
