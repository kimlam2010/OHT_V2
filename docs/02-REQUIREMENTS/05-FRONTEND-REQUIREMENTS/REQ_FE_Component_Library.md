# 🧩 REQ_FE_Component_Library - Thư viện Component Frontend

**Mục đích:** Định nghĩa thư viện component chuẩn cho OHT-50 Master Module Dashboard

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
1. **Reusable Components** - Component có thể tái sử dụng cho tất cả modules
2. **Consistent Design** - Thiết kế nhất quán theo design system
3. **Module-Specific Components** - Component chuyên biệt cho từng module
4. **Accessibility** - Tuân thủ tiêu chuẩn accessibility
5. **Performance** - Component tối ưu hiệu suất

---

## 🏗️ **Cấu trúc Component Library:**

### **1. Base Components (Core):**
```
┌─────────────────────────────────────┐
│         Base Components             │
│  ┌─────────────┬─────────────────┐  │
│  │ Button      │  Input          │  │
│  ├─────────────┼─────────────────┤  │
│  │ Card        │  Modal          │  │
│  ├─────────────┼─────────────────┤  │
│  │ Badge       │  Tooltip        │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

### **2. Layout Components:**
```
┌─────────────────────────────────────┐
│         Layout Components           │
│  ┌─────────────┬─────────────────┐  │
│  │ Header      │  Sidebar        │  │
│  ├─────────────┼─────────────────┤  │
│  │ Navigation  │  Content        │  │
│  ├─────────────┼─────────────────┤  │
│  │ StatusBar   │  ActionPanel    │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

### **3. Data Visualization Components:**
```
┌─────────────────────────────────────┐
│      Data Visualization             │
│  ┌─────────────┬─────────────────┐  │
│  │ Gauge       │  Chart          │  │
│  ├─────────────┼─────────────────┤  │
│  │ Progress    │  Indicator      │  │
│  ├─────────────┼─────────────────┤  │
│  │ Table       │  List           │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

### **4. Module-Specific Components:**
```
┌─────────────────────────────────────┐
│      Module Components              │
│  ┌─────────────┬─────────────────┐  │
│  │ Safety      │  Motor          │  │
│  ├─────────────┼─────────────────┤  │
│  │ Power       │  Location       │  │
│  ├─────────────┼─────────────────┤  │
│  │ System      │  Communication  │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

---

## 🧩 **Base Components:**

### **1. Button Component:**
```typescript
interface ButtonProps {
  variant: 'primary' | 'secondary' | 'danger' | 'success' | 'warning';
  size: 'sm' | 'md' | 'lg';
  disabled?: boolean;
  loading?: boolean;
  icon?: ReactNode;
  children: ReactNode;
  onClick?: () => void;
}

// Variants:
// - primary: System actions (blue)
// - secondary: Secondary actions (gray)
// - danger: Critical actions (red)
// - success: Positive actions (green)
// - warning: Caution actions (orange)
```

### **2. Input Component:**
```typescript
interface InputProps {
  type: 'text' | 'number' | 'password' | 'email';
  placeholder?: string;
  value: string | number;
  onChange: (value: string | number) => void;
  disabled?: boolean;
  error?: string;
  label?: string;
  required?: boolean;
}
```

### **3. Card Component:**
```typescript
interface CardProps {
  title?: string;
  subtitle?: string;
  children: ReactNode;
  variant?: 'default' | 'elevated' | 'outlined';
  padding?: 'sm' | 'md' | 'lg';
  onClick?: () => void;
}
```

### **4. Modal Component:**
```typescript
interface ModalProps {
  isOpen: boolean;
  onClose: () => void;
  title: string;
  children: ReactNode;
  size?: 'sm' | 'md' | 'lg' | 'xl';
  showCloseButton?: boolean;
}
```

---

## 📊 **Data Visualization Components:**

### **1. Gauge Component (Safety Module):**
```typescript
interface GaugeProps {
  value: number;
  min: number;
  max: number;
  unit: string;
  thresholds: {
    warning: number;
    danger: number;
  };
  size?: 'sm' | 'md' | 'lg';
  showValue?: boolean;
  showThresholds?: boolean;
}

// Usage for Safety Module:
<Gauge 
  value={2500} 
  min={0} 
  max={5000} 
  unit="mm"
  thresholds={{ warning: 1000, danger: 500 }}
/>
```

### **2. Chart Component:**
```typescript
interface ChartProps {
  type: 'line' | 'bar' | 'area' | 'pie';
  data: Array<{ x: string | number; y: number }>;
  xAxis?: string;
  yAxis?: string;
  height?: number;
  width?: number;
  color?: string;
  showGrid?: boolean;
  showLegend?: boolean;
}
```

### **3. Progress Component:**
```typescript
interface ProgressProps {
  value: number;
  max: number;
  variant?: 'default' | 'success' | 'warning' | 'danger';
  showLabel?: boolean;
  size?: 'sm' | 'md' | 'lg';
  animated?: boolean;
}

// Usage for Battery Level:
<Progress 
  value={80} 
  max={100} 
  variant="success" 
  showLabel 
/>
```

### **4. Indicator Component:**
```typescript
interface IndicatorProps {
  status: 'online' | 'offline' | 'warning' | 'error';
  label?: string;
  size?: 'sm' | 'md' | 'lg';
  showPulse?: boolean;
}

// Usage for Module Status:
<Indicator 
  status="online" 
  label="Safety Module" 
  showPulse 
/>
```

---

## 🎛️ **Module-Specific Components:**

### **1. Safety Module Components:**

#### **1.1 DistanceSensorGauge:**
```typescript
interface DistanceSensorGaugeProps {
  zone: 1 | 2 | 3 | 4;
  distance: number;
  threshold: number;
  unit: 'mm' | 'cm' | 'm';
  onThresholdExceeded?: () => void;
}
```

#### **1.2 ProximitySensorIndicator:**
```typescript
interface ProximitySensorIndicatorProps {
  sensorId: 1 | 2 | 3 | 4;
  isActive: boolean;
  distance?: number;
  onActivated?: () => void;
}
```

#### **1.3 SafetyRelayControl:**
```typescript
interface SafetyRelayControlProps {
  relayId: 1 | 2 | 3 | 4;
  isActive: boolean;
  function: 'emergency_brake' | 'warning_lights' | 'alarm' | 'e_stop';
  onToggle: (active: boolean) => void;
  requireConfirmation?: boolean;
}
```

### **2. Travel Motor Components:**

#### **2.1 MotorControlPanel:**
```typescript
interface MotorControlPanelProps {
  motorId: 1 | 2;
  speed: number;
  direction: 'forward' | 'reverse' | 'stop';
  isEnabled: boolean;
  isBrakeApplied: boolean;
  current: number;
  onSpeedChange: (speed: number) => void;
  onDirectionChange: (direction: string) => void;
  onEnableToggle: (enabled: boolean) => void;
  onBrakeToggle: (applied: boolean) => void;
}
```

#### **2.2 DigitalIOStatus:**
```typescript
interface DigitalIOStatusProps {
  type: 'input' | 'output';
  channels: Array<{
    id: number;
    status: boolean;
    label?: string;
  }>;
  onOutputToggle?: (channelId: number, status: boolean) => void;
}
```

### **3. Power Module Components:**

#### **3.1 BatteryStatus:**
```typescript
interface BatteryStatusProps {
  level: number;
  voltage: number;
  current: number;
  temperature: number;
  timeRemaining?: number;
  isCharging: boolean;
  showDetails?: boolean;
}
```

#### **3.2 ChargerStatus:**
```typescript
interface ChargerStatusProps {
  isConnected: boolean;
  isCharging: boolean;
  mode: 'fast' | 'normal' | 'trickle';
  inputVoltage: number;
  outputVoltage: number;
  efficiency: number;
}
```

### **4. Dock & Location Components:**

#### **4.1 PositionDisplay:**
```typescript
interface PositionDisplayProps {
  x: number;
  y: number;
  z: number;
  roll: number;
  pitch: number;
  yaw: number;
  unit: 'mm' | 'cm' | 'm';
  showOrientation?: boolean;
}
```

#### **4.2 MagneticSensorStatus:**
```typescript
interface MagneticSensorStatusProps {
  sensors: Array<{
    id: number;
    isDetected: boolean;
    strength?: number;
  }>;
  showStrength?: boolean;
}
```

---

## 🎨 **Component Styling:**

### **1. Design Tokens:**
```css
:root {
  /* Colors */
  --primary: #2563EB;
  --secondary: #6B7280;
  --success: #059669;
  --warning: #D97706;
  --danger: #DC2626;
  
  /* Spacing */
  --spacing-xs: 4px;
  --spacing-sm: 8px;
  --spacing-md: 16px;
  --spacing-lg: 24px;
  --spacing-xl: 32px;
  
  /* Border Radius */
  --radius-sm: 4px;
  --radius-md: 8px;
  --radius-lg: 12px;
  
  /* Shadows */
  --shadow-sm: 0 1px 3px rgba(0,0,0,0.1);
  --shadow-md: 0 4px 6px rgba(0,0,0,0.1);
  --shadow-lg: 0 10px 15px rgba(0,0,0,0.1);
}
```

### **2. Component Variants:**
```css
/* Button Variants */
.btn-primary { background: var(--primary); }
.btn-secondary { background: var(--secondary); }
.btn-danger { background: var(--danger); }
.btn-success { background: var(--success); }
.btn-warning { background: var(--warning); }

/* Card Variants */
.card-default { box-shadow: var(--shadow-sm); }
.card-elevated { box-shadow: var(--shadow-md); }
.card-outlined { border: 1px solid var(--border); }
```

---

## ♿ **Accessibility Features:**

### **1. ARIA Labels:**
```typescript
// All interactive components must have ARIA labels
<Button aria-label="Emergency Stop" onClick={handleEStop}>
  E-Stop
</Button>

<Gauge 
  aria-label={`Distance sensor zone ${zone}: ${distance}${unit}`}
  value={distance}
/>
```

### **2. Keyboard Navigation:**
```typescript
// All components must support keyboard navigation
const handleKeyDown = (event: KeyboardEvent) => {
  if (event.key === 'Enter' || event.key === ' ') {
    event.preventDefault();
    onClick();
  }
};
```

### **3. Focus Management:**
```typescript
// Proper focus management for modals and complex components
useEffect(() => {
  if (isOpen) {
    const firstFocusable = modalRef.current?.querySelector(
      'button, [href], input, select, textarea, [tabindex]:not([tabindex="-1"])'
    );
    firstFocusable?.focus();
  }
}, [isOpen]);
```

---

## 📱 **Responsive Behavior:**

### **1. Mobile-First Approach:**
```css
/* Base styles for mobile */
.component {
  width: 100%;
  padding: var(--spacing-sm);
}

/* Tablet styles */
@media (min-width: 768px) {
  .component {
    width: 50%;
    padding: var(--spacing-md);
  }
}

/* Desktop styles */
@media (min-width: 1200px) {
  .component {
    width: 33.33%;
    padding: var(--spacing-lg);
  }
}
```

### **2. Touch-Friendly Sizing:**
```css
/* Minimum touch target size */
.btn, .input, .indicator {
  min-height: 44px;
  min-width: 44px;
}
```

---

## 🚀 **Performance Optimization:**

### **1. Lazy Loading:**
```typescript
// Lazy load heavy components
const HeavyChart = lazy(() => import('./HeavyChart'));

// Use Suspense for loading states
<Suspense fallback={<ChartSkeleton />}>
  <HeavyChart data={chartData} />
</Suspense>
```

### **2. Memoization:**
```typescript
// Memoize expensive components
const ExpensiveComponent = memo(({ data }) => {
  return <div>{/* Expensive rendering */}</div>;
});
```

### **3. Virtual Scrolling:**
```typescript
// For large lists/tables
import { FixedSizeList as List } from 'react-window';

<List
  height={400}
  itemCount={1000}
  itemSize={35}
  itemData={data}
>
  {Row}
</List>
```

---

## 📋 **Component Documentation:**

### **1. Storybook Integration:**
```typescript
// Component stories for documentation
export default {
  title: 'Components/Safety/DistanceSensorGauge',
  component: DistanceSensorGauge,
  parameters: {
    docs: {
      description: {
        component: 'Gauge component for displaying distance sensor readings'
      }
    }
  }
};

export const Default = () => (
  <DistanceSensorGauge 
    zone={1} 
    distance={2500} 
    threshold={1000} 
    unit="mm" 
  />
);
```

### **2. Props Documentation:**
```typescript
// JSDoc comments for all props
interface ComponentProps {
  /** The current value to display */
  value: number;
  /** Minimum value for the range */
  min: number;
  /** Maximum value for the range */
  max: number;
  /** Unit of measurement */
  unit: string;
  /** Whether the component is disabled */
  disabled?: boolean;
}
```

---

## 🎯 **Success Criteria:**

### **1. Reusability:**
- ✅ 80% components có thể tái sử dụng
- ✅ Consistent API across components
- ✅ Proper TypeScript definitions

### **2. Performance:**
- ✅ Component render time < 16ms
- ✅ Bundle size < 100KB for component library
- ✅ Memory usage < 50MB

### **3. Accessibility:**
- ✅ WCAG 2.1 Level AA compliance
- ✅ 100% keyboard navigation support
- ✅ Screen reader compatibility

### **4. Documentation:**
- ✅ 100% components có documentation
- ✅ Interactive examples
- ✅ Props validation

---

**Changelog v2.0:**
- ✅ Updated theo System Architecture v2.0
- ✅ Added Safety Module components với 4 analog + 4 digital sensors
- ✅ Added comprehensive component library structure
- ✅ Added accessibility features
- ✅ Added performance optimization guidelines
- ✅ Added responsive design patterns
