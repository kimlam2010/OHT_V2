# 👥 REQ_UI_UX_User_Research - Nghiên cứu người dùng

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UX Researchers, Product Managers, UI/UX Designers, Stakeholders

---

## 🎯 **Mục tiêu User Research:**

### **Concept chính:**
- **System:** Comprehensive user research framework cho robot OHT-50
- **Focus:** Understanding user needs và safety requirements
- **Style:** Industrial context với technical user base
- **Target:** Kỹ sư kỹ thuật cao và quản lý trong môi trường outdoor

### **Nguyên tắc nghiên cứu:**
1. **Safety-First:** Ưu tiên hiểu nhu cầu an toàn
2. **Context-Aware:** Nghiên cứu trong môi trường thực tế
3. **Technical Depth:** Hiểu sâu về technical requirements
4. **Iterative:** Continuous research và validation

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| User Personas | Định nghĩa người dùng | Designers, PMs | Design Strategy | ✅ Hoàn thành |
| User Journey Mapping | Hành trình người dùng | UX Researchers | Interaction Design | ✅ Hoàn thành |
| Research Methods | Phương pháp nghiên cứu | Researchers, PMs | Research Strategy | ✅ Hoàn thành |
| User Needs Analysis | Phân tích nhu cầu | Designers, Developers | Requirements | ✅ Hoàn thành |

---

## 👤 **1. USER PERSONAS - Định nghĩa người dùng**

