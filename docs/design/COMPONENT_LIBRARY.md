# 🧩 OHT-50 Component Library

**Phiên bản:** v1.0  
**Ngày cập nhật:** 2025-01-27  
**Team:** Frontend  
**Status:** ✅ COMPLETE

---

## 📋 **Tổng quan**

Component Library cho OHT-50 Dashboard, cung cấp các reusable components với consistent design và behavior.

### **🎯 Mục tiêu:**
- Reusable components cho toàn bộ dashboard
- Consistent API và behavior
- Accessibility compliance
- TypeScript support
- Comprehensive documentation

---

## 🎨 **Core Components**

### **1. Button Component**

```jsx
import { Button } from '@/components/ui/button';

// Variants
<Button variant="primary">Primary Button</Button>
<Button variant="secondary">Secondary Button</Button>
<Button variant="ghost">Ghost Button</Button>
<Button variant="destructive">Delete</Button>

// Sizes
<Button size="sm">Small</Button>
<Button size="md">Medium</Button>
<Button size="lg">Large</Button>

// States
<Button disabled>Disabled</Button>
<Button loading>Loading...</Button>
```

**Props:**
- `variant`: 'primary' | 'secondary' | 'ghost' | 'destructive'
- `size`: 'sm' | 'md' | 'lg'
- `disabled`: boolean
- `loading`: boolean
- `onClick`: function

### **2. Card Component**

```jsx
import { Card, CardHeader, CardContent, CardFooter } from '@/components/ui/card';

<Card>
  <CardHeader>
    <CardTitle>System Status</CardTitle>
    <CardDescription>Current system information</CardDescription>
  </CardHeader>
  <CardContent>
    <StatusBadge state="idle">Idle</StatusBadge>
  </CardContent>
  <CardFooter>
    <Button>View Details</Button>
  </CardFooter>
</Card>
```

**Props:**
- `elevated`: boolean (adds shadow)
- `interactive`: boolean (hover effects)

### **3. Status Badge Component**

```jsx
import { StatusBadge } from '@/components/ui/status-badge';

// System states
<StatusBadge state="idle">Idle</StatusBadge>
<StatusBadge state="move">Move</StatusBadge>
<StatusBadge state="dock">Dock</StatusBadge>
<StatusBadge state="fault">Fault</StatusBadge>
<StatusBadge state="estop">E-Stop</StatusBadge>

// Custom states
<StatusBadge variant="success">Success</StatusBadge>
<StatusBadge variant="warning">Warning</StatusBadge>
<StatusBadge variant="error">Error</StatusBadge>
```

**Props:**
- `state`: 'idle' | 'move' | 'dock' | 'fault' | 'estop'
- `variant`: 'success' | 'warning' | 'error' | 'info'

### **4. Form Components**

```jsx
import { 
  FormField, 
  FormLabel, 
  FormInput, 
  FormSelect, 
  FormMessage 
} from '@/components/ui/form';

<FormField>
  <FormLabel>Speed Limit (m/s)</FormLabel>
  <FormInput 
    type="number" 
    min="0" 
    max="100" 
    placeholder="Enter speed limit"
  />
  <FormMessage>Speed must be between 0 and 100 m/s</FormMessage>
</FormField>

<FormField>
  <FormLabel>Operation Mode</FormLabel>
  <FormSelect>
    <option value="auto">Automatic</option>
    <option value="manual">Manual</option>
    <option value="test">Test Mode</option>
  </FormSelect>
</FormField>
```

**Props:**
- `error`: boolean
- `disabled`: boolean
- `required`: boolean

### **5. Navigation Components**

```jsx
import { Sidebar, SidebarItem } from '@/components/ui/sidebar';

<Sidebar>
  <SidebarItem 
    icon="home" 
    href="/overview" 
    active={true}
  >
    Tổng quan
  </SidebarItem>
  <SidebarItem 
    icon="chart" 
    href="/vax-charts"
  >
    Đồ thị v‑a‑x
  </SidebarItem>
  <SidebarItem 
    icon="file-text" 
    href="/logs"
  >
    Nhật ký
  </SidebarItem>
</Sidebar>
```

**Props:**
- `icon`: string (Lucide icon name)
- `href`: string
- `active`: boolean
- `disabled`: boolean

### **6. Data Display Components**

```jsx
import { DataTable, DataTableColumn } from '@/components/ui/data-table';

const columns: DataTableColumn[] = [
  { key: 'timestamp', label: 'Thời gian', sortable: true },
  { key: 'state', label: 'Trạng thái', render: (value) => <StatusBadge state={value}>{value}</StatusBadge> },
  { key: 'speed', label: 'Vận tốc (m/s)', sortable: true },
  { key: 'position', label: 'Vị trí (m)', sortable: true }
];

<DataTable 
  columns={columns} 
  data={telemetryData}
  pagination={true}
  search={true}
/>
```

**Props:**
- `columns`: DataTableColumn[]
- `data`: any[]
- `pagination`: boolean
- `search`: boolean
- `loading`: boolean

### **7. Feedback Components**

```jsx
import { Alert, Toast, Modal } from '@/components/ui/feedback';

// Alert
<Alert variant="success" title="Operation Successful">
  The system has been started successfully.
</Alert>

<Alert variant="warning" title="Warning">
  Speed limit is approaching maximum.
</Alert>

<Alert variant="error" title="Error">
  Failed to connect to the system.
</Alert>

// Toast
<Toast 
  variant="success" 
  title="Success"
  message="Operation completed"
  duration={3000}
/>

// Modal
<Modal 
  open={showModal} 
  onClose={() => setShowModal(false)}
  title="Confirm Operation"
>
  <p>Are you sure you want to start the system?</p>
  <div className="flex gap-2">
    <Button variant="primary">Confirm</Button>
    <Button variant="ghost">Cancel</Button>
  </div>
</Modal>
```

---

## 📊 **Chart Components**

### **1. VAX Chart Component**

```jsx
import { VAXChart } from '@/components/charts/vax-chart';

<VAXChart
  data={chartData}
  channels={['v', 'a', 'x']}
  timeRange={30}
  live={true}
  onChannelToggle={(channel) => console.log(channel)}
  onTimeRangeChange={(range) => console.log(range)}
/>
```

**Props:**
- `data`: ChartData[]
- `channels`: ('v' | 'a' | 'x')[]
- `timeRange`: number (seconds)
- `live`: boolean
- `onChannelToggle`: function
- `onTimeRangeChange`: function

### **2. Real-time Chart Component**

```jsx
import { RealTimeChart } from '@/components/charts/realtime-chart';

<RealTimeChart
  data={realTimeData}
  maxPoints={100}
  updateInterval={100}
  height={400}
  showGrid={true}
  showLegend={true}
/>
```

**Props:**
- `data`: RealTimeData[]
- `maxPoints`: number
- `updateInterval`: number (ms)
- `height`: number
- `showGrid`: boolean
- `showLegend`: boolean

---

## 🔧 **Layout Components**

### **1. Dashboard Layout**

```jsx
import { DashboardLayout } from '@/components/layout/dashboard-layout';

<DashboardLayout>
  <DashboardLayout.Sidebar>
    <Navigation />
  </DashboardLayout.Sidebar>
  <DashboardLayout.Header>
    <Header />
  </DashboardLayout.Header>
  <DashboardLayout.Content>
    <PageContent />
  </DashboardLayout.Content>
</DashboardLayout>
```

### **2. Grid Layout**

```jsx
import { Grid, GridItem } from '@/components/layout/grid';

<Grid columns={12} gap={4}>
  <GridItem span={6}>
    <Card>System Status</Card>
  </GridItem>
  <GridItem span={6}>
    <Card>Performance Metrics</Card>
  </GridItem>
  <GridItem span={12}>
    <Card>VAX Chart</Card>
  </GridItem>
</Grid>
```

**Props:**
- `columns`: number
- `gap`: number
- `span`: number (for GridItem)

---

## 🎯 **Specialized Components**

### **1. System Control Panel**

