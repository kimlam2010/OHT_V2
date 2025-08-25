# 📊 REQ_UI_UX_Report_Generation - Tạo báo cáo

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Data Analysts, Product Managers

---

## 🎯 **Mục tiêu Report Generation:**

### **Concept chính:**
- **System:** Comprehensive reporting system cho robot OHT-50
- **Focus:** Data-driven insights với safety và performance metrics
- **Style:** Industrial theme với clear data visualization
- **Target:** Quản lý, kỹ sư kỹ thuật cao, và stakeholders

### **Nguyên tắc thiết kế:**
1. **Data Accuracy:** Đảm bảo tính chính xác của dữ liệu
2. **Safety Focus:** Ưu tiên metrics an toàn và compliance
3. **Actionable Insights:** Cung cấp insights có thể hành động
4. **Export Flexibility:** Hỗ trợ nhiều format export

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| Report Types | Loại báo cáo | Data Analysts, Managers | Business Intelligence | ✅ Hoàn thành |
| Report Interface | Giao diện báo cáo | Designers, Developers | User Interface | ✅ Hoàn thành |
| Data Visualization | Hiển thị dữ liệu | Designers, Analysts | Data Presentation | ✅ Hoàn thành |
| Export System | Hệ thống xuất báo cáo | Developers, Users | Data Export | ✅ Hoàn thành |

---

## 📋 **1. REPORT TYPES - Loại báo cáo**