### **Primary Personas:**
```html
<!-- User Personas -->
<div class="user-personas">
  <h3>👤 User Personas</h3>
  
  <div class="personas-grid">
    <div class="persona-card primary">
      <div class="persona-header">
        <div class="persona-avatar">
          <span class="avatar-icon">👨‍💼</span>
        </div>
        <div class="persona-info">
          <h4 class="persona-name">Nguyễn Văn Kỹ Sư</h4>
          <span class="persona-title">Senior Robotics Engineer</span>
          <span class="persona-company">OHT-50 Operations Team</span>
        </div>
      </div>
      
      <div class="persona-details">
        <div class="detail-section">
          <h5>📋 Demographics</h5>
          <ul>
            <li><strong>Age:</strong> 28-35 years old</li>
            <li><strong>Education:</strong> Bachelor's in Robotics/Mechanical Engineering</li>
            <li><strong>Experience:</strong> 5-8 years in industrial robotics</li>
            <li><strong>Location:</strong> Factory floor, outdoor environment</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>🎯 Goals & Motivations</h5>
          <ul>
            <li>Ensure robot safety và optimal performance</li>
            <li>Minimize downtime và maintenance costs</li>
            <li>Quick problem diagnosis và resolution</li>
            <li>Efficient task scheduling và monitoring</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>💡 Pain Points</h5>
          <ul>
            <li>Complex interfaces slow down emergency responses</li>
            <li>Lack of real-time system status visibility</li>
            <li>Difficult troubleshooting procedures</li>
            <li>Poor mobile interface for field work</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>🛠️ Technical Skills</h5>
          <ul>
            <li>Advanced robotics programming</li>
            <li>Industrial automation systems</li>
            <li>Safety protocols và compliance</li>
            <li>Data analysis và reporting</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>📱 Technology Usage</h5>
          <ul>
            <li>Daily use of industrial control systems</li>
            <li>Mobile devices for field monitoring</li>
            <li>Tablet interfaces for configuration</li>
            <li>Real-time data dashboards</li>
          </ul>
        </div>
      </div>
    </div>
    
    <div class="persona-card secondary">
      <div class="persona-header">
        <div class="persona-avatar">
          <span class="avatar-icon">👩‍💼</span>
        </div>
        <div class="persona-info">
          <h4 class="persona-name">Trần Thị Quản Lý</h4>
          <span class="persona-title">Operations Manager</span>
          <span class="persona-company">Production Management</span>
        </div>
      </div>
      
      <div class="persona-details">
        <div class="detail-section">
          <h5>📋 Demographics</h5>
          <ul>
            <li><strong>Age:</strong> 35-45 years old</li>
            <li><strong>Education:</strong> MBA/Industrial Management</li>
            <li><strong>Experience:</strong> 10+ years in production management</li>
            <li><strong>Location:</strong> Office, occasional factory visits</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>🎯 Goals & Motivations</h5>
          <ul>
            <li>Maximize production efficiency</li>
            <li>Ensure safety compliance</li>
            <li>Reduce operational costs</li>
            <li>Improve team productivity</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>💡 Pain Points</h5>
          <ul>
            <li>Limited visibility into robot performance</li>
            <li>Complex reporting systems</li>
            <li>Difficulty tracking safety incidents</li>
            <li>Poor integration with existing systems</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>🛠️ Technical Skills</h5>
          <ul>
            <li>Production planning và scheduling</li>
            <li>Performance metrics analysis</li>
            <li>Safety management systems</li>
            <li>Team coordination</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>📱 Technology Usage</h5>
          <ul>
            <li>Desktop applications for planning</li>
            <li>Mobile apps for quick monitoring</li>
            <li>Reporting dashboards</li>
            <li>Communication tools</li>
          </ul>
        </div>
      </div>
    </div>
    
    <div class="persona-card tertiary">
      <div class="persona-header">
        <div class="persona-avatar">
          <span class="avatar-icon">👨‍🔧</span>
        </div>
        <div class="persona-info">
          <h4 class="persona-name">Lê Văn Bảo Trì</h4>
          <span class="persona-title">Maintenance Technician</span>
          <span class="persona-company">Technical Support Team</span>
        </div>
      </div>
      
      <div class="persona-details">
        <div class="detail-section">
          <h5>📋 Demographics</h5>
          <ul>
            <li><strong>Age:</strong> 25-40 years old</li>
            <li><strong>Education:</strong> Technical diploma/Certification</li>
            <li><strong>Experience:</strong> 3-6 years in equipment maintenance</li>
            <li><strong>Location:</strong> Factory floor, maintenance areas</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>🎯 Goals & Motivations</h5>
          <ul>
            <li>Quick equipment diagnosis</li>
            <li>Efficient repair procedures</li>
            <li>Preventive maintenance scheduling</li>
            <li>Safety during maintenance work</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>💡 Pain Points</h5>
          <ul>
            <li>Complex diagnostic procedures</li>
            <li>Limited access to technical documentation</li>
            <li>Poor mobile interface for field work</li>
            <li>Difficulty tracking maintenance history</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>🛠️ Technical Skills</h5>
          <ul>
            <li>Equipment troubleshooting</li>
            <li>Preventive maintenance</li>
            <li>Safety procedures</li>
            <li>Basic programming</li>
          </ul>
        </div>
        
        <div class="detail-section">
          <h5>📱 Technology Usage</h5>
          <ul>
            <li>Mobile devices for diagnostics</li>
            <li>Technical documentation apps</li>
            <li>Maintenance tracking systems</li>
            <li>Communication tools</li>
          </ul>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **User Personas CSS:**
```css
/* User Personas */
.user-personas {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.personas-grid {
  display: grid;
  gap: var(--space-xl);
}

.persona-card {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
  transition: all 0.3s ease;
}

.persona-card:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.persona-card.primary {
  border-left-color: var(--primary-orange);
  background: rgba(255, 107, 53, 0.05);
}

.persona-card.secondary {
  border-left-color: var(--info-blue);
  background: rgba(23, 162, 184, 0.05);
}

.persona-card.tertiary {
  border-left-color: var(--success-green);
  background: rgba(40, 167, 69, 0.05);
}

.persona-header {
  display: flex;
  align-items: center;
  gap: var(--space-lg);
  margin-bottom: var(--space-xl);
  padding-bottom: var(--space-lg);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.persona-avatar {
  flex-shrink: 0;
}

.avatar-icon {
  font-size: 3rem;
  display: block;
}

.persona-info {
  flex: 1;
}

.persona-name {
  margin: 0 0 var(--space-sm) 0;
  color: var(--industrial-gray-700);
  font-weight: var(--font-bold);
  font-size: 1.25rem;
}

.persona-title {
  display: block;
  color: var(--primary-orange);
  font-weight: var(--font-semibold);
  margin-bottom: var(--space-xs);
}

.persona-company {
  display: block;
  color: var(--industrial-gray-600);
  font-size: 0.875rem;
}

.persona-details {
  display: grid;
  gap: var(--space-lg);
}

.detail-section h5 {
  margin-bottom: var(--space-md);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

.detail-section ul {
  margin: 0;
  padding-left: var(--space-lg);
}

.detail-section li {
  margin-bottom: var(--space-sm);
  color: var(--industrial-gray-600);
  line-height: 1.4;
}

.detail-section strong {
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}
```

---

## 🗺️ **2. USER JOURNEY MAPPING - Hành trình người dùng**

### **Critical User Journeys:**
```html
<!-- User Journey Mapping -->
<div class="user-journey-mapping">
  <h3>🗺️ User Journey Mapping</h3>
  
  <div class="journey-scenarios">
    <div class="journey-scenario emergency">
      <h4>🚨 Emergency Response Journey</h4>
      
      <div class="journey-timeline">
        <div class="timeline-step">
          <div class="step-header">
            <span class="step-number">1</span>
            <span class="step-title">Emergency Detection</span>
            <span class="step-time">0-5s</span>
          </div>
          <div class="step-content">
            <div class="step-actions">
              <span class="action">System detects emergency</span>
              <span class="action">Alert notification triggered</span>
              <span class="action">E-Stop activation</span>
            </div>
            <div class="step-emotions">
              <span class="emotion high-stress">High Stress</span>
              <span class="emotion urgency">Urgency</span>
            </div>
            <div class="step-touchpoints">
              <span class="touchpoint">Alert Banner</span>
              <span class="touchpoint">E-Stop Button</span>
              <span class="touchpoint">Audio Alarm</span>
            </div>
          </div>
        </div>
        
        <div class="timeline-step">
          <div class="step-header">
            <span class="step-number">2</span>
            <span class="step-title">Emergency Assessment</span>
            <span class="step-time">5-30s</span>
          </div>
          <div class="step-content">
            <div class="step-actions">
              <span class="action">Review emergency details</span>
              <span class="action">Assess robot status</span>
              <span class="action">Check safety conditions</span>
            </div>
            <div class="step-emotions">
              <span class="emotion focus">Focused</span>
              <span class="emotion concern">Concern</span>
            </div>
            <div class="step-touchpoints">
              <span class="touchpoint">Status Dashboard</span>
              <span class="touchpoint">Emergency Details</span>
              <span class="touchpoint">Safety Indicators</span>
            </div>
          </div>
        </div>
        
        <div class="timeline-step">
          <div class="step-header">
            <span class="step-number">3</span>
            <span class="step-title">Response Action</span>
            <span class="step-time">30s-2min</span>
          </div>
          <div class="step-content">
            <div class="step-actions">
              <span class="action">Execute safety procedures</span>
              <span class="action">Contact emergency team</span>
              <span class="action">Document incident</span>
            </div>
            <div class="step-emotions">
              <span class="emotion control">In Control</span>
              <span class="emotion relief">Relief</span>
            </div>
            <div class="step-touchpoints">
              <span class="touchpoint">Control Panel</span>
              <span class="touchpoint">Communication Tools</span>
              <span class="touchpoint">Incident Report</span>
            </div>
          </div>
        </div>
        
        <div class="timeline-step">
          <div class="step-header">
            <span class="step-number">4</span>
            <span class="step-title">Recovery & Analysis</span>
            <span class="step-time">2min-1hr</span>
          </div>
          <div class="step-content">
            <div class="step-actions">
              <span class="action">System recovery</span>
              <span class="action">Incident analysis</span>
              <span class="action">Preventive measures</span>
            </div>
            <div class="step-emotions">
              <span class="emotion learning">Learning</span>
              <span class="emotion improvement">Improvement</span>
            </div>
            <div class="step-touchpoints">
              <span class="touchpoint">Recovery Tools</span>
              <span class="touchpoint">Analysis Reports</span>
              <span class="touchpoint">Configuration</span>
            </div>
          </div>
        </div>
      </div>
    </div>
    
    <div class="journey-scenario daily">
      <h4>📅 Daily Operations Journey</h4>
      
      <div class="journey-timeline">
        <div class="timeline-step">
          <div class="step-header">
            <span class="step-number">1</span>
            <span class="step-title">Morning Check</span>
            <span class="step-time">5-10min</span>
          </div>
          <div class="step-content">
            <div class="step-actions">
              <span class="action">System status review</span>
              <span class="action">Battery level check</span>
              <span class="action">Safety inspection</span>
            </div>
            <div class="step-emotions">
              <span class="emotion routine">Routine</span>
              <span class="emotion confidence">Confidence</span>
            </div>
            <div class="step-touchpoints">
              <span class="touchpoint">Dashboard</span>
              <span class="touchpoint">Status Cards</span>
              <span class="touchpoint">Quick Checks</span>
            </div>
          </div>
        </div>
        
        <div class="timeline-step">
          <div class="step-header">
            <span class="step-number">2</span>
            <span class="step-title">Task Planning</span>
            <span class="step-time">10-15min</span>
          </div>
          <div class="step-content">
            <div class="step-actions">
              <span class="action">Review task queue</span>
              <span class="action">Schedule optimization</span>
              <span class="action">Resource allocation</span>
            </div>
            <div class="step-emotions">
              <span class="emotion planning">Planning</span>
              <span class="emotion efficiency">Efficiency</span>
            </div>
            <div class="step-touchpoints">
              <span class="touchpoint">Task Manager</span>
              <span class="touchpoint">Scheduling Tools</span>
              <span class="touchpoint">Resource Monitor</span>
            </div>
          </div>
        </div>
        
        <div class="timeline-step">
          <div class="step-header">
            <span class="step-number">3</span>
            <span class="step-title">Monitoring</span>
            <span class="step-time">Ongoing</span>
          </div>
          <div class="step-content">
            <div class="step-actions">
              <span class="action">Real-time monitoring</span>
              <span class="action">Performance tracking</span>
              <span class="action">Alert management</span>
            </div>
            <div class="step-emotions">
              <span class="emotion vigilance">Vigilance</span>
              <span class="emotion awareness">Awareness</span>
            </div>
            <div class="step-touchpoints">
              <span class="touchpoint">Real-time Dashboard</span>
              <span class="touchpoint">Alert System</span>
              <span class="touchpoint">Performance Metrics</span>
            </div>
          </div>
        </div>
        
        <div class="timeline-step">
          <div class="step-header">
            <span class="step-number">4</span>
            <span class="step-title">End of Day</span>
            <span class="step-time">15-20min</span>
          </div>
          <div class="step-content">
            <div class="step-actions">
              <span class="action">Performance review</span>
              <span class="action">Maintenance planning</span>
              <span class="action">Report generation</span>
            </div>
            <div class="step-emotions">
              <span class="emotion reflection">Reflection</span>
              <span class="emotion satisfaction">Satisfaction</span>
            </div>
            <div class="step-touchpoints">
              <span class="touchpoint">Performance Reports</span>
              <span class="touchpoint">Maintenance Scheduler</span>
              <span class="touchpoint">Daily Summary</span>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **User Journey CSS:**
```css
/* User Journey Mapping */
.user-journey-mapping {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.journey-scenarios {
  display: grid;
  gap: var(--space-xl);
}

.journey-scenario h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

.journey-timeline {
  display: grid;
  gap: var(--space-lg);
}

.timeline-step {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
  transition: all 0.3s ease;
}

.timeline-step:hover {
  transform: translateX(4px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.journey-scenario.emergency .timeline-step {
  border-left-color: var(--error-red);
  background: rgba(220, 53, 69, 0.05);
}

.journey-scenario.daily .timeline-step {
  border-left-color: var(--info-blue);
  background: rgba(23, 162, 184, 0.05);
}

.step-header {
  display: flex;
  align-items: center;
  gap: var(--space-md);
  margin-bottom: var(--space-lg);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.step-number {
  background: var(--primary-orange);
  color: white;
  width: 32px;
  height: 32px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  font-weight: var(--font-bold);
  flex-shrink: 0;
}

.step-title {
  flex: 1;
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
  font-size: 1.125rem;
}

.step-time {
  background: var(--bg-primary);
  color: var(--primary-orange);
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.875rem;
  font-weight: var(--font-medium);
}

.step-content {
  display: grid;
  gap: var(--space-md);
}

.step-actions {
  display: flex;
  flex-wrap: wrap;
  gap: var(--space-sm);
}

.action {
  background: var(--bg-primary);
  color: var(--industrial-gray-700);
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.875rem;
  font-weight: var(--font-medium);
  border: 1px solid var(--industrial-gray-300);
}

.step-emotions {
  display: flex;
  gap: var(--space-sm);
}

.emotion {
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-semibold);
  text-transform: uppercase;
}

.emotion.high-stress {
  background: var(--error-red);
  color: white;
}

.emotion.urgency {
  background: var(--warning-yellow);
  color: var(--industrial-gray-800);
}

.emotion.focus {
  background: var(--info-blue);
  color: white;
}

.emotion.concern {
  background: var(--warning-yellow);
  color: var(--industrial-gray-800);
}

.emotion.control {
  background: var(--success-green);
  color: white;
}

.emotion.relief {
  background: var(--info-blue);
  color: white;
}

.emotion.learning {
  background: var(--primary-orange);
  color: white;
}

.emotion.improvement {
  background: var(--success-green);
  color: white;
}

.emotion.routine {
  background: var(--industrial-gray-500);
  color: white;
}

.emotion.confidence {
  background: var(--success-green);
  color: white;
}

.emotion.planning {
  background: var(--info-blue);
  color: white;
}

.emotion.efficiency {
  background: var(--success-green);
  color: white;
}

.emotion.vigilance {
  background: var(--warning-yellow);
  color: var(--industrial-gray-800);
}

.emotion.awareness {
  background: var(--info-blue);
  color: white;
}

.emotion.reflection {
  background: var(--primary-orange);
  color: white;
}

.emotion.satisfaction {
  background: var(--success-green);
  color: white;
}

.step-touchpoints {
  display: flex;
  flex-wrap: wrap;
  gap: var(--space-sm);
}

.touchpoint {
  background: var(--primary-orange);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-semibold);
}
```

---

## 🔬 **3. RESEARCH METHODS - Phương pháp nghiên cứu**

### **Research Methodology:**
```html
<!-- Research Methods -->
<div class="research-methods">
  <h3>🔬 Research Methods</h3>
  
  <div class="methods-grid">
    <div class="method-category">
      <h4>📊 Quantitative Research</h4>
      
      <div class="methods-list">
        <div class="method-item">
          <div class="method-header">
            <span class="method-name">Usage Analytics</span>
            <span class="method-type">Data Analysis</span>
          </div>
          <div class="method-description">
            <p>Track user interactions, feature usage, and performance metrics to understand behavior patterns.</p>
            <div class="method-metrics">
              <span class="metric">Session duration</span>
              <span class="metric">Feature adoption</span>
              <span class="metric">Error rates</span>
              <span class="metric">Task completion</span>
            </div>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-header">
            <span class="method-name">Performance Testing</span>
            <span class="method-type">Benchmarking</span>
          </div>
          <div class="method-description">
            <p>Measure system performance, response times, and reliability under various conditions.</p>
            <div class="method-metrics">
              <span class="metric">Response time</span>
              <span class="metric">Uptime</span>
              <span class="metric">Error frequency</span>
              <span class="metric">Load capacity</span>
            </div>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-header">
            <span class="method-name">A/B Testing</span>
            <span class="method-type">Experimentation</span>
          </div>
          <div class="method-description">
            <p>Compare different interface designs and features to optimize user experience.</p>
            <div class="method-metrics">
              <span class="metric">Conversion rates</span>
              <span class="metric">User preference</span>
              <span class="metric">Task efficiency</span>
              <span class="metric">Error reduction</span>
            </div>
          </div>
        </div>
      </div>
    </div>
    
    <div class="method-category">
      <h4>🎯 Qualitative Research</h4>
      
      <div class="methods-list">
        <div class="method-item">
          <div class="method-header">
            <span class="method-name">User Interviews</span>
            <span class="method-type">Direct Feedback</span>
          </div>
          <div class="method-description">
            <p>Conduct structured interviews with engineers and managers to understand needs and pain points.</p>
            <div class="method-metrics">
              <span class="metric">User satisfaction</span>
              <span class="metric">Feature requests</span>
              <span class="metric">Pain points</span>
              <span class="metric">Workflow insights</span>
            </div>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-header">
            <span class="method-name">Contextual Inquiry</span>
            <span class="method-type">Field Study</span>
          </div>
          <div class="method-description">
            <p>Observe users in their natural work environment to understand real-world usage patterns.</p>
            <div class="method-metrics">
              <span class="metric">Work environment</span>
              <span class="metric">Task flow</span>
              <span class="metric">Environmental factors</span>
              <span class="metric">Integration needs</span>
            </div>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-header">
            <span class="method-name">Usability Testing</span>
            <span class="method-type">Task Analysis</span>
          </div>
          <div class="method-description">
            <p>Test interface usability with real users performing specific tasks in controlled environment.</p>
            <div class="method-metrics">
              <span class="metric">Task completion</span>
              <span class="metric">Time to complete</span>
              <span class="metric">Error frequency</span>
              <span class="metric">User satisfaction</span>
            </div>
          </div>
        </div>
      </div>
    </div>
    
    <div class="method-category">
      <h4>🛡️ Safety Research</h4>
      
      <div class="methods-list">
        <div class="method-item">
          <div class="method-header">
            <span class="method-name">Safety Audits</span>
            <span class="method-type">Compliance</span>
          </div>
          <div class="method-description">
            <p>Evaluate interface compliance with safety standards and emergency response requirements.</p>
            <div class="method-metrics">
              <span class="metric">Safety compliance</span>
              <span class="metric">Emergency access</span>
              <span class="metric">Response time</span>
              <span class="metric">Error prevention</span>
            </div>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-header">
            <span class="method-name">Emergency Scenarios</span>
            <span class="method-type">Simulation</span>
          </div>
          <div class="method-description">
            <p>Simulate emergency situations to test interface effectiveness in critical scenarios.</p>
            <div class="method-metrics">
              <span class="metric">Response accuracy</span>
              <span class="metric">Time to action</span>
              <span class="metric">Error rates</span>
              <span class="metric">User confidence</span>
            </div>
          </div>
        </div>
        
        <div class="method-item">
          <div class="method-header">
            <span class="method-name">Risk Assessment</span>
            <span class="method-type">Analysis</span>
          </div>
          <div class="method-description">
            <p>Identify potential risks and failure points in the user interface and interaction design.</p>
            <div class="method-metrics">
              <span class="metric">Risk identification</span>
              <span class="metric">Mitigation strategies</span>
              <span class="metric">Failure modes</span>
              <span class="metric">Safety improvements</span>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Research Methods CSS:**
```css
/* Research Methods */
.research-methods {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.methods-grid {
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

.methods-list {
  display: grid;
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

.method-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-md);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.method-name {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
  font-size: 1.125rem;
}

.method-type {
  background: var(--primary-orange);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-semibold);
  text-transform: uppercase;
}

.method-description p {
  margin-bottom: var(--space-md);
  color: var(--industrial-gray-600);
  line-height: 1.4;
}

.method-metrics {
  display: flex;
  flex-wrap: wrap;
  gap: var(--space-xs);
}

.metric {
  background: var(--bg-primary);
  color: var(--primary-orange);
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-medium);
  border: 1px solid var(--industrial-gray-300);
}
```

---

## 📚 **Tham chiếu:**

- [Design System](./REQ_UI_UX_Design_System.md) - Hệ thống thiết kế
- [Information Architecture](./REQ_UI_UX_Information_Architecture.md) - Kiến trúc thông tin
- [Interaction Design](./REQ_UI_UX_Interaction_Design.md) - Thiết kế tương tác
- [Usability Testing](./REQ_UI_UX_Usability_Testing.md) - Kiểm thử khả năng sử dụng

---

**Changelog v1.0:**
- ✅ Created comprehensive user research framework
- ✅ Defined primary and secondary user personas
- ✅ Implemented user journey mapping
- ✅ Added research methodology guidelines
- ✅ Created safety-focused research methods
- ✅ Included quantitative and qualitative approaches
- ✅ Added emergency response journey mapping
- ✅ Implemented daily operations journey
- ✅ Created research metrics and KPIs
- ✅ Added contextual inquiry guidelines
- ✅ Included usability testing framework
- ✅ Added safety audit methodology

**🚨 Lưu ý:** Tài liệu này tập trung vào understanding user needs với safety-first approach cho robot OHT-50, đảm bảo research methodology phù hợp với industrial environment và technical user base.
