# 🧩 OHT-50 Component Library

**Phiên bản:** v2.0  
**Ngày cập nhật:** 2025-01-27  
**Team:** Frontend  
**Status:** ✅ UPDATED - Enhanced for Industrial Deployment

---

## 📋 **Tổng quan**

Component Library cho OHT-50 Dashboard, cung cấp các reusable components với consistent design và behavior, được nâng cấp cho industrial deployment.

### **🎯 Mục tiêu:**
- Reusable components cho toàn bộ dashboard
- Consistent API và behavior
- Accessibility compliance
- TypeScript support
- Comprehensive documentation
- **NEW:** Industrial-grade monitoring và control

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
<Button variant="emergency">Emergency Stop</Button> // NEW

// Sizes
<Button size="sm">Small</Button>
<Button size="md">Medium</Button>
<Button size="lg">Large</Button>
<Button size="xl">Extra Large</Button> // NEW - for industrial use

// States
<Button disabled>Disabled</Button>
<Button loading>Loading...</Button>
<Button locked>Locked</Button> // NEW - for safety-critical operations
```

**Props:**
- `variant`: 'primary' | 'secondary' | 'ghost' | 'destructive' | 'emergency'
- `size`: 'sm' | 'md' | 'lg' | 'xl'
- `disabled`: boolean
- `loading`: boolean
- `locked`: boolean // NEW
- `onClick`: function
- `confirmationRequired`: boolean // NEW - for critical operations

### **2. Card Component**

```jsx
import { Card, CardHeader, CardContent, CardFooter } from '@/components/ui/card';

<Card>
  <CardHeader>
    <CardTitle>System Status</CardTitle>
    <CardDescription>Current system information</CardDescription>
    <CardStatus status="healthy" /> // NEW - health indicator
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
- `status`: 'healthy' | 'warning' | 'error' | 'critical' // NEW

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
<StatusBadge variant="critical">Critical</StatusBadge> // NEW

// Health indicators
<StatusBadge health="excellent">Excellent</StatusBadge> // NEW
<StatusBadge health="good">Good</StatusBadge> // NEW
<StatusBadge health="fair">Fair</StatusBadge> // NEW
<StatusBadge health="poor">Poor</StatusBadge> // NEW
```

**Props:**
- `state`: 'idle' | 'move' | 'dock' | 'fault' | 'estop'
- `variant`: 'success' | 'warning' | 'error' | 'critical'
- `health`: 'excellent' | 'good' | 'fair' | 'poor' // NEW

### **4. Form Components**

```jsx
import { 
  FormField, 
  FormLabel, 
  FormInput, 
  FormSelect, 
  FormMessage,
  FormRange, // NEW
  FormToggle, // NEW
  FormUnit // NEW
} from '@/components/ui/form';

<FormField>
  <FormLabel>Speed Limit</FormLabel>
  <FormInput 
    type="number" 
    min="0" 
    max="100" 
    placeholder="Enter speed limit"
  />
  <FormUnit unit="m/s" /> // NEW
  <FormMessage>Speed must be between 0 and 100 m/s</FormMessage>
</FormField>

<FormField>
  <FormLabel>Jerk Limit</FormLabel> // NEW
  <FormRange 
    min="0" 
    max="10" 
    step="0.1"
    value={jerkLimit}
    onChange={setJerkLimit}
  />
  <FormUnit unit="m/s³" /> // NEW
</FormField>

<FormField>
  <FormLabel>Safety Mode</FormLabel>
  <FormToggle 
    checked={safetyMode}
    onChange={setSafetyMode}
    description="Enable enhanced safety features"
  />
</FormField>
```

**Props:**
- `error`: boolean
- `disabled`: boolean
- `required`: boolean
- `unit`: string // NEW
- `description`: string // NEW

---

## 🔧 **NEW: Hardware Monitoring Components**

### **1. Motor Status Panel**

```jsx
import { MotorStatusPanel } from '@/components/hardware/motor-status';