### **Report Categories:**
```html
<!-- Report Types -->
<div class="report-types">
  <h3>📋 Report Types</h3>
  
  <div class="report-categories">
    <div class="report-category safety">
      <div class="category-header">
        <span class="category-icon">🚨</span>
        <span class="category-name">Safety Reports</span>
        <span class="category-priority">Critical</span>
      </div>
      <div class="category-content">
        <div class="report-items">
          <div class="report-item">
            <span class="item-name">Safety Incident Report</span>
            <span class="item-frequency">Real-time</span>
            <span class="item-format">PDF, Excel</span>
          </div>
          <div class="report-item">
            <span class="item-name">E-Stop Activation Log</span>
            <span class="item-frequency">Daily</span>
            <span class="item-format">PDF, CSV</span>
          </div>
          <div class="report-item">
            <span class="item-name">Safety Compliance Report</span>
            <span class="item-frequency">Weekly</span>
            <span class="item-format">PDF, Excel</span>
          </div>
          <div class="report-item">
            <span class="item-name">Obstacle Detection Summary</span>
            <span class="item-frequency">Daily</span>
            <span class="item-format">PDF, CSV</span>
          </div>
        </div>
        <div class="category-description">
          <p>Critical safety reports for compliance and incident tracking. Required for regulatory purposes and safety audits.</p>
        </div>
      </div>
    </div>
    
    <div class="report-category operational">
      <div class="category-header">
        <span class="category-icon">⚙️</span>
        <span class="category-name">Operational Reports</span>
        <span class="category-priority">High</span>
      </div>
      <div class="category-content">
        <div class="report-items">
          <div class="report-item">
            <span class="item-name">Performance Metrics Report</span>
            <span class="item-frequency">Daily</span>
            <span class="item-format">PDF, Excel</span>
          </div>
          <div class="report-item">
            <span class="item-name">Task Completion Summary</span>
            <span class="item-frequency">Daily</span>
            <span class="item-format">PDF, CSV</span>
          </div>
          <div class="report-item">
            <span class="item-name">Battery Usage Report</span>
            <span class="item-frequency">Weekly</span>
            <span class="item-format">PDF, Excel</span>
          </div>
          <div class="report-item">
            <span class="item-name">Maintenance Schedule Report</span>
            <span class="item-frequency">Weekly</span>
            <span class="item-format">PDF, Excel</span>
          </div>
        </div>
        <div class="category-description">
          <p>Operational performance reports for efficiency analysis and maintenance planning.</p>
        </div>
      </div>
    </div>
    
    <div class="report-category system">
      <div class="category-header">
        <span class="category-icon">🔧</span>
        <span class="category-name">System Reports</span>
        <span class="category-priority">Medium</span>
      </div>
      <div class="category-content">
        <div class="report-items">
          <div class="report-item">
            <span class="item-name">System Health Report</span>
            <span class="item-frequency">Daily</span>
            <span class="item-format">PDF, Excel</span>
          </div>
          <div class="report-item">
            <span class="item-name">Error Log Summary</span>
            <span class="item-frequency">Daily</span>
            <span class="item-format">PDF, CSV</span>
          </div>
          <div class="report-item">
            <span class="item-name">Network Performance Report</span>
            <span class="item-frequency">Weekly</span>
            <span class="item-format">PDF, Excel</span>
          </div>
          <div class="report-item">
            <span class="item-name">User Activity Report</span>
            <span class="item-frequency">Weekly</span>
            <span class="item-format">PDF, Excel</span>
          </div>
        </div>
        <div class="category-description">
          <p>System-level reports for monitoring system health, errors, and user activities.</p>
        </div>
      </div>
    </div>
    
    <div class="report-category analytics">
      <div class="category-header">
        <span class="category-icon">📈</span>
        <span class="category-name">Analytics Reports</span>
        <span class="category-priority">Low</span>
      </div>
      <div class="category-content">
        <div class="report-items">
          <div class="report-item">
            <span class="item-name">Trend Analysis Report</span>
            <span class="item-frequency">Monthly</span>
            <span class="item-format">PDF, Excel</span>
          </div>
          <div class="report-item">
            <span class="item-name">Predictive Maintenance Report</span>
            <span class="item-frequency">Weekly</span>
            <span class="item-format">PDF, Excel</span>
          </div>
          <div class="report-item">
            <span class="item-name">Efficiency Optimization Report</span>
            <span class="item-frequency">Monthly</span>
            <span class="item-format">PDF, Excel</span>
          </div>
          <div class="report-item">
            <span class="item-name">Cost Analysis Report</span>
            <span class="item-frequency">Monthly</span>
            <span class="item-format">PDF, Excel</span>
          </div>
        </div>
        <div class="category-description">
          <p>Advanced analytics reports for trend analysis, predictive maintenance, and optimization insights.</p>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Report Types CSS:**
```css
/* Report Types */
.report-types {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.report-categories {
  display: grid;
  gap: var(--space-xl);
}

.report-category {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border-left: 4px solid var(--primary-orange);
  transition: all 0.3s ease;
}

.report-category:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.report-category.safety {
  border-left-color: var(--error-red);
  background: rgba(220, 53, 69, 0.05);
}

.report-category.operational {
  border-left-color: var(--primary-orange);
  background: rgba(255, 107, 53, 0.05);
}

.report-category.system {
  border-left-color: var(--info-blue);
  background: rgba(23, 162, 184, 0.05);
}

.report-category.analytics {
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

.report-category.safety .category-priority {
  background: var(--error-red);
}

.report-category.operational .category-priority {
  background: var(--primary-orange);
}

.report-category.system .category-priority {
  background: var(--info-blue);
}

.report-category.analytics .category-priority {
  background: var(--success-green);
}

.report-items {
  display: grid;
  gap: var(--space-md);
  margin-bottom: var(--space-lg);
}

.report-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-primary);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
  transition: all 0.2s ease;
}

.report-item:hover {
  background: rgba(255, 107, 53, 0.1);
  transform: translateX(4px);
}

.item-name {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
}

.item-frequency {
  background: var(--bg-secondary);
  color: var(--primary-orange);
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-medium);
}

.item-format {
  background: var(--success-green);
  color: white;
  padding: var(--space-xs) var(--space-sm);
  border-radius: 0.25rem;
  font-size: 0.75rem;
  font-weight: var(--font-semibold);
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

## 🎛️ **2. REPORT INTERFACE - Giao diện báo cáo**

### **Report Generation Interface:**
```html
<!-- Report Generation Interface -->
<div class="report-generation-interface">
  <h3>🎛️ Report Generation Interface</h3>
  
  <div class="report-layout">
    <div class="report-sidebar">
      <div class="sidebar-header">
        <h4>Report Categories</h4>
        <button class="sidebar-toggle" id="reportSidebarToggle">☰</button>
      </div>
      
      <nav class="report-nav">
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
        
        <a href="#analytics" class="nav-item" data-category="analytics">
          <span class="nav-icon">📈</span>
          <span class="nav-label">Analytics</span>
          <span class="nav-count">4</span>
        </a>
      </nav>
    </div>
    
    <div class="report-main">
      <div class="report-header">
        <div class="report-title">
          <h2 id="reportTitle">Safety Incident Report</h2>
          <p id="reportDescription">Comprehensive safety incident tracking and analysis.</p>
        </div>
        
        <div class="report-actions">
          <button class="action-btn secondary" id="scheduleBtn">
            <span class="btn-icon">📅</span>
            <span class="btn-text">Schedule</span>
          </button>
          <button class="action-btn primary" id="generateBtn">
            <span class="btn-icon">📊</span>
            <span class="btn-text">Generate Report</span>
          </button>
        </div>
      </div>
      
      <div class="report-content">
        <div class="report-configuration">
          <div class="config-section">
            <h3>Report Parameters</h3>
            
            <div class="config-form">
              <div class="form-group">
                <label for="dateRange">Date Range</label>
                <div class="date-range-selector">
                  <select id="dateRange">
                    <option value="today">Today</option>
                    <option value="yesterday">Yesterday</option>
                    <option value="last7days">Last 7 Days</option>
                    <option value="last30days" selected>Last 30 Days</option>
                    <option value="last90days">Last 90 Days</option>
                    <option value="custom">Custom Range</option>
                  </select>
                </div>
              </div>
              
              <div class="form-group">
                <label for="reportFormat">Report Format</label>
                <div class="format-selector">
                  <label class="format-option">
                    <input type="radio" name="format" value="pdf" checked>
                    <span class="format-label">PDF</span>
                  </label>
                  <label class="format-option">
                    <input type="radio" name="format" value="excel">
                    <span class="format-label">Excel</span>
                  </label>
                  <label class="format-option">
                    <input type="radio" name="format" value="csv">
                    <span class="format-label">CSV</span>
                  </label>
                </div>
              </div>
              
              <div class="form-group">
                <label for="includeCharts">Include Charts</label>
                <div class="toggle-switch">
                  <input type="checkbox" id="includeCharts" checked>
                  <span class="toggle-slider"></span>
                </div>
              </div>
              
              <div class="form-group">
                <label for="includeDetails">Include Detailed Data</label>
                <div class="toggle-switch">
                  <input type="checkbox" id="includeDetails" checked>
                  <span class="toggle-slider"></span>
                </div>
              </div>
            </div>
          </div>
          
          <div class="config-section">
            <h3>Report Preview</h3>
            
            <div class="report-preview">
              <div class="preview-header">
                <h4>Safety Incident Report - Last 30 Days</h4>
                <span class="preview-date">Generated: 2025-01-28 14:30</span>
              </div>
              
              <div class="preview-content">
                <div class="preview-summary">
                  <div class="summary-item">
                    <span class="summary-label">Total Incidents:</span>
                    <span class="summary-value">12</span>
                  </div>
                  <div class="summary-item">
                    <span class="summary-label">Critical Incidents:</span>
                    <span class="summary-value critical">2</span>
                  </div>
                  <div class="summary-item">
                    <span class="summary-label">Resolved:</span>
                    <span class="summary-value resolved">10</span>
                  </div>
                  <div class="summary-item">
                    <span class="summary-label">Pending:</span>
                    <span class="summary-value pending">2</span>
                  </div>
                </div>
                
                <div class="preview-chart">
                  <div class="chart-placeholder">
                    <span class="chart-icon">📊</span>
                    <span class="chart-text">Incident Trend Chart</span>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Report Interface CSS:**
```css
/* Report Generation Interface */
.report-generation-interface {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.report-layout {
  display: grid;
  grid-template-columns: 280px 1fr;
  gap: var(--space-lg);
  min-height: 600px;
}

/* Sidebar */
.report-sidebar {
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

.report-nav {
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
.report-main {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  border: 1px solid var(--industrial-gray-300);
  overflow: hidden;
}

.report-header {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  padding: var(--space-lg);
  border-bottom: 1px solid var(--industrial-gray-300);
  background: var(--bg-primary);
}

.report-title h2 {
  margin: 0 0 var(--space-sm) 0;
  color: var(--industrial-gray-700);
  font-weight: var(--font-bold);
}

.report-title p {
  margin: 0;
  color: var(--industrial-gray-600);
  font-size: 0.875rem;
}

.report-actions {
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

.report-content {
  padding: var(--space-lg);
}

.report-configuration {
  display: grid;
  gap: var(--space-xl);
}

.config-section {
  background: var(--bg-primary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border: 1px solid var(--industrial-gray-300);
}

.config-section h3 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

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

.date-range-selector select {
  padding: var(--space-sm);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.5rem;
  font-size: 0.875rem;
  color: var(--industrial-gray-700);
  background: var(--bg-secondary);
  cursor: pointer;
}

.format-selector {
  display: flex;
  gap: var(--space-md);
}

.format-option {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
  cursor: pointer;
}

.format-option input[type="radio"] {
  accent-color: var(--primary-orange);
}

.format-label {
  font-weight: var(--font-medium);
  color: var(--industrial-gray-700);
}

/* Report Preview */
.report-preview {
  background: var(--bg-secondary);
  border-radius: 0.5rem;
  border: 1px solid var(--industrial-gray-300);
  overflow: hidden;
}

.preview-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-primary);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.preview-header h4 {
  margin: 0;
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.preview-date {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
}

.preview-content {
  padding: var(--space-lg);
}

.preview-summary {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: var(--space-md);
  margin-bottom: var(--space-lg);
}

.summary-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-primary);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
}

.summary-label {
  font-weight: var(--font-medium);
  color: var(--industrial-gray-600);
}

.summary-value {
  font-weight: var(--font-bold);
  font-size: 1.25rem;
  color: var(--industrial-gray-700);
}

.summary-value.critical {
  color: var(--error-red);
}

.summary-value.resolved {
  color: var(--success-green);
}

.summary-value.pending {
  color: var(--warning-yellow);
}

.preview-chart {
  background: var(--bg-primary);
  border-radius: 0.5rem;
  padding: var(--space-xl);
  border: 2px dashed var(--industrial-gray-300);
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: var(--space-md);
}

.chart-placeholder {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: var(--space-sm);
}

.chart-icon {
  font-size: 3rem;
  color: var(--industrial-gray-400);
}

.chart-text {
  font-size: 1.125rem;
  color: var(--industrial-gray-600);
  font-weight: var(--font-medium);
}
```

---

## 📈 **3. DATA VISUALIZATION - Hiển thị dữ liệu**

### **Chart Components:**
```html
<!-- Data Visualization -->
<div class="data-visualization">
  <h3>📈 Data Visualization</h3>
  
  <div class="chart-components">
    <div class="chart-section">
      <h4>Safety Metrics Dashboard</h4>
      
      <div class="chart-grid">
        <div class="chart-card">
          <div class="chart-header">
            <h5>E-Stop Activations</h5>
            <span class="chart-period">Last 30 Days</span>
          </div>
          <div class="chart-container">
            <div class="line-chart">
              <div class="chart-line" style="height: 60%; background: var(--error-red);"></div>
              <div class="chart-line" style="height: 40%; background: var(--error-red);"></div>
              <div class="chart-line" style="height: 80%; background: var(--error-red);"></div>
              <div class="chart-line" style="height: 20%; background: var(--error-red);"></div>
              <div class="chart-line" style="height: 50%; background: var(--error-red);"></div>
            </div>
            <div class="chart-labels">
              <span>Week 1</span>
              <span>Week 2</span>
              <span>Week 3</span>
              <span>Week 4</span>
              <span>Current</span>
            </div>
          </div>
        </div>
        
        <div class="chart-card">
          <div class="chart-header">
            <h5>Safety Compliance</h5>
            <span class="chart-period">Current Month</span>
          </div>
          <div class="chart-container">
            <div class="pie-chart">
              <div class="pie-segment" style="--percentage: 85; --color: var(--success-green);"></div>
              <div class="pie-segment" style="--percentage: 10; --color: var(--warning-yellow);"></div>
              <div class="pie-segment" style="--percentage: 5; --color: var(--error-red);"></div>
              <div class="pie-center">
                <span class="pie-value">85%</span>
                <span class="pie-label">Compliant</span>
              </div>
            </div>
          </div>
        </div>
        
        <div class="chart-card">
          <div class="chart-header">
            <h5>Incident Severity</h5>
            <span class="chart-period">Last 30 Days</span>
          </div>
          <div class="chart-container">
            <div class="bar-chart">
              <div class="bar-group">
                <div class="bar critical" style="height: 80%;"></div>
                <span class="bar-label">Critical</span>
              </div>
              <div class="bar-group">
                <div class="bar warning" style="height: 60%;"></div>
                <span class="bar-label">Warning</span>
              </div>
              <div class="bar-group">
                <div class="bar info" style="height: 40%;"></div>
                <span class="bar-label">Info</span>
              </div>
            </div>
          </div>
        </div>
        
        <div class="chart-card">
          <div class="chart-header">
            <h5>Response Time</h5>
            <span class="chart-period">Average (ms)</span>
          </div>
          <div class="chart-container">
            <div class="gauge-chart">
              <div class="gauge-background"></div>
              <div class="gauge-fill" style="--percentage: 75; --color: var(--primary-orange);"></div>
              <div class="gauge-center">
                <span class="gauge-value">150ms</span>
                <span class="gauge-label">Avg Response</span>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
    
    <div class="chart-section">
      <h4>Performance Metrics</h4>
      
      <div class="metrics-grid">
        <div class="metric-card">
          <div class="metric-header">
            <span class="metric-icon">⚡</span>
            <span class="metric-title">Uptime</span>
          </div>
          <div class="metric-value">99.8%</div>
          <div class="metric-trend positive">
            <span class="trend-icon">↗</span>
            <span class="trend-value">+0.2%</span>
          </div>
        </div>
        
        <div class="metric-card">
          <div class="metric-header">
            <span class="metric-icon">🎯</span>
            <span class="metric-title">Task Success Rate</span>
          </div>
          <div class="metric-value">97.5%</div>
          <div class="metric-trend positive">
            <span class="trend-icon">↗</span>
            <span class="trend-value">+1.2%</span>
          </div>
        </div>
        
        <div class="metric-card">
          <div class="metric-header">
            <span class="metric-icon">🔋</span>
            <span class="metric-title">Battery Efficiency</span>
          </div>
          <div class="metric-value">92.3%</div>
          <div class="metric-trend negative">
            <span class="trend-icon">↘</span>
            <span class="trend-value">-0.5%</span>
          </div>
        </div>
        
        <div class="metric-card">
          <div class="metric-header">
            <span class="metric-icon">🚀</span>
            <span class="metric-title">Average Speed</span>
          </div>
          <div class="metric-value">1.8 m/s</div>
          <div class="metric-trend positive">
            <span class="trend-icon">↗</span>
            <span class="trend-value">+0.1 m/s</span>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### **Data Visualization CSS:**
```css
/* Data Visualization */
.data-visualization {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.chart-components {
  display: grid;
  gap: var(--space-xl);
}

.chart-section h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
  border-bottom: 2px solid var(--primary-orange);
  padding-bottom: var(--space-sm);
}

.chart-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: var(--space-lg);
}

.chart-card {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border: 1px solid var(--industrial-gray-300);
  transition: all 0.3s ease;
}

.chart-card:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.chart-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-lg);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.chart-header h5 {
  margin: 0;
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.chart-period {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
}

.chart-container {
  min-height: 200px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
}

/* Line Chart */
.line-chart {
  display: flex;
  align-items: end;
  gap: var(--space-sm);
  height: 120px;
  width: 100%;
  margin-bottom: var(--space-sm);
}

.chart-line {
  flex: 1;
  border-radius: 2px 2px 0 0;
  transition: all 0.3s ease;
}

.chart-labels {
  display: flex;
  justify-content: space-between;
  width: 100%;
  font-size: 0.75rem;
  color: var(--industrial-gray-600);
}

/* Pie Chart */
.pie-chart {
  position: relative;
  width: 150px;
  height: 150px;
  border-radius: 50%;
  background: conic-gradient(
    var(--success-green) 0deg 306deg,
    var(--warning-yellow) 306deg 324deg,
    var(--error-red) 324deg 360deg
  );
}

.pie-center {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  text-align: center;
  background: var(--bg-secondary);
  border-radius: 50%;
  width: 60px;
  height: 60px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
}

.pie-value {
  font-size: 1rem;
  font-weight: var(--font-bold);
  color: var(--success-green);
}

.pie-label {
  font-size: 0.75rem;
  color: var(--industrial-gray-600);
}

/* Bar Chart */
.bar-chart {
  display: flex;
  align-items: end;
  gap: var(--space-md);
  height: 120px;
  width: 100%;
}

.bar-group {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: var(--space-sm);
  flex: 1;
}

.bar {
  width: 100%;
  border-radius: 2px 2px 0 0;
  transition: all 0.3s ease;
}

.bar.critical {
  background: var(--error-red);
}

.bar.warning {
  background: var(--warning-yellow);
}

.bar.info {
  background: var(--info-blue);
}

.bar-label {
  font-size: 0.75rem;
  color: var(--industrial-gray-600);
  text-align: center;
}

/* Gauge Chart */
.gauge-chart {
  position: relative;
  width: 150px;
  height: 75px;
  overflow: hidden;
}

.gauge-background {
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  border-radius: 75px 75px 0 0;
  background: var(--industrial-gray-300);
}

.gauge-fill {
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  border-radius: 75px 75px 0 0;
  background: conic-gradient(var(--primary-orange) 0deg calc(var(--percentage) * 1.8deg), transparent calc(var(--percentage) * 1.8deg) 180deg);
  transform: rotate(-90deg);
}

.gauge-center {
  position: absolute;
  bottom: 0;
  left: 50%;
  transform: translateX(-50%);
  text-align: center;
}

.gauge-value {
  display: block;
  font-size: 1.25rem;
  font-weight: var(--font-bold);
  color: var(--industrial-gray-700);
}

.gauge-label {
  display: block;
  font-size: 0.75rem;
  color: var(--industrial-gray-600);
}

/* Metrics Grid */
.metrics-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: var(--space-lg);
}

.metric-card {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  border: 1px solid var(--industrial-gray-300);
  transition: all 0.3s ease;
}

.metric-card:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.metric-header {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
  margin-bottom: var(--space-md);
}

.metric-icon {
  font-size: 1.5rem;
}

.metric-title {
  font-weight: var(--font-medium);
  color: var(--industrial-gray-600);
}

.metric-value {
  font-size: 2rem;
  font-weight: var(--font-bold);
  color: var(--industrial-gray-700);
  margin-bottom: var(--space-sm);
}

.metric-trend {
  display: flex;
  align-items: center;
  gap: var(--space-xs);
  font-size: 0.875rem;
  font-weight: var(--font-medium);
}

.metric-trend.positive {
  color: var(--success-green);
}

.metric-trend.negative {
  color: var(--error-red);
}

.trend-icon {
  font-size: 1rem;
}
```

---

## 📚 **Tham chiếu:**

- [Visual Design](./REQ_UI_UX_Visual_Design.md) - Thiết kế hình ảnh
- [Dashboard Design](./REQ_UI_UX_Dashboard_Design.md) - Thiết kế dashboard
- [Configuration Interface](./REQ_UI_UX_Configuration_Interface.md) - Giao diện cấu hình
- [Alert System](./REQ_UI_UX_Alert_System.md) - Hệ thống cảnh báo

---

**Changelog v1.0:**
- ✅ Created comprehensive report generation system
- ✅ Defined report categories and types
- ✅ Implemented safety-focused reporting
- ✅ Added data visualization components
- ✅ Created report generation interface
- ✅ Included export functionality
- ✅ Added chart components (line, pie, bar, gauge)
- ✅ Implemented metrics dashboard
- ✅ Created report scheduling system
- ✅ Added report customization options
- ✅ Included report preview functionality
- ✅ Added multiple export formats (PDF, Excel, CSV)

**🚨 Lưu ý:** Tài liệu này tập trung vào data-driven reporting với safety-first approach cho robot OHT-50, đảm bảo insights có thể hành động và compliance reporting.
