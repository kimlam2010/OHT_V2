# 🖥️ REQ_FE_Architecture - Yêu cầu kiến trúc Frontend

**Mục đích:** Định nghĩa yêu cầu kiến trúc Frontend cho OHT-50 Master Module Dashboard

---

## 📋 **Thông tin tài liệu:**
- **Phiên bản:** v2.0
- **Ngày tạo:** 2025-01-28
- **Người tạo:** PM Team
- **Trạng thái:** Implementation Guide
- **Cập nhật theo:** System Architecture v2.0

---

## 🎯 **Mục tiêu:**

### **Yêu cầu chính:**
1. **Real-time Dashboard** - Dashboard thời gian thực cho 5 module bắt buộc
2. **Module Management UI** - Giao diện quản lý 11 module (5+3+3)
3. **Safety Monitoring** - Giám sát an toàn với 4 analog + 4 digital sensors
4. **Responsive Design** - Thiết kế responsive cho mobile/tablet/desktop
5. **User Experience** - UX tối ưu cho operator và technician

---

## 🏗️ **Kiến trúc Frontend:**

### **1. Presentation Layer:**
```
┌─────────────────────────────────────┐
│         Dashboard Layout            │
│  ┌─────────────┬─────────────────┐  │
│  │ Header Bar  │  Navigation     │  │
│  ├─────────────┼─────────────────┤  │
│  │ Sidebar     │  Main Content   │  │
│  ├─────────────┼─────────────────┤  │
│  │ Status Bar  │  Action Panel   │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

**Components:**
- ✅ **HeaderBar** - System status, user info, notifications
- ✅ **Navigation** - Module navigation, breadcrumbs
- ✅ **Sidebar** - Module tree, quick actions
- ✅ **MainContent** - Module-specific dashboards
- ✅ **StatusBar** - Real-time status, alerts
- ✅ **ActionPanel** - Control buttons, emergency actions

### **2. Module Dashboard Components:**

#### **2.1 Master Control Dashboard:**
```
┌─────────────────────────────────────┐
│         Master Control              │
│  ┌─────────────┬─────────────────┐  │
│  │ System      │  Network        │  │
│  │ Status      │  Status         │  │
│  ├─────────────┼─────────────────┤  │
│  │ Module      │  Communication  │  │
│  │ Overview    │  Status         │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

#### **2.2 Power Module Dashboard:**
```
┌─────────────────────────────────────┐
│         Power Management            │
│  ┌─────────────┬─────────────────┐  │
│  │ Battery     │  Charger        │  │
│  │ Status      │  Status         │  │
│  ├─────────────┼─────────────────┤  │
│  │ Voltage     │  Current        │  │
│  │ Monitor     │  Monitor        │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

#### **2.3 Safety Module Dashboard:**
```
┌─────────────────────────────────────┐
│         Safety Monitoring           │
│  ┌─────────────┬─────────────────┐  │
│  │ Zone 1-4    │  Proximity      │  │
│  │ Distance    │  Sensors        │  │
│  ├─────────────┼─────────────────┤  │
│  │ Safety      │  Relay          │  │
│  │ Status      │  Controls       │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

#### **2.4 Travel Motor Dashboard:**
```
┌─────────────────────────────────────┐
│         Travel Motor Control        │
│  ┌─────────────┬─────────────────┐  │
│  │ Motor 1     │  Motor 2        │  │
│  │ Control     │  Control        │  │
│  ├─────────────┼─────────────────┤  │
│  │ Digital     │  Digital        │  │
│  │ Inputs      │  Outputs        │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

#### **2.5 Dock & Location Dashboard:**
```
┌─────────────────────────────────────┐
│         Dock & Location             │
│  ┌─────────────┬─────────────────┐  │
│  │ Position    │  Orientation    │  │
│  │ Data        │  Data           │  │
│  ├─────────────┼─────────────────┤  │
│  │ Magnetic    │  Limit          │  │
│  │ Sensors     │  Switches       │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

### **3. State Management:**
```
┌─────────────────────────────────────┐
│         State Management            │
│  ┌─────────────┬─────────────────┐  │
│  │ Module      │  System         │  │
│  │ States      │  States         │  │
│  ├─────────────┼─────────────────┤  │
│  │ User        │  Configuration  │  │
│  │ States      │  States         │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

**States:**
- ✅ **ModuleStates** - Trạng thái 11 modules
- ✅ **SystemStates** - Trạng thái tổng thể hệ thống
- ✅ **UserStates** - Authentication, permissions
- ✅ **ConfigurationStates** - Settings, preferences

### **4. Communication Layer:**
```
┌─────────────────────────────────────┐
│         Communication               │
│  ┌─────────────┬─────────────────┐  │
│  │ WebSocket   │  REST API       │  │
│  │ Client      │  Client         │  │
│  ├─────────────┼─────────────────┤  │
│  │ Real-time   │  Event          │  │
│  │ Updates     │  Handling       │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

**Services:**
- ✅ **WebSocketService** - Real-time communication
- ✅ **APIService** - REST API calls
- ✅ **EventService** - Event handling và routing
- ✅ **NotificationService** - Alerts và notifications

---

## 🎨 **UI/UX Requirements:**

### **1. Design System:**
- ✅ **Color Scheme** - Consistent với brand guidelines
- ✅ **Typography** - Readable fonts cho technical data
- ✅ **Icons** - Intuitive icons cho modules và actions
- ✅ **Spacing** - Consistent spacing system
- ✅ **Components** - Reusable component library

### **2. Responsive Design:**
- ✅ **Desktop** - Full dashboard với tất cả modules
- ✅ **Tablet** - Optimized layout cho tablet
- ✅ **Mobile** - Essential functions cho mobile
- ✅ **Touch Support** - Touch-friendly controls

### **3. Accessibility:**
- ✅ **WCAG 2.1** - Level AA compliance
- ✅ **Keyboard Navigation** - Full keyboard support
- ✅ **Screen Reader** - Screen reader compatibility
- ✅ **High Contrast** - High contrast mode support

---

## 🔧 **Technical Requirements:**

### **1. Framework & Libraries:**
- ✅ **React/Vue.js** - Modern frontend framework
- ✅ **TypeScript** - Type safety
- ✅ **State Management** - Redux/Vuex hoặc Zustand
- ✅ **UI Library** - Material-UI, Ant Design, hoặc custom
- ✅ **Charts** - Real-time charts cho telemetry data

### **2. Performance:**
- ✅ **Lazy Loading** - Module-based lazy loading
- ✅ **Caching** - Client-side caching
- ✅ **Optimization** - Bundle optimization
- ✅ **Real-time** - Efficient real-time updates

### **3. Security:**
- ✅ **Authentication** - JWT-based authentication
- ✅ **Authorization** - Role-based access control
- ✅ **Input Validation** - Client-side validation
- ✅ **XSS Protection** - XSS prevention

---

## 📊 **Module-Specific Requirements:**

### **1. Safety Module UI:**
- ✅ **4 Analog Charts** - Real-time distance sensor charts
- ✅ **4 Digital Indicators** - Proximity sensor status
- ✅ **4 Relay Controls** - Safety relay control buttons
- ✅ **Zone Visualization** - Safety zone visual representation
- ✅ **Alert System** - Safety alerts và notifications

### **2. Travel Motor UI:**
- ✅ **Motor Control Panels** - Individual motor controls
- ✅ **Speed Indicators** - Real-time speed displays
- ✅ **Digital I/O Status** - DI/DO status indicators
- ✅ **PID Tuning Interface** - PID parameter adjustment
- ✅ **Movement Visualization** - Movement status display

### **3. Power Module UI:**
- ✅ **Battery Status** - Battery level, voltage, current
- ✅ **Charger Status** - Charging status và parameters
- ✅ **Power Consumption** - Real-time power consumption
- ✅ **Temperature Monitoring** - Temperature displays
- ✅ **Power History** - Historical power data charts

### **4. Dock & Location UI:**
- ✅ **Position Display** - Current position coordinates
- ✅ **Orientation Data** - IMU data visualization
- ✅ **Magnetic Sensors** - Magnetic sensor status
- ✅ **Limit Switches** - Limit switch status
- ✅ **Location History** - Position history tracking

---

## 🚀 **Implementation Priority:**

### **Phase 1 - Core Modules (5 bắt buộc):**
1. **Master Control Dashboard** - System overview
2. **Safety Module Dashboard** - Safety monitoring
3. **Travel Motor Dashboard** - Motor control
4. **Power Module Dashboard** - Power management
5. **Dock & Location Dashboard** - Position tracking

### **Phase 2 - Optional Modules (3 tùy chọn):**
1. **Lifter Motor Dashboard** - Lifter control
2. **Cargo Door Dashboard** - Door control
3. **Safety Extended Dashboard** - Extended safety features

### **Phase 3 - Plug-and-Play Modules (3 plug-and-play):**
1. **RFID Reader Dashboard** - RFID monitoring
2. **Camera Module Dashboard** - Camera feeds
3. **Environmental Sensor Dashboard** - Environmental data

---

## 📋 **Success Criteria:**

### **Functional:**
- ✅ Tất cả 5 module bắt buộc có dashboard đầy đủ
- ✅ Real-time updates cho tất cả sensors
- ✅ Responsive design trên tất cả devices
- ✅ User authentication và authorization

### **Performance:**
- ✅ Page load time < 2 seconds
- ✅ Real-time update latency < 100ms
- ✅ Smooth animations và transitions
- ✅ Efficient memory usage

### **User Experience:**
- ✅ Intuitive navigation
- ✅ Clear data visualization
- ✅ Consistent design language
- ✅ Accessibility compliance

---

**Changelog v2.0:**
- ✅ Updated theo System Architecture v2.0
- ✅ Added Safety Module với 4 analog + 4 digital sensors
- ✅ Added module-specific dashboard requirements
- ✅ Added implementation phases
- ✅ Added success criteria