<MotorStatusPanel
  motors={[
    {
      id: 'motor-1',
      name: 'Drive Motor',
      current: 2.5,
      voltage: 24.0,
      temperature: 45,
      speed: 1500,
      torque: 12.5,
      efficiency: 94,
      health: 'excellent',
      warnings: []
    },
    {
      id: 'motor-2',
      name: 'Steering Motor',
      current: 1.8,
      voltage: 24.0,
      temperature: 42,
      speed: 800,
      torque: 8.2,
      efficiency: 92,
      health: 'good',
      warnings: ['Temperature approaching limit']
    }
  ]}
  onMotorSelect={(motorId) => handleMotorSelect(motorId)}
  onMaintenanceAlert={(motorId) => handleMaintenanceAlert(motorId)}
/>
```

**Props:**
- `motors`: MotorData[]
- `onMotorSelect`: function
- `onMaintenanceAlert`: function
- `showDetails`: boolean

### **2. Sensor Health Panel**

```jsx
import { SensorHealthPanel } from '@/components/hardware/sensor-health';

<SensorHealthPanel
  sensors={[
    {
      id: 'lidar-1',
      name: 'LiDAR Front',
      type: 'lidar',
      status: 'operational',
      accuracy: 98.5,
      range: 10.2,
      calibration: 'valid',
      lastCalibration: '2024-01-20',
      nextCalibration: '2024-02-20',
      health: 'excellent'
    },
    {
      id: 'encoder-1',
      name: 'Position Encoder',
      type: 'encoder',
      status: 'operational',
      resolution: 4096,
      accuracy: 99.2,
      calibration: 'valid',
      lastCalibration: '2024-01-15',
      nextCalibration: '2024-02-15',
      health: 'good'
    }
  ]}
  onCalibrationRequest={(sensorId) => handleCalibrationRequest(sensorId)}
  onSensorTest={(sensorId) => handleSensorTest(sensorId)}
/>
```

**Props:**
- `sensors`: SensorData[]
- `onCalibrationRequest`: function
- `onSensorTest`: function
- `showCalibrationSchedule`: boolean

### **3. Power System Monitor**

```jsx
import { PowerSystemMonitor } from '@/components/hardware/power-system';

<PowerSystemMonitor
  powerData={{
    voltage: { main: 24.0, aux: 12.0 },
    current: { main: 15.5, aux: 2.1 },
    power: { total: 372, efficiency: 94.5 },
    battery: { level: 85, health: 92, cycles: 1250 },
    temperature: { battery: 38, controller: 42 },
    alerts: ['Battery health below 95%']
  }}
  onPowerAlert={(alert) => handlePowerAlert(alert)}
  onBatteryMaintenance={() => handleBatteryMaintenance()}
/>
```

**Props:**
- `powerData`: PowerData
- `onPowerAlert`: function
- `onBatteryMaintenance`: function
- `showEfficiencyTrends`: boolean

---

## 🛡️ **NEW: Advanced Safety Components**

### **1. Safety Configuration Panel**

```jsx
import { SafetyConfigPanel } from '@/components/safety/safety-config';

<SafetyConfigPanel
  config={{
    velocity: { max: 3.0, emergency: 1.5 },
    acceleration: { max: 2.0, emergency: 0.5 },
    jerk: { max: 5.0, emergency: 1.0 }, // NEW
    deceleration: { normal: 2.0, emergency: 4.0 }, // NEW
    safetyMargins: { static: 0.5, dynamic: 0.2 }, // NEW
    obstacleDetection: {
      range: 10.0,
      sensitivity: 0.8,
      falsePositiveFilter: 0.9
    },
    interlock: {
      enabled: true,
      timeout: 100,
      redundancy: 'dual'
    }
  }}
  onConfigUpdate={(config) => handleConfigUpdate(config)}
  onValidationRequest={() => handleValidationRequest()}
/>
```

**Props:**
- `config`: SafetyConfig
- `onConfigUpdate`: function
- `onValidationRequest`: function
- `showAdvancedOptions`: boolean

### **2. Safety Zone Editor**

```jsx
import { SafetyZoneEditor } from '@/components/safety/safety-zone-editor';

<SafetyZoneEditor
  zones={[
    {
      id: 'zone-1',
      name: 'Loading Area',
      type: 'restricted',
      coordinates: [[0, 0], [10, 0], [10, 5], [0, 5]],
      speedLimit: 0.5,
      accessLevel: 'authorized',
      active: true
    },
    {
      id: 'zone-2',
      name: 'High Speed Corridor',
      type: 'speed-limited',
      coordinates: [[10, 0], [50, 0], [50, 3], [10, 3]],
      speedLimit: 2.5,
      accessLevel: 'all',
      active: true
    }
  ]}
  onZoneAdd={(zone) => handleZoneAdd(zone)}
  onZoneEdit={(zoneId, zone) => handleZoneEdit(zoneId, zone)}
  onZoneDelete={(zoneId) => handleZoneDelete(zoneId)}
  onZoneValidation={(zoneId) => handleZoneValidation(zoneId)}
/>
```

**Props:**
- `zones`: SafetyZone[]
- `onZoneAdd`: function
- `onZoneEdit`: function
- `onZoneDelete`: function
- `onZoneValidation`: function
- `show3DView`: boolean

### **3. Safety Status Dashboard**

```jsx
import { SafetyStatusDashboard } from '@/components/safety/safety-status';

<SafetyStatusDashboard
  safetyStatus={{
    eStop: { status: 'safe', lastTest: '2024-01-27 10:30' },
    collision: { status: 'safe', distance: 2.5, threshold: 1.0 },
    velocity: { status: 'warning', current: 2.8, limit: 3.0 },
    acceleration: { status: 'safe', current: 1.2, limit: 2.0 },
    jerk: { status: 'safe', current: 2.1, limit: 5.0 }, // NEW
    interlock: { status: 'safe', engaged: true },
    zones: { active: 3, violated: 0 },
    compliance: { sil2: true, lastAudit: '2024-01-15' }
  }}
  onSafetyAlert={(alert) => handleSafetyAlert(alert)}
  onEmergencyStop={() => handleEmergencyStop()}
/>
```

**Props:**
- `safetyStatus`: SafetyStatus
- `onSafetyAlert`: function
- `onEmergencyStop`: function
- `showComplianceDetails`: boolean

---

## 📊 **NEW: Performance Analytics Components**

### **1. Efficiency Metrics Panel**

```jsx
import { EfficiencyMetricsPanel } from '@/components/analytics/efficiency-metrics';

<EfficiencyMetricsPanel
  efficiencyData={{
    power: {
      consumption: 15.2, // kWh
      efficiency: 94.5, // %
      trend: 'improving',
      breakdown: {
        motors: 65,
        electronics: 20,
        auxiliaries: 15
      }
    },
    operational: {
      uptime: 98.5, // %
      cycleTime: 45.2, // seconds
      throughput: 125, // items/hour
      utilization: 87.3 // %
    },
    quality: {
      accuracy: 99.2, // %
      repeatability: 98.8, // %
      defectRate: 0.1, // %
      customerSatisfaction: 96.5 // %
    }
  }}
  timeRange="24h"
  onMetricSelect={(metric) => handleMetricSelect(metric)}
/>
```

**Props:**
- `efficiencyData`: EfficiencyData
- `timeRange`: string
- `onMetricSelect`: function
- `showTrends`: boolean

### **2. Performance Trend Chart**

```jsx
import { PerformanceTrendChart } from '@/components/analytics/performance-trend';

<PerformanceTrendChart
  data={performanceData}
  metrics={['efficiency', 'power', 'throughput', 'quality']}
  timeRange="7d"
  comparison="previous-week"
  onMetricToggle={(metric) => handleMetricToggle(metric)}
  onTimeRangeChange={(range) => handleTimeRangeChange(range)}
/>
```

**Props:**
- `data`: PerformanceData[]
- `metrics`: string[]
- `timeRange`: string
- `comparison`: string
- `onMetricToggle`: function
- `onTimeRangeChange`: function

### **3. Quality Assurance Panel**

```jsx
import { QualityAssurancePanel } from '@/components/analytics/quality-assurance';

<QualityAssurancePanel
  qualityData={{
    positioning: {
      accuracy: 99.2, // mm
      repeatability: 98.8, // %
      drift: 0.05, // mm/hour
      calibration: 'valid'
    },
    taskCompletion: {
      successRate: 99.5, // %
      averageTime: 45.2, // seconds
      variance: 2.1, // seconds
      trends: 'improving'
    },
    safety: {
      incidents: 0,
      nearMisses: 2,
      safetyScore: 98.5, // %
      compliance: 'full'
    }
  }}
  onQualityAlert={(alert) => handleQualityAlert(alert)}
  onCalibrationRequest={() => handleCalibrationRequest()}
/>
```

**Props:**
- `qualityData`: QualityData
- `onQualityAlert`: function
- `onCalibrationRequest`: function
- `showDetailedMetrics`: boolean

---

## 🔧 **NEW: Maintenance Dashboard Components**

### **1. Predictive Maintenance Panel**

```jsx
import { PredictiveMaintenancePanel } from '@/components/maintenance/predictive-maintenance';

<PredictiveMaintenancePanel
  maintenanceData={{
    components: [
      {
        id: 'motor-1',
        name: 'Drive Motor',
        health: 85, // %
        remainingLife: 1200, // hours
        nextService: '2024-02-15',
        criticality: 'high',
        recommendations: ['Replace bearings', 'Check alignment']
      },
      {
        id: 'belt-1',
        name: 'Drive Belt',
        health: 72, // %
        remainingLife: 800, // hours
        nextService: '2024-02-01',
        criticality: 'medium',
        recommendations: ['Monitor wear', 'Prepare replacement']
      }
    ],
    alerts: [
      { component: 'motor-1', type: 'maintenance', priority: 'high' },
      { component: 'belt-1', type: 'wear', priority: 'medium' }
    ]
  }}
  onMaintenanceSchedule={(componentId) => handleMaintenanceSchedule(componentId)}
  onComponentSelect={(componentId) => handleComponentSelect(componentId)}
/>
```

**Props:**
- `maintenanceData`: MaintenanceData
- `onMaintenanceSchedule`: function
- `onComponentSelect`: function
- `showSpareParts`: boolean

### **2. Spare Parts Management**

```jsx
import { SparePartsManagement } from '@/components/maintenance/spare-parts';

<SparePartsManagement
  spareParts={[
    {
      id: 'bearing-001',
      name: 'Motor Bearing',
      partNumber: 'MB-001',
      quantity: 5,
      minQuantity: 2,
      cost: 45.00,
      supplier: 'ABC Bearings',
      leadTime: 7, // days
      lastOrder: '2024-01-20',
      nextOrder: '2024-02-10'
    },
    {
      id: 'belt-001',
      name: 'Drive Belt',
      partNumber: 'DB-001',
      quantity: 3,
      minQuantity: 1,
      cost: 25.00,
      supplier: 'XYZ Belts',
      leadTime: 3, // days
      lastOrder: '2024-01-25',
      nextOrder: '2024-02-05'
    }
  ]}
  onOrderParts={(parts) => handleOrderParts(parts)}
  onInventoryUpdate={(partId, quantity) => handleInventoryUpdate(partId, quantity)}
/>
```

**Props:**
- `spareParts`: SparePart[]
- `onOrderParts`: function
- `onInventoryUpdate`: function
- `showCostAnalysis`: boolean

### **3. Service History**

```jsx
import { ServiceHistory } from '@/components/maintenance/service-history';

<ServiceHistory
  serviceRecords={[
    {
      id: 'service-001',
      date: '2024-01-15',
      type: 'preventive',
      component: 'motor-1',
      technician: 'John Smith',
      description: 'Routine bearing inspection and lubrication',
      parts: ['lubricant-001'],
      cost: 150.00,
      duration: 2, // hours
      status: 'completed'
    },
    {
      id: 'service-002',
      date: '2024-01-20',
      type: 'corrective',
      component: 'belt-1',
      technician: 'Jane Doe',
      description: 'Belt replacement due to wear',
      parts: ['belt-001'],
      cost: 75.00,
      duration: 1, // hours
      status: 'completed'
    }
  ]}
  onServiceRecord={(record) => handleServiceRecord(record)}
  onServiceSchedule={(componentId) => handleServiceSchedule(componentId)}
/>
```

**Props:**
- `serviceRecords`: ServiceRecord[]
- `onServiceRecord`: function
- `onServiceSchedule`: function
- `showCostAnalysis`: boolean

---

## 📈 **Enhanced Chart Components**

### **1. Advanced VAX Chart**

```jsx
import { AdvancedVAXChart } from '@/components/charts/advanced-vax-chart';

<AdvancedVAXChart
  data={chartData}
  channels={['v', 'a', 'x', 'j']} // Added jerk (j)
  timeRange={30}
  live={true}
  safetyLimits={{
    velocity: { max: 3.0, warning: 2.5 },
    acceleration: { max: 2.0, warning: 1.5 },
    jerk: { max: 5.0, warning: 3.0 } // NEW
  }}
  onLimitViolation={(channel, value) => handleLimitViolation(channel, value)}
  onChannelToggle={(channel) => setActiveChannels(channel)}
  onTimeRangeChange={(range) => setTimeRange(range)}
/>
```

**Props:**
- `data`: ChartData[]
- `channels`: ('v' | 'a' | 'x' | 'j')[]
- `timeRange`: number (seconds)
- `live`: boolean
- `safetyLimits`: SafetyLimits // NEW
- `onLimitViolation`: function // NEW
- `onChannelToggle`: function
- `onTimeRangeChange`: function

### **2. Power Consumption Chart**

```jsx
import { PowerConsumptionChart } from '@/components/charts/power-consumption';

<PowerConsumptionChart
  data={powerData}
  metrics={['voltage', 'current', 'power', 'efficiency']}
  timeRange="24h"
  breakdown={{
    motors: true,
    electronics: true,
    auxiliaries: true
  }}
  onMetricSelect={(metric) => handleMetricSelect(metric)}
  onEfficiencyAlert={(alert) => handleEfficiencyAlert(alert)}
/>
```

**Props:**
- `data`: PowerData[]
- `metrics`: string[]
- `timeRange`: string
- `breakdown`: PowerBreakdown
- `onMetricSelect`: function
- `onEfficiencyAlert`: function

---

## 🎯 **Enhanced Layout Components**

### **1. Industrial Dashboard Layout**

```jsx
import { IndustrialDashboardLayout } from '@/components/layout/industrial-dashboard';

<IndustrialDashboardLayout>
  <IndustrialDashboardLayout.Sidebar>
    <Navigation />
  </IndustrialDashboardLayout.Sidebar>
  <IndustrialDashboardLayout.Header>
    <Header />
    <EmergencyControls /> // NEW
  </IndustrialDashboardLayout.Header>
  <IndustrialDashboardLayout.Content>
    <PageContent />
  </IndustrialDashboardLayout.Content>
  <IndustrialDashboardLayout.Footer>
    <SystemStatus />
    <AlertsPanel /> // NEW
  </IndustrialDashboardLayout.Footer>
</IndustrialDashboardLayout>
```

### **2. Emergency Controls**

```jsx
import { EmergencyControls } from '@/components/layout/emergency-controls';

<EmergencyControls
  eStopActive={false}
  safetyStatus="safe"
  onEmergencyStop={() => handleEmergencyStop()}
  onSafetyOverride={() => handleSafetyOverride()}
  onSystemReset={() => handleSystemReset()}
/>
```

**Props:**
- `eStopActive`: boolean
- `safetyStatus`: string
- `onEmergencyStop`: function
- `onSafetyOverride`: function
- `onSystemReset`: function

---

## ♿ **Enhanced Accessibility Features**

### **Industrial Accessibility**

```jsx
// High contrast mode for industrial environments
<HighContrastMode>
  <Component />
</HighContrastMode>

// Large touch targets for operator use
<LargeTouchTarget minSize={44}>
  <Button>Industrial Button</Button>
</LargeTouchTarget>

// Voice commands for hands-free operation
<VoiceCommandProvider commands={voiceCommands}>
  <Component />
</VoiceCommandProvider>
```

---

## 📚 **Usage Examples**

### **Complete Industrial Dashboard**

```jsx
import { IndustrialDashboardLayout } from '@/components/layout/industrial-dashboard';
import { HardwareMonitoringPanel } from '@/components/hardware/hardware-monitoring';
import { SafetyStatusDashboard } from '@/components/safety/safety-status';
import { PerformanceAnalytics } from '@/components/analytics/performance-analytics';
import { MaintenanceDashboard } from '@/components/maintenance/maintenance-dashboard';

function IndustrialDashboardPage() {
  return (
    <IndustrialDashboardLayout>
      <IndustrialDashboardLayout.Sidebar>
        <Navigation />
      </IndustrialDashboardLayout.Sidebar>
      
      <IndustrialDashboardLayout.Content>
        <div className="space-y-6">
          {/* Hardware Monitoring */}
          <Card>
            <CardHeader>
              <CardTitle>Hardware Status</CardTitle>
              <CardStatus status="healthy" />
            </CardHeader>
            <CardContent>
              <HardwareMonitoringPanel />
            </CardContent>
          </Card>

          {/* Safety Status */}
          <Card>
            <CardHeader>
              <CardTitle>Safety System</CardTitle>
            </CardHeader>
            <CardContent>
              <SafetyStatusDashboard />
            </CardContent>
          </Card>

          {/* Performance Analytics */}
          <Card>
            <CardHeader>
              <CardTitle>Performance Analytics</CardTitle>
            </CardHeader>
            <CardContent>
              <PerformanceAnalytics />
            </CardContent>
          </Card>

          {/* Maintenance Dashboard */}
          <Card>
            <CardHeader>
              <CardTitle>Maintenance</CardTitle>
            </CardHeader>
            <CardContent>
              <MaintenanceDashboard />
            </CardContent>
          </Card>
        </div>
      </IndustrialDashboardLayout.Content>
    </IndustrialDashboardLayout>
  );
}
```

---

## 🔧 **Development Guidelines**

### **Creating Industrial Components**

1. **Safety-First Design:**
```jsx
// Always include safety considerations
interface IndustrialComponentProps {
  safetyCritical?: boolean;
  confirmationRequired?: boolean;
  emergencyOverride?: boolean;
  auditTrail?: boolean;
}
```

2. **Performance Monitoring:**
```jsx
// Include performance metrics
interface PerformanceProps {
  responseTime?: number;
  memoryUsage?: number;
  errorRate?: number;
  uptime?: number;
}
```

3. **Accessibility Standards:**
```jsx
// Industrial accessibility requirements
interface AccessibilityProps {
  highContrast?: boolean;
  largeTouchTargets?: boolean;
  voiceCommands?: boolean;
  screenReader?: boolean;
}
```

---

## 📚 **Resources**

### **Documentation**
- [Design System](./DESIGN_SYSTEM.md)
- [Accessibility Guidelines](./ACCESSIBILITY.md)
- [Industrial Standards](./INDUSTRIAL_STANDARDS.md) // NEW
- [Safety Guidelines](./SAFETY_GUIDELINES.md) // NEW

### **Tools**
- **Storybook:** Component development và documentation
- **Testing Library:** Component testing
- **axe-core:** Accessibility testing
- **Safety Validator:** Safety compliance testing // NEW

---

**Changelog v2.0:**
- ✅ Added Hardware Monitoring components
- ✅ Added Advanced Safety components
- ✅ Added Performance Analytics components
- ✅ Added Maintenance Dashboard components
- ✅ Enhanced Chart components với jerk data
- ✅ Added Industrial Layout components
- ✅ Enhanced Accessibility cho industrial use
- ✅ Added Safety-First design principles
- ✅ Added Performance monitoring capabilities
- ✅ Added Industrial standards compliance