# 🏗️ REQ_UI_UX_Information_Architecture - Kiến trúc thông tin

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Product Managers

---

## 🎯 **Mục tiêu kiến trúc thông tin:**

### **Mục đích chính:**
- Tổ chức thông tin hệ thống OHT-50 một cách logic và dễ hiểu
- Đảm bảo người dùng tìm thấy thông tin cần thiết nhanh chóng
- Tối ưu hóa workflow cho Quản lý và Kỹ sư
- Đảm bảo safety và emergency response nhanh chóng

### **Nguyên tắc thiết kế:**
1. **Safety First:** Thông tin an toàn luôn ưu tiên cao nhất
2. **Real-time Priority:** Thông tin real-time được hiển thị nổi bật
3. **Progressive Disclosure:** Thông tin hiển thị theo cấp độ quan trọng
4. **Consistent Navigation:** Điều hướng nhất quán trong toàn bộ hệ thống
5. **Quick Access:** Truy cập nhanh đến các chức năng quan trọng

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| **Site Map** | Cấu trúc tổng thể website | UI/UX Designers | Navigation design | ✅ Hoàn thành |
| **Content Hierarchy** | Phân cấp thông tin | UI/UX Designers | Information design | ✅ Hoàn thành |
| **Navigation Structure** | Cấu trúc điều hướng | UI/UX Designers, FE Devs | User experience | ✅ Hoàn thành |
| **User Flows** | Luồng sử dụng chính | UI/UX Designers | User journey | ✅ Hoàn thành |
| **Data Architecture** | Cấu trúc dữ liệu | FE Devs, Backend | Data management | ✅ Hoàn thành |

---

## 🗺️ **SITE MAP - Cấu trúc tổng thể:**

```
OHT-50 Dashboard
├── 🔐 Authentication
│   ├── Login
│   ├── Logout
│   └── Password Reset
│
├── 🏠 Dashboard (Main)
│   ├── Robot Status Overview
│   ├── Real-time Alerts
│   ├── Quick Actions
│   └── System Health
│
├── 🤖 Robot Control
│   ├── Manual Control Panel
│   ├── Movement Controls
│   ├── Emergency Stop
│   └── Voice Commands
│
├── 🗺️ Map & Navigation
│   ├── Live Robot Location
│   ├── LiDAR 360° View
│   ├── Dock Location Status
│   └── Path Planning
│
├── 📊 Monitoring
│   ├── Real-time Telemetry
│   ├── Speed & Position
│   ├── Connection Status
│   └── System Performance
│
├── ⚠️ Safety & Alarms
│   ├── Active Alerts
│   ├── Alert History
│   ├── Safety Status
│   └── Emergency Procedures
│
├── ⚙️ Configuration
│   ├── Robot Settings
│   ├── System Parameters
│   ├── User Management
│   └── Access Control
│
├── 📈 Analytics
│   ├── Mission Statistics
│   ├── Error Reports
│   ├── Performance Metrics
│   └── Usage Analytics
│
├── 📋 Missions
│   ├── Active Missions
│   ├── Mission History
│   ├── Mission Planning
│   └── Center Integration
│
└── 📚 Documentation
    ├── User Manual
    ├── Troubleshooting
    ├── API Documentation
    └── System Logs
```

---

## 📋 **CONTENT HIERARCHY - Phân cấp thông tin:**

### **Level 1: Critical Information (Always Visible)**
```
🔴 Emergency Stop Button
⚠️ Active Alerts Banner
🤖 Robot Status Indicator
📍 Current Location
🔗 Connection Status
```

### **Level 2: Primary Information (Dashboard)**
```
📊 Real-time Telemetry
🎮 Manual Control Panel
🗺️ Live Map View
⚡ System Performance
📈 Quick Statistics
```

### **Level 3: Secondary Information (Detailed Views)**
```
📋 Mission Details
⚙️ Configuration Settings
📊 Historical Data
🔧 Maintenance Info
📚 Documentation
```

### **Level 4: Supporting Information (Help & Settings)**
```
❓ Help & Support
👤 User Profile
⚙️ System Settings
📝 Activity Logs
🔒 Security Settings
```

---

## 🧭 **NAVIGATION STRUCTURE - Cấu trúc điều hướng:**

### **Primary Navigation (Top Bar):**
```
[Logo] OHT-50 | Dashboard | Robot Control | Map | Monitoring | Safety | Config | Analytics | Missions | Docs
```

### **Secondary Navigation (Sidebar):**
```
🏠 Dashboard
├── 📊 Overview
├── ⚡ Quick Actions
└── 📈 System Health

🤖 Robot Control
├── 🎮 Manual Control
├── 🚀 Movement
├── 🛑 Emergency Stop
└── 🎤 Voice Commands

🗺️ Map & Navigation
├── 📍 Live Location
├── 🔍 LiDAR View
├── 🏠 Dock Status
└── 🛣️ Path Planning

📊 Monitoring
├── 📡 Telemetry
├── 🏃 Speed & Position
├── 🔗 Connection
└── ⚡ Performance

⚠️ Safety & Alarms
├── 🚨 Active Alerts
├── 📋 Alert History
├── 🛡️ Safety Status
└── 🆘 Emergency

⚙️ Configuration
├── 🤖 Robot Settings
├── ⚙️ System Params
├── 👥 User Management
└── 🔒 Access Control
```

### **Contextual Navigation:**
```
Breadcrumbs: Dashboard > Robot Control > Manual Control
Quick Actions: [Emergency Stop] [Pause] [Resume] [Home]
Status Bar: Connected | Battery: 85% | Speed: 2.5 m/s | Location: Zone A
```

---

## 🔄 **USER FLOWS - Luồng sử dụng chính:**

### **Flow 1: Daily Monitoring (Quản lý)**
```
1. Login → Dashboard
2. Check Robot Status → Green/Red/Yellow
3. Review Active Alerts → No alerts or View details
4. Monitor Real-time Data → Speed, Position, Battery
5. Check Mission Status → Active/Completed/Failed
6. Review System Health → All systems OK
```

### **Flow 2: Emergency Response (Kỹ sư)**
```
1. Alert Notification → Sound + Visual
2. Quick Assessment → Alert level + Location
3. Emergency Stop → Immediate action
4. Safety Check → Robot stopped safely
5. Problem Diagnosis → Check error logs
6. Resolution Action → Manual intervention
7. System Recovery → Resume operations
```

### **Flow 3: Manual Control (Kỹ sư)**
```
1. Navigate to Robot Control
2. Check Safety Status → All clear
3. Enable Manual Mode → Confirmation required
4. Use Control Panel → Movement controls
5. Monitor Real-time → Position, speed, obstacles
6. Execute Commands → Forward, backward, turn
7. Disable Manual Mode → Return to auto
```

### **Flow 4: Configuration (Admin)**
```
1. Navigate to Configuration
2. Select Robot Settings
3. Modify Parameters → Speed limits, safety zones
4. Save Changes → Confirmation dialog
5. Test Configuration → Validate settings
6. Deploy Changes → Apply to robot
```

---

## 💾 **DATA ARCHITECTURE - Cấu trúc dữ liệu:**

### **Real-time Data (1s update):**
```typescript
interface RobotStatus {
  // Core Status
  status: 'idle' | 'moving' | 'docking' | 'error' | 'emergency';
  battery: number; // 0-100%
  speed: number; // m/s
  position: { x: number; y: number; z: number };
  heading: number; // degrees
  
  // Safety
  emergencyStop: boolean;
  safetyZone: 'clear' | 'warning' | 'violation';
  obstacleDetected: boolean;
  
  // Connection
  connectionStatus: 'connected' | 'disconnected' | 'weak';
  signalStrength: number; // 0-100%
  
  // System
  temperature: number;
  humidity: number;
  systemHealth: 'good' | 'warning' | 'critical';
}

interface AlertData {
  id: string;
  type: 'error' | 'warning' | 'info' | 'emergency';
  severity: 'low' | 'medium' | 'high' | 'critical';
  message: string;
  timestamp: Date;
  location?: { x: number; y: number };
  resolved: boolean;
  actionRequired: boolean;
}

interface TelemetryData {
  timestamp: Date;
  robotStatus: RobotStatus;
  alerts: AlertData[];
  performance: {
    cpu: number;
    memory: number;
    network: number;
  };
}
```

### **Historical Data:**
```typescript
interface MissionData {
  id: string;
  name: string;
  status: 'pending' | 'active' | 'completed' | 'failed';
  startTime: Date;
  endTime?: Date;
  route: Array<{ x: number; y: number }>;
  statistics: {
    distance: number;
    duration: number;
    stops: number;
    errors: number;
  };
}

interface SystemLog {
  id: string;
  timestamp: Date;
  level: 'info' | 'warning' | 'error' | 'critical';
  category: 'system' | 'safety' | 'communication' | 'mission';
  message: string;
  details?: any;
}
```

---

## 🎨 **INFORMATION DESIGN PRINCIPLES:**

### **1. Visual Hierarchy:**
- **Critical Information:** Red, large, prominent
- **Important Information:** Orange, medium, visible
- **Normal Information:** Blue, standard, readable
- **Supporting Information:** Gray, small, subtle

### **2. Information Density:**
- **Dashboard:** < 20 key metrics
- **Detail Views:** < 50 data points
- **Configuration:** < 100 settings
- **Reports:** Paginated, searchable

### **3. Progressive Disclosure:**
- **Level 1:** Essential info (always visible)
- **Level 2:** Important info (dashboard)
- **Level 3:** Detailed info (click to expand)
- **Level 4:** Supporting info (help, settings)

### **4. Contextual Information:**
- **Hover Tooltips:** Additional details
- **Context Menus:** Quick actions
- **Breadcrumbs:** Navigation context
- **Status Indicators:** Visual feedback

---

## 📱 **RESPONSIVE INFORMATION ARCHITECTURE:**

### **Desktop (1920x1080):**
- **Full Dashboard:** All sections visible
- **Multi-column Layout:** 3-4 columns
- **Detailed Controls:** Full control panel
- **Large Map View:** Full screen map

### **Tablet (1024x768):**
- **Adaptive Dashboard:** 2-3 columns
- **Collapsible Sidebar:** Hamburger menu
- **Touch-friendly Controls:** Larger buttons
- **Medium Map View:** 70% screen

### **Mobile (375x667):**
- **Single Column:** Stacked layout
- **Bottom Navigation:** Tab bar
- **Essential Controls:** Emergency + basic
- **Compact Map:** 50% screen

---

## 🔍 **SEARCH & FILTER ARCHITECTURE:**

### **Global Search:**
```
Search: [🔍 Type to search...]
├── Quick Results
│   ├── Robot Status
│   ├── Active Alerts
│   ├── Recent Missions
│   └── System Settings
└── Advanced Search
    ├── Date Range
    ├── Alert Type
    ├── Mission Status
    └── System Category
```

### **Filter System:**
```
Filters: [All] [Active] [Completed] [Failed]
├── Time: [Today] [Week] [Month] [Custom]
├── Type: [Error] [Warning] [Info] [Emergency]
├── Location: [Zone A] [Zone B] [Zone C]
└── Status: [Online] [Offline] [Maintenance]
```

---

## 📊 **INFORMATION METRICS:**

### **Performance Targets:**
- **Information Discovery:** < 3 clicks
- **Critical Action Access:** < 1 click
- **Data Loading Time:** < 1 second
- **Search Results:** < 2 seconds
- **Navigation Accuracy:** > 95%

### **Usability Metrics:**
- **Task Completion Rate:** > 90%
- **Error Rate:** < 5%
- **User Satisfaction:** > 4.5/5
- **Time to Complete Tasks:** < 30 seconds
- **Information Recall:** > 80%

---

**Changelog:**
- v1.0 (2025-01-28): Initial information architecture for OHT-50
- Added site map structure
- Added content hierarchy
- Added navigation structure
- Added user flows
- Added data architecture

---

## 🎯 **PHẠM VI OHT-50 - THEO UI/UX REQUIREMENTS**

### **Dashboard Requirements (theo REQ_UI_UX_Dashboard_Design.md):**
- **Layout Structure:** Multi-panel dashboard với 12-column grid system
- **Status Panel:** Robot status overview với real-time indicators
- **Control Panel:** Manual control interface với emergency controls
- **Map Interface:** LiDAR 360° visualization với interactive controls

### **UI Components (theo các tài liệu UI/UX):**

#### **1. Core Dashboard Components (theo REQ_UI_UX_Dashboard_Design.md):**
- **Robot Status Card:** Operating mode, battery, speed, position, connection, temperature
- **Manual Control Panel:** Movement controls, speed control, emergency controls
- **LiDAR Map Interface:** Real-time 360° visualization, obstacle detection, path planning

#### **2. Control Panel Components (theo REQ_UI_UX_Control_Panel.md):**
- **Control Mode Selector:** AUTO/MANUAL/SEMI/MAINT modes
- **Movement Controls:** Directional buttons (forward, backward, left, right, stop)
- **Speed Control:** Slider và presets (Slow, Normal, Fast, Max)
- **Emergency Controls:** E-Stop, Pause, Reset, Home buttons

#### **3. Alert System Components (theo REQ_UI_UX_Alert_System.md):**
- **Alert Banner System:** Critical, Warning, Info, Success alerts
- **Notification Center:** Filterable notifications với actions
- **Audio Notifications:** Configurable audio alerts với volume control

#### **4. Configuration Interface (theo REQ_UI_UX_Configuration_Interface.md):**
- **Configuration Categories:** Safety, Operational, System, Interface
- **Configuration Panel:** Sidebar navigation với form controls
- **Validation System:** Real-time validation với safety checks

#### **5. Map Interface Components (theo REQ_UI_UX_Map_Interface.md):**
- **LiDAR Visualization:** Real-time point cloud rendering
- **Navigation Path:** Path planning với waypoints và safety corridor
- **Obstacle Detection:** Clustering algorithm với classification
- **Interactive Controls:** Zoom, pan, reset view, fullscreen

#### **6. Accessibility Features (theo REQ_UI_UX_Accessibility.md):**
- **Visual Accessibility:** Color contrast, typography, focus indicators
- **Motor Accessibility:** Keyboard navigation, touch targets
- **Cognitive Accessibility:** Clear hierarchy, consistent patterns
- **Assistive Technology:** Screen reader support, semantic HTML

### **Design System Integration (theo REQ_UI_UX_Design_System.md):**
- **Design Tokens:** Color system, typography, spacing, shadows
- **Component Library:** Button, Form, Data Display, Navigation, Feedback components
- **Responsive Design:** Mobile-first approach với breakpoints
- **Accessibility Guidelines:** WCAG 2.1 AA compliance

### **Information Architecture (theo REQ_UI_UX_Information_Architecture.md):**
- **Site Map:** 9 main sections từ Authentication đến Documentation
- **Content Hierarchy:** 4 levels từ Critical đến Supporting information
- **Navigation Structure:** Primary navigation + Secondary sidebar
- **User Flows:** 4 main flows (Daily Monitoring, Emergency Response, Manual Control, Configuration)

### **Interaction Design (theo REQ_UI_UX_Interaction_Design.md):**
- **Emergency Controls:** Safety-first với immediate response
- **Manual Control Panel:** Intuitive movement controls với real-time feedback
- **Voice Commands:** Voice control interface với command recognition
- **Visual Feedback:** Loading states, status indicators, alert notifications

---

**Next Steps:**
- Create wireframes based on this architecture
- Implement navigation structure
- Design information hierarchy
- Test user flows
- Validate with stakeholders