```jsx
import { SystemControlPanel } from '@/components/specialized/system-control';

<SystemControlPanel
  systemState={systemState}
  onStart={() => handleStart()}
  onStop={() => handleStop()}
  onEmergencyStop={() => handleEmergencyStop()}
  disabled={!isConnected}
/>
```

### **2. Telemetry Display**

```jsx
import { TelemetryDisplay } from '@/components/specialized/telemetry-display';

<TelemetryDisplay
  telemetry={telemetryData}
  showVelocity={true}
  showAcceleration={true}
  showPosition={true}
  units="metric"
/>
```

### **3. Safety Status Panel**

```jsx
import { SafetyStatusPanel } from '@/components/specialized/safety-status';

<SafetyStatusPanel
  eStopStatus={eStopStatus}
  safetyZones={safetyZones}
  interlockStatus={interlockStatus}
  onAcknowledge={() => handleAcknowledge()}
/>
```

---

## ♿ **Accessibility Features**

### **Keyboard Navigation**

Tất cả components hỗ trợ keyboard navigation:

```jsx
// Focus management
<Button onKeyDown={(e) => {
  if (e.key === 'Enter' || e.key === ' ') {
    e.preventDefault();
    handleClick();
  }
}}>
  Click me
</Button>

// Skip links
<a href="#main-content" className="skip-link">
  Skip to main content
</a>
```

### **Screen Reader Support**

```jsx
// ARIA labels
<button aria-label="Close dialog" aria-pressed="false">
  <i data-lucide="x"></i>
</button>

// Live regions
<div role="status" aria-live="polite">
  {loading ? 'Loading...' : 'Data loaded'}
</div>

// Descriptions
<StatusBadge 
  state="idle" 
  aria-describedby="idle-description"
>
  Idle
</StatusBadge>
<div id="idle-description" className="sr-only">
  System is ready and waiting for commands
</div>
```

---

## 📱 **Responsive Behavior**

### **Mobile Adaptations**

```jsx
// Responsive grid
<Grid columns={{ mobile: 1, tablet: 2, desktop: 4 }}>
  <GridItem>Card 1</GridItem>
  <GridItem>Card 2</GridItem>
  <GridItem>Card 3</GridItem>
  <GridItem>Card 4</GridItem>
</Grid>

// Responsive navigation
<Sidebar 
  mobileCollapsible={true}
  mobileBreakpoint={768}
>
  {/* Navigation items */}
</Sidebar>
```

### **Touch Interactions**

```jsx
// Touch-friendly buttons
<Button 
  minTouchTarget={44} // 44px minimum touch target
  touchFeedback={true}
>
  Touch me
</Button>

// Swipe gestures
<SwipeableCard onSwipeLeft={() => handleSwipe()}>
  <CardContent>Swipe me</CardContent>
</SwipeableCard>
```

---

## 🧪 **Testing Guidelines**

### **Component Testing**

```jsx
// Unit tests
describe('Button Component', () => {
  it('renders with correct variant', () => {
    render(<Button variant="primary">Click me</Button>);
    expect(screen.getByRole('button')).toHaveClass('btn-primary');
  });

  it('handles click events', () => {
    const handleClick = jest.fn();
    render(<Button onClick={handleClick}>Click me</Button>);
    fireEvent.click(screen.getByRole('button'));
    expect(handleClick).toHaveBeenCalled();
  });
});
```

### **Accessibility Testing**

```jsx
// Accessibility tests
describe('Button Accessibility', () => {
  it('has accessible name', () => {
    render(<Button>Click me</Button>);
    expect(screen.getByRole('button')).toHaveAccessibleName('Click me');
  });

  it('supports keyboard navigation', () => {
    render(<Button>Click me</Button>);
    screen.getByRole('button').focus();
    fireEvent.keyDown(screen.getByRole('button'), { key: 'Enter' });
    // Test keyboard interaction
  });
});
```

---

## 📚 **Usage Examples**

### **Complete Dashboard Page**

```jsx
import { DashboardLayout } from '@/components/layout/dashboard-layout';
import { SystemControlPanel } from '@/components/specialized/system-control';
import { VAXChart } from '@/components/charts/vax-chart';
import { TelemetryDisplay } from '@/components/specialized/telemetry-display';

function DashboardPage() {
  return (
    <DashboardLayout>
      <DashboardLayout.Sidebar>
        <Navigation />
      </DashboardLayout.Sidebar>
      
      <DashboardLayout.Content>
        <div className="space-y-6">
          {/* System Control */}
          <Card>
            <CardHeader>
              <CardTitle>System Control</CardTitle>
            </CardHeader>
            <CardContent>
              <SystemControlPanel
                systemState={systemState}
                onStart={handleStart}
                onStop={handleStop}
                onEmergencyStop={handleEmergencyStop}
              />
            </CardContent>
          </Card>

          {/* Telemetry */}
          <Grid columns={12} gap={4}>
            <GridItem span={6}>
              <Card>
                <CardHeader>
                  <CardTitle>Real-time Telemetry</CardTitle>
                </CardHeader>
                <CardContent>
                  <TelemetryDisplay telemetry={telemetryData} />
                </CardContent>
              </Card>
            </GridItem>
            
            <GridItem span={6}>
              <Card>
                <CardHeader>
                  <CardTitle>System Status</CardTitle>
                </CardHeader>
                <CardContent>
                  <StatusBadge state={systemState}>
                    {systemState}
                  </StatusBadge>
                </CardContent>
              </Card>
            </GridItem>
          </Grid>

          {/* VAX Chart */}
          <Card>
            <CardHeader>
              <CardTitle>VAX Charts</CardTitle>
            </CardHeader>
            <CardContent>
              <VAXChart
                data={chartData}
                channels={['v', 'a', 'x']}
                timeRange={30}
                live={true}
              />
            </CardContent>
          </Card>
        </div>
      </DashboardLayout.Content>
    </DashboardLayout>
  );
}
```

---

## 🔧 **Development Guidelines**

### **Creating New Components**

1. **Component Structure:**
```jsx
// components/ui/my-component.tsx
import React from 'react';
import { cn } from '@/lib/utils';

interface MyComponentProps {
  variant?: 'default' | 'primary';
  size?: 'sm' | 'md' | 'lg';
  children: React.ReactNode;
  className?: string;
}

export const MyComponent: React.FC<MyComponentProps> = ({
  variant = 'default',
  size = 'md',
  children,
  className,
  ...props
}) => {
  return (
    <div
      className={cn(
        'base-styles',
        variant === 'primary' && 'primary-styles',
        size === 'sm' && 'sm-styles',
        size === 'lg' && 'lg-styles',
        className
      )}
      {...props}
    >
      {children}
    </div>
  );
};
```

2. **TypeScript Types:**
```typescript
// types/components.ts
export interface BaseComponentProps {
  className?: string;
  disabled?: boolean;
  'aria-label'?: string;
}

export interface ButtonProps extends BaseComponentProps {
  variant: 'primary' | 'secondary' | 'ghost' | 'destructive';
  size: 'sm' | 'md' | 'lg';
  loading?: boolean;
  onClick?: () => void;
}
```

3. **Testing:**
```jsx
// __tests__/components/my-component.test.tsx
import { render, screen } from '@testing-library/react';
import { MyComponent } from '@/components/ui/my-component';

describe('MyComponent', () => {
  it('renders correctly', () => {
    render(<MyComponent>Test content</MyComponent>);
    expect(screen.getByText('Test content')).toBeInTheDocument();
  });
});
```

---

## 📚 **Resources**

### **Documentation**
- [Design System](./DESIGN_SYSTEM.md)
- [Accessibility Guidelines](./ACCESSIBILITY.md)
- [Testing Guide](./TESTING_GUIDE.md)

### **Tools**
- **Storybook:** Component development và documentation
- **Testing Library:** Component testing
- **axe-core:** Accessibility testing

---

**Changelog v1.0:**
- ✅ Created comprehensive component library
- ✅ Defined core components với TypeScript
- ✅ Added specialized OHT-50 components
- ✅ Implemented accessibility features
- ✅ Added responsive behavior
- ✅ Created testing guidelines
- ✅ Added usage examples