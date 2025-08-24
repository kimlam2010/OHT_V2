# 🎨 REQ_FE_UI_UX_Specifications - Yêu cầu UI/UX Frontend

**Mục đích:** Định nghĩa chi tiết yêu cầu UI/UX cho OHT-50 Master Module Dashboard

---

## 📋 **Thông tin tài liệu:**
- **Phiên bản:** v2.0
- **Ngày tạo:** 2025-01-28
- **Người tạo:** PM Team
- **Trạng thái:** Implementation Guide
- **Cập nhật theo:** System Architecture v2.0

---

## 🎯 **Mục tiêu UI/UX:**

### **Yêu cầu chính:**
1. **Intuitive Navigation** - Dễ dàng chuyển đổi giữa 11 modules
2. **Real-time Visualization** - Hiển thị dữ liệu thời gian thực rõ ràng
3. **Safety-First Design** - Ưu tiên hiển thị thông tin an toàn
4. **Responsive Layout** - Tối ưu cho tất cả thiết bị
5. **Accessibility** - Tuân thủ tiêu chuẩn accessibility

---

## 🎨 **Design System:**

### **1. Color Palette:**
```
Primary Colors:
- Primary Blue: #2563EB (System actions)
- Success Green: #059669 (Normal status)
- Warning Orange: #D97706 (Warning alerts)
- Danger Red: #DC2626 (Error/Safety alerts)
- Neutral Gray: #6B7280 (Secondary text)

Background Colors:
- Primary BG: #FFFFFF (Main background)
- Secondary BG: #F9FAFB (Card background)
- Dark BG: #111827 (Dark mode)
- Overlay BG: #00000080 (Modal overlay)
```

### **2. Typography:**
```
Headings:
- H1: 32px, Bold, #111827
- H2: 24px, SemiBold, #111827
- H3: 20px, Medium, #374151
- H4: 16px, Medium, #374151

Body Text:
- Large: 18px, Regular, #374151
- Medium: 16px, Regular, #6B7280
- Small: 14px, Regular, #9CA3AF
- Caption: 12px, Regular, #9CA3AF
```

### **3. Spacing System:**
```
Spacing Scale:
- XS: 4px
- SM: 8px
- MD: 16px
- LG: 24px
- XL: 32px
- 2XL: 48px
- 3XL: 64px
```

---

## 📱 **Layout Specifications:**

### **1. Desktop Layout (≥1200px):**
```
┌─────────────────────────────────────────────────────────┐
│ Header Bar (64px)                                       │
├─────────────┬───────────────────────────────────────────┤
│ Sidebar     │ Main Content Area                         │
│ (280px)     │ (Flexible)                                │
│             │ ┌─────────────────────────────────────┐   │
│             │ │ Module Dashboard                    │   │
│             │ └─────────────────────────────────────┘   │
│             │ ┌─────────────────────────────────────┐   │
│             │ │ Status Bar (48px)                   │   │
│             │ └─────────────────────────────────────┘   │
└─────────────┴───────────────────────────────────────────┘
```

### **2. Tablet Layout (768px-1199px):**
```
┌─────────────────────────────────────────────────────────┐
│ Header Bar (56px)                                       │
├─────────────────────────────────────────────────────────┤
│ Navigation Tabs                                         │
├─────────────────────────────────────────────────────────┤
│ Main Content Area                                       │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ Module Dashboard                                    │ │
│ └─────────────────────────────────────────────────────┘ │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ Status Bar (40px)                                   │ │
│ └─────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

### **3. Mobile Layout (≤767px):**
```
┌─────────────────────────────────────────────────────────┐
│ Header Bar (48px)                                       │
├─────────────────────────────────────────────────────────┤
│ Module Selector (Dropdown)                              │
├─────────────────────────────────────────────────────────┤
│ Main Content Area                                       │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ Module Dashboard                                    │ │
│ └─────────────────────────────────────────────────────┘ │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ Status Bar (32px)                                   │ │
│ └─────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

---

## 🧩 **Component Specifications:**

### **1. Header Bar:**
```
┌─────────────────────────────────────────────────────────┐
│ Logo │ System Status │ User Info │ Notifications │ Menu │
└─────────────────────────────────────────────────────────┘

Components:
- Logo: 32x32px, left-aligned
- System Status: Green/Orange/Red indicator
- User Info: Avatar + username
- Notifications: Badge with count
- Menu: Hamburger menu (mobile)
```

### **2. Sidebar Navigation:**
```
┌─────────────────────────────────────────────────────────┐
│ Module Tree                                             │
│ ├─ Master Control (0x01)                                │
│ ├─ Power Module (0x02)                                  │
│ ├─ Safety Module (0x03)                                 │
│ ├─ Travel Motor (0x04)                                  │
│ ├─ Dock & Location (0x05)                               │
│ ├─ Lifter Motor (0x06)                                  │
│ ├─ Cargo Door (0x07)                                    │
│ ├─ Safety Extended (0x08)                               │
│ ├─ RFID Reader (0x09)                                   │
│ ├─ Camera Module (0x0A)                                 │
│ └─ Environmental (0x0B)                                 │
└─────────────────────────────────────────────────────────┘
```

### **3. Module Dashboard Cards:**
```
┌─────────────────────────────────────────────────────────┐
│ Module Name                    │ Status │ Actions │
├─────────────────────────────────────────────────────────┤
│ Content Area (Flexible)                                 │
│ ┌─────────────┬─────────────┬─────────────┬───────────┐ │
│ │ Metric 1    │ Metric 2    │ Metric 3    │ Metric 4  │ │
│ └─────────────┴─────────────┴─────────────┴───────────┘ │
└─────────────────────────────────────────────────────────┘

Card Specifications:
- Border radius: 8px
- Shadow: 0 1px 3px rgba(0,0,0,0.1)
- Padding: 16px
- Margin: 8px
```

---

## 📊 **Module-Specific UI Components:**

### **1. Safety Module Components:**

#### **1.1 Distance Sensor Charts:**
```
┌─────────────────────────────────────────────────────────┐
│ Zone 1 Distance: 2.5m                    │ [Gauge]     │
│ Zone 2 Distance: 1.2m                    │ [Gauge]     │
│ Zone 3 Distance: 0.8m                    │ [Gauge]     │
│ Zone 4 Distance: 0.3m                    │ [Gauge]     │
└─────────────────────────────────────────────────────────┘

Gauge Specifications:
- Type: Circular gauge
- Range: 0-5m
- Colors: Green (safe) → Yellow (warning) → Red (danger)
- Size: 80x80px
```

#### **1.2 Proximity Sensor Indicators:**
```
┌─────────────────────────────────────────────────────────┐
│ Proximity Sensors:                                      │
│ [●] [●] [○] [●]  (Active/Inactive)                      │
│ DI1  DI2  DI3  DI4                                      │
└─────────────────────────────────────────────────────────┘

Indicator Specifications:
- Active: Green circle (●)
- Inactive: Gray circle (○)
- Size: 16x16px
- Spacing: 8px
```

#### **1.3 Safety Relay Controls:**
```
┌─────────────────────────────────────────────────────────┐
│ Safety Relays:                                          │
│ [Emergency Brake] [Warning Lights] [Alarm] [E-Stop]     │
│    ON/OFF         ON/OFF         ON/OFF    ON/OFF       │
└─────────────────────────────────────────────────────────┘

Button Specifications:
- Type: Toggle buttons
- Size: 120x40px
- Colors: Red for safety functions
- Confirmation required for critical actions
```

### **2. Travel Motor Components:**

#### **2.1 Motor Control Panels:**
```
┌─────────────────────────────────────────────────────────┐
│ Motor 1 Control:                                        │
│ Speed: [████████░░] 80%    Direction: [Forward/Reverse] │
│ Enable: [ON]  Brake: [OFF]  Current: 2.5A               │
│                                                         │
│ Motor 2 Control:                                        │
│ Speed: [██████░░░░] 60%    Direction: [Forward/Reverse] │
│ Enable: [ON]  Brake: [OFF]  Current: 2.1A               │
└─────────────────────────────────────────────────────────┘

Control Specifications:
- Speed slider: 0-100% range
- Direction buttons: Forward/Reverse/Stop
- Enable/Brake toggles
- Current display: Real-time amperage
```

#### **2.2 Digital I/O Status:**
```
┌─────────────────────────────────────────────────────────┐
│ Digital Inputs:  [●] [○] [●] [○] [●] [○] [●] [○]       │
│                  DI1 DI2 DI3 DI4 DI5 DI6 DI7 DI8       │
│                                                         │
│ Digital Outputs: [●] [○] [●] [○] [●] [○] [●] [○]       │
│                   DO1 DO2 DO3 DO4 DO5 DO6 DO7 DO8      │
└─────────────────────────────────────────────────────────┘
```

### **3. Power Module Components:**

#### **3.1 Battery Status:**
```
┌─────────────────────────────────────────────────────────┐
│ Battery Status:                                         │
│ Level: [████████░░] 80%    Voltage: 48.2V              │
│ Current: 2.1A    Temperature: 25°C    Time: 4h 30m     │
└─────────────────────────────────────────────────────────┘

Battery Specifications:
- Level bar: Green/Yellow/Red based on percentage
- Voltage display: Real-time voltage
- Current display: Charge/discharge current
- Temperature: With warning thresholds
- Time remaining: Estimated runtime
```

#### **3.2 Charger Status:**
```
┌─────────────────────────────────────────────────────────┐
│ Charger Status:                                         │
│ Status: [Charging]    Mode: [Fast Charge]              │
│ Input: 220V AC    Output: 48V DC    Efficiency: 95%    │
└─────────────────────────────────────────────────────────┘
```

### **4. Dock & Location Components:**

#### **4.1 Position Display:**
```
┌─────────────────────────────────────────────────────────┐
│ Current Position:                                       │
│ X: 1250.5mm    Y: 850.2mm    Z: 0.0mm                  │
│ Orientation: Roll: 0.5°  Pitch: -1.2°  Yaw: 45.8°      │
└─────────────────────────────────────────────────────────┘
```

#### **4.2 Magnetic Sensor Status:**
```
┌─────────────────────────────────────────────────────────┐
│ Magnetic Sensors:                                       │
│ [●] [●] [○] [●]  (Detected/Not Detected)               │
│ Mag1 Mag2 Mag3 Mag4                                     │
└─────────────────────────────────────────────────────────┘
```

---

## 🚨 **Alert & Notification System:**

### **1. Alert Levels:**
```
Critical (Red): Immediate action required
- E-Stop activated
- Safety zone violation
- System failure

Warning (Orange): Attention required
- Low battery
- High temperature
- Communication loss

Info (Blue): Information only
- System status changes
- Configuration updates
- Normal operations
```

### **2. Notification Components:**
```
┌─────────────────────────────────────────────────────────┐
│ [🔴] E-Stop activated - Safety Module 0x03              │
│ [🟠] Low battery warning - 15% remaining               │
│ [🔵] System ready - All modules online                  │
└─────────────────────────────────────────────────────────┘

Notification Specifications:
- Auto-dismiss: Info (5s), Warning (10s), Critical (manual)
- Sound alerts: Critical and Warning only
- Persistent: Critical alerts until resolved
```

---

## ♿ **Accessibility Requirements:**

### **1. WCAG 2.1 Level AA Compliance:**
- **Color Contrast:** Minimum 4.5:1 ratio
- **Keyboard Navigation:** Full keyboard support
- **Screen Reader:** ARIA labels và semantic HTML
- **Focus Indicators:** Visible focus indicators
- **Text Scaling:** Support up to 200% zoom

### **2. Keyboard Shortcuts:**
```
Navigation:
- Tab: Next element
- Shift+Tab: Previous element
- Enter/Space: Activate element
- Arrow keys: Navigate within components

Module Selection:
- Ctrl+1: Master Control
- Ctrl+2: Power Module
- Ctrl+3: Safety Module
- Ctrl+4: Travel Motor
- Ctrl+5: Dock & Location

Emergency:
- E: Emergency Stop
- S: System Status
- H: Help
```

---

## 📱 **Responsive Behavior:**

### **1. Breakpoint Strategy:**
```
Desktop: ≥1200px - Full sidebar, all features
Tablet: 768px-1199px - Collapsible sidebar, optimized layout
Mobile: ≤767px - Stacked layout, essential features only
```

### **2. Touch Interactions:**
```
Touch Targets:
- Minimum size: 44x44px
- Spacing: 8px between touch targets
- Feedback: Visual feedback on touch

Gestures:
- Swipe: Navigate between modules
- Pinch: Zoom charts và maps
- Long press: Context menus
```

---

## 🎯 **Success Metrics:**

### **1. Usability Metrics:**
- **Task Completion Rate:** >95%
- **Error Rate:** <2%
- **Time to Complete Tasks:** <30 seconds
- **User Satisfaction Score:** >4.5/5

### **2. Performance Metrics:**
- **Page Load Time:** <2 seconds
- **Interaction Response:** <100ms
- **Animation Frame Rate:** 60fps
- **Memory Usage:** <100MB

### **3. Accessibility Metrics:**
- **WCAG Compliance:** Level AA
- **Screen Reader Compatibility:** 100%
- **Keyboard Navigation:** 100%
- **Color Blind Friendly:** 100%

---

**Changelog v2.0:**
- ✅ Updated theo System Architecture v2.0
- ✅ Added Safety Module UI components với 4 analog + 4 digital sensors
- ✅ Added detailed component specifications
- ✅ Added accessibility requirements
- ✅ Added responsive design guidelines
- ✅ Added success metrics
