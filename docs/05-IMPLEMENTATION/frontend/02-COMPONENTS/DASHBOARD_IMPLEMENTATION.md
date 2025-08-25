# 📊 Dashboard Implementation - OHT-50

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** 🚧 Implementation in Progress  
**Đối tượng đọc:** Frontend Developers, UI/UX Designers

---

## 🎯 **Mục tiêu Dashboard Implementation:**

### **Mục đích chính:**
- Implement comprehensive dashboard cho robot OHT-50
- Tuân thủ UI/UX Design Requirements đã được hoàn thành
- Đảm bảo safety-first design với emergency controls
- Tối ưu hóa cho real-time data và monitoring

### **Nguyên tắc implementation:**
1. **Safety First:** Emergency controls luôn ưu tiên cao nhất
2. **Real-time Priority:** Real-time data display ưu tiên
3. **Responsive Design:** Mobile-first approach
4. **Accessibility:** WCAG 2.1 AA compliance
5. **Performance:** Optimized cho real-time updates

---

## 🏗️ **DASHBOARD ARCHITECTURE:**

### **Component Structure:**
```
Dashboard
├── Header
│   ├── Logo & Title
│   ├── User Menu
│   └── Quick Actions
├── Sidebar
│   ├── Navigation Menu
│   ├── Robot Status Summary
│   └── Quick Controls
├── Main Content
│   ├── Alert Banner
│   ├── Robot Status Card
│   ├── Control Panel
│   ├── Map Interface
│   └── System Health
└── Footer
    ├── Connection Status
    ├── System Info
    └── Emergency Stop
```

### **Layout Grid System:**
```css
/* Dashboard Grid Layout */
.dashboard-container {
  display: grid;
  grid-template-areas:
    "header header"
    "sidebar main"
    "footer footer";
  grid-template-columns: 280px 1fr;
  grid-template-rows: auto 1fr auto;
  height: 100vh;
  gap: 1rem;
  padding: 1rem;
}

.dashboard-header {
  grid-area: header;
}

.dashboard-sidebar {
  grid-area: sidebar;
}

.dashboard-main {
  grid-area: main;
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 1rem;
  overflow-y: auto;
}

.dashboard-footer {
  grid-area: footer;
}

/* Responsive Breakpoints */
@media (max-width: 1024px) {
  .dashboard-container {
    grid-template-columns: 1fr;
    grid-template-areas:
      "header"
      "main"
      "footer";
  }
  
  .dashboard-sidebar {
    position: fixed;
    left: -280px;
    top: 0;
    height: 100vh;
    z-index: 50;
    transition: left 0.3s ease;
  }
  
  .dashboard-sidebar.open {
    left: 0;
  }
}
```

---

## 🧩 **CORE COMPONENTS:**

### **Dashboard Header:**
```typescript
// src/components/dashboard/DashboardHeader/DashboardHeader.tsx
import React from 'react';
import { useRobotStore } from '../../../stores/robotStore';
import { useAuth } from '../../../hooks/useAuth';
import { cn } from '../../../utils/cn';

const DashboardHeader: React.FC = () => {
  const { status, connection } = useRobotStore();
  const { user, logout } = useAuth();
  
  return (
    <header className="bg-white border-b border-industrial-gray-200 px-6 py-4">
      <div className="flex items-center justify-between">
        {/* Logo & Title */}
        <div className="flex items-center space-x-4">
          <div className="w-10 h-10 bg-primary-orange rounded-lg flex items-center justify-center">
            <span className="text-white font-bold text-lg">O</span>
          </div>
          <div>
            <h1 className="text-xl font-bold text-industrial-gray-900">
              OHT-50 Dashboard
            </h1>
            <p className="text-sm text-industrial-gray-500">
              Robot Control & Monitoring System
            </p>
          </div>
        </div>
        
        {/* Status Indicators */}
        <div className="flex items-center space-x-4">
          {/* Connection Status */}
          <div className="flex items-center space-x-2">
            <div className={cn(
              'w-3 h-3 rounded-full',
              connection === 'connected' ? 'bg-success-green' : 'bg-error-red'
            )} />
            <span className="text-sm text-industrial-gray-600">
              {connection === 'connected' ? 'Connected' : 'Disconnected'}
            </span>
          </div>
          
          {/* Robot Status */}
          <div className="flex items-center space-x-2">
            <div className={cn(
              'w-3 h-3 rounded-full',
              status === 'operational' ? 'bg-success-green' : 
              status === 'warning' ? 'bg-warning-yellow' : 'bg-error-red'
            )} />
            <span className="text-sm text-industrial-gray-600 capitalize">
              {status}
            </span>
          </div>
          
          {/* User Menu */}
          <div className="relative">
            <button className="flex items-center space-x-2 p-2 rounded-md hover:bg-industrial-gray-100">
              <div className="w-8 h-8 bg-primary-orange rounded-full flex items-center justify-center">
                <span className="text-white text-sm font-medium">
                  {user?.name?.charAt(0) || 'U'}
                </span>
              </div>
              <span className="text-sm text-industrial-gray-700">
                {user?.name || 'User'}
              </span>
            </button>
          </div>
        </div>
      </div>
    </header>
  );
};

export default DashboardHeader;
```

### **Robot Status Card:**
```typescript
// src/components/dashboard/RobotStatus/RobotStatus.tsx
import React from 'react';
import { useRobotStore } from '../../../stores/robotStore';
import Card from '../../ui/Card/Card';
import { cn } from '../../../utils/cn';

const RobotStatus: React.FC = () => {
  const { status, battery, speed, position, temperature, connection } = useRobotStore();
  
  const getStatusColor = (status: string) => {
    switch (status) {
      case 'operational':
        return 'text-success-green';
      case 'warning':
        return 'text-warning-yellow';
      case 'error':
        return 'text-error-red';
      default:
        return 'text-industrial-gray-500';
    }
  };
  
  const getBatteryColor = (level: number) => {
    if (level > 50) return 'text-success-green';
    if (level > 20) return 'text-warning-yellow';
    return 'text-error-red';
  };
  
  return (
    <Card className="col-span-1">
      <div className="flex items-center justify-between mb-4">
        <h2 className="text-lg font-semibold text-industrial-gray-900">
          Robot Status
        </h2>
        <div className={cn(
          'px-2 py-1 rounded-full text-xs font-medium',
          getStatusColor(status)
        )}>
          {status.toUpperCase()}
        </div>
      </div>
      
      <div className="space-y-4">
        {/* Battery Level */}
        <div className="flex items-center justify-between">
          <span className="text-sm text-industrial-gray-600">Battery</span>
          <div className="flex items-center space-x-2">
            <div className="w-16 h-2 bg-industrial-gray-200 rounded-full overflow-hidden">
              <div
                className={cn(
                  'h-full rounded-full transition-all duration-300',
                  getBatteryColor(battery.level)
                )}
                style={{ width: `${battery.level}%` }}
              />
            </div>
            <span className={cn(
              'text-sm font-medium',
              getBatteryColor(battery.level)
            )}>
              {battery.level}%
            </span>
          </div>
        </div>
        
        {/* Speed */}
        <div className="flex items-center justify-between">
          <span className="text-sm text-industrial-gray-600">Speed</span>
          <span className="text-sm font-medium text-industrial-gray-900">
            {speed.toFixed(1)} m/s
          </span>
        </div>
        
        {/* Position */}
        <div className="flex items-center justify-between">
          <span className="text-sm text-industrial-gray-600">Position</span>
          <span className="text-sm font-medium text-industrial-gray-900">
            X: {position.x.toFixed(1)}m, Y: {position.y.toFixed(1)}m
          </span>
        </div>
        
        {/* Temperature */}
        <div className="flex items-center justify-between">
          <span className="text-sm text-industrial-gray-600">Temperature</span>
          <span className="text-sm font-medium text-industrial-gray-900">
            {temperature}°C
          </span>
        </div>
        
        {/* Connection */}
        <div className="flex items-center justify-between">
          <span className="text-sm text-industrial-gray-600">Connection</span>
          <div className="flex items-center space-x-1">
            <div className={cn(
              'w-2 h-2 rounded-full',
              connection === 'connected' ? 'bg-success-green' : 'bg-error-red'
            )} />
            <span className="text-sm font-medium text-industrial-gray-900 capitalize">
              {connection}
            </span>
          </div>
        </div>
      </div>
    </Card>
  );
};

export default RobotStatus;
```

### **Alert Banner:**
```typescript
// src/components/dashboard/AlertBanner/AlertBanner.tsx
import React from 'react';
import { useAlertStore } from '../../../stores/alertStore';
import { cn } from '../../../utils/cn';

const AlertBanner: React.FC = () => {
  const { criticalAlerts } = useAlertStore();
  
  if (criticalAlerts.length === 0) return null;
  
  const mostRecentAlert = criticalAlerts[0];
  
  return (
    <div className="col-span-full">
      <div className={cn(
        'p-4 rounded-lg border-l-4 flex items-center justify-between',
        'bg-error-red-50 border-error-red text-error-red-800'
      )}>
        <div className="flex items-center space-x-3">
          <svg className="w-5 h-5" fill="currentColor" viewBox="0 0 20 20">
            <path fillRule="evenodd" d="M8.257 3.099c.765-1.36 2.722-1.36 3.486 0l5.58 9.92c.75 1.334-.213 2.98-1.742 2.98H4.42c-1.53 0-2.493-1.646-1.743-2.98l5.58-9.92zM11 13a1 1 0 11-2 0 1 1 0 012 0zm-1-8a1 1 0 00-1 1v3a1 1 0 002 0V6a1 1 0 00-1-1z" clipRule="evenodd" />
          </svg>
          <div>
            <h3 className="font-medium">{mostRecentAlert.title}</h3>
            <p className="text-sm opacity-90">{mostRecentAlert.message}</p>
          </div>
        </div>
        
        <div className="flex items-center space-x-2">
          <span className="text-sm opacity-75">
            {criticalAlerts.length} critical alert{criticalAlerts.length > 1 ? 's' : ''}
          </span>
          <button className="p-1 rounded hover:bg-error-red-100">
            <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
            </svg>
          </button>
        </div>
      </div>
    </div>
  );
};

export default AlertBanner;
```

---

## 🎮 **CONTROL PANEL:**

### **Control Panel Component:**
```typescript
// src/components/dashboard/ControlPanel/ControlPanel.tsx
import React from 'react';
import { useRobotStore } from '../../../stores/robotStore';
import Card from '../../ui/Card/Card';
import Button from '../../ui/Button/Button';
import EmergencyStop from '../../safety/EmergencyStop/EmergencyStop';

const ControlPanel: React.FC = () => {
  const { sendCommand, mode, setMode } = useRobotStore();
  
  const handleMovement = (direction: string) => {
    sendCommand({
      type: 'movement',
      direction,
      timestamp: new Date()
    });
  };
  
  const handleModeChange = (newMode: string) => {
    setMode(newMode);
  };
  
  return (
    <Card className="col-span-1">
      <h2 className="text-lg font-semibold text-industrial-gray-900 mb-4">
        Control Panel
      </h2>
      
      {/* Mode Selector */}
      <div className="mb-6">
        <label className="block text-sm font-medium text-industrial-gray-700 mb-2">
          Operating Mode
        </label>
        <div className="grid grid-cols-2 gap-2">
          {['auto', 'manual', 'semi', 'maintenance'].map((modeOption) => (
            <Button
              key={modeOption}
              variant={mode === modeOption ? 'primary' : 'secondary'}
              size="sm"
              onClick={() => handleModeChange(modeOption)}
              className="capitalize"
            >
              {modeOption}
            </Button>
          ))}
        </div>
      </div>
      
      {/* Movement Controls */}
      <div className="mb-6">
        <label className="block text-sm font-medium text-industrial-gray-700 mb-2">
          Movement Controls
        </label>
        <div className="grid grid-cols-3 gap-2">
          <div />
          <Button
            variant="secondary"
            onClick={() => handleMovement('forward')}
            className="py-3"
          >
            ↑
          </Button>
          <div />
          
          <Button
            variant="secondary"
            onClick={() => handleMovement('left')}
            className="py-3"
          >
            ←
          </Button>
          <Button
            variant="danger"
            onClick={() => handleMovement('stop')}
            className="py-3"
          >
            ⏹
          </Button>
          <Button
            variant="secondary"
            onClick={() => handleMovement('right')}
            className="py-3"
          >
            →
          </Button>
          
          <div />
          <Button
            variant="secondary"
            onClick={() => handleMovement('backward')}
            className="py-3"
          >
            ↓
          </Button>
          <div />
        </div>
      </div>
      
      {/* Emergency Stop */}
      <div className="mb-4">
        <EmergencyStop />
      </div>
      
      {/* Quick Actions */}
      <div className="grid grid-cols-2 gap-2">
        <Button
          variant="secondary"
          size="sm"
          onClick={() => sendCommand({ type: 'home', timestamp: new Date() })}
        >
          🏠 Home
        </Button>
        <Button
          variant="secondary"
          size="sm"
          onClick={() => sendCommand({ type: 'reset', timestamp: new Date() })}
        >
          🔄 Reset
        </Button>
      </div>
    </Card>
  );
};

export default ControlPanel;
```

---

## 🗺️ **MAP INTERFACE:**

### **Map Interface Component:**
```typescript
// src/components/dashboard/MapInterface/MapInterface.tsx
import React, { useRef, useEffect } from 'react';
import { useRobotStore } from '../../../stores/robotStore';
import Card from '../../ui/Card/Card';

const MapInterface: React.FC = () => {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const { position, obstacles, path } = useRobotStore();
  
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    
    const ctx = canvas.getContext('2d');
    if (!ctx) return;
    
    // Clear canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    // Draw grid
    drawGrid(ctx, canvas.width, canvas.height);
    
    // Draw obstacles
    drawObstacles(ctx, obstacles);
    
    // Draw path
    drawPath(ctx, path);
    
    // Draw robot
    drawRobot(ctx, position);
    
  }, [position, obstacles, path]);
  
  const drawGrid = (ctx: CanvasRenderingContext2D, width: number, height: number) => {
    ctx.strokeStyle = '#E5E7EB';
    ctx.lineWidth = 1;
    
    const gridSize = 20;
    
    for (let x = 0; x < width; x += gridSize) {
      ctx.beginPath();
      ctx.moveTo(x, 0);
      ctx.lineTo(x, height);
      ctx.stroke();
    }
    
    for (let y = 0; y < height; y += gridSize) {
      ctx.beginPath();
      ctx.moveTo(0, y);
      ctx.lineTo(width, y);
      ctx.stroke();
    }
  };
  
  const drawObstacles = (ctx: CanvasRenderingContext2D, obstacles: any[]) => {
    ctx.fillStyle = '#EF4444';
    
    obstacles.forEach(obstacle => {
      ctx.beginPath();
      ctx.arc(obstacle.x, obstacle.y, 5, 0, 2 * Math.PI);
      ctx.fill();
    });
  };
  
  const drawPath = (ctx: CanvasRenderingContext2D, path: any[]) => {
    if (path.length < 2) return;
    
    ctx.strokeStyle = '#FF6B35';
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(path[0].x, path[0].y);
    
    for (let i = 1; i < path.length; i++) {
      ctx.lineTo(path[i].x, path[i].y);
    }
    
    ctx.stroke();
  };
  
  const drawRobot = (ctx: CanvasRenderingContext2D, position: any) => {
    // Draw robot body
    ctx.fillStyle = '#FF6B35';
    ctx.beginPath();
    ctx.arc(position.x, position.y, 8, 0, 2 * Math.PI);
    ctx.fill();
    
    // Draw robot direction
    ctx.strokeStyle = '#FFFFFF';
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(position.x, position.y);
    ctx.lineTo(position.x + Math.cos(position.heading) * 12, 
               position.y + Math.sin(position.heading) * 12);
    ctx.stroke();
  };
  
  return (
    <Card className="col-span-2">
      <div className="flex items-center justify-between mb-4">
        <h2 className="text-lg font-semibold text-industrial-gray-900">
          Robot Map
        </h2>
        <div className="flex items-center space-x-2">
          <button className="p-2 rounded-md hover:bg-industrial-gray-100">
            🔍+
          </button>
          <button className="p-2 rounded-md hover:bg-industrial-gray-100">
            🔍-
          </button>
          <button className="p-2 rounded-md hover:bg-industrial-gray-100">
            🔄
          </button>
        </div>
      </div>
      
      <div className="relative bg-industrial-gray-100 rounded-lg overflow-hidden">
        <canvas
          ref={canvasRef}
          width={600}
          height={400}
          className="w-full h-64 object-cover"
        />
        
        {/* Map Info Overlay */}
        <div className="absolute bottom-4 left-4 bg-white bg-opacity-90 rounded-lg p-3">
          <div className="text-sm text-industrial-gray-600">
            <div>Position: ({position.x.toFixed(1)}, {position.y.toFixed(1)})</div>
            <div>Obstacles: {obstacles.length}</div>
            <div>Path Points: {path.length}</div>
          </div>
        </div>
      </div>
    </Card>
  );
};

export default MapInterface;
```

---

## 📊 **SYSTEM HEALTH:**

### **System Health Component:**
```typescript
// src/components/dashboard/SystemHealth/SystemHealth.tsx
import React from 'react';
import { useRobotStore } from '../../../stores/robotStore';
import Card from '../../ui/Card/Card';
import { cn } from '../../../utils/cn';

const SystemHealth: React.FC = () => {
  const { systemHealth } = useRobotStore();
  
  const getHealthColor = (status: string) => {
    switch (status) {
      case 'good':
        return 'text-success-green';
      case 'warning':
        return 'text-warning-yellow';
      case 'critical':
        return 'text-error-red';
      default:
        return 'text-industrial-gray-500';
    }
  };
  
  const getHealthIcon = (status: string) => {
    switch (status) {
      case 'good':
        return '✅';
      case 'warning':
        return '⚠️';
      case 'critical':
        return '❌';
      default:
        return '❓';
    }
  };
  
  return (
    <Card className="col-span-1">
      <h2 className="text-lg font-semibold text-industrial-gray-900 mb-4">
        System Health
      </h2>
      
      <div className="space-y-3">
        {Object.entries(systemHealth).map(([system, status]) => (
          <div key={system} className="flex items-center justify-between">
            <div className="flex items-center space-x-2">
              <span className="text-lg">{getHealthIcon(status)}</span>
              <span className="text-sm text-industrial-gray-700 capitalize">
                {system.replace(/([A-Z])/g, ' $1').trim()}
              </span>
            </div>
            <span className={cn(
              'text-sm font-medium',
              getHealthColor(status)
            )}>
              {status.toUpperCase()}
            </span>
          </div>
        ))}
      </div>
      
      {/* Overall Health Score */}
      <div className="mt-4 pt-4 border-t border-industrial-gray-200">
        <div className="flex items-center justify-between">
          <span className="text-sm text-industrial-gray-600">Overall Health</span>
          <div className="flex items-center space-x-2">
            <div className="w-16 h-2 bg-industrial-gray-200 rounded-full overflow-hidden">
              <div
                className="h-full bg-success-green rounded-full"
                style={{ width: `${systemHealth.overall}%` }}
              />
            </div>
            <span className="text-sm font-medium text-industrial-gray-900">
              {systemHealth.overall}%
            </span>
          </div>
        </div>
      </div>
    </Card>
  );
};

export default SystemHealth;
```

---

## 📱 **RESPONSIVE IMPLEMENTATION:**

### **Mobile Dashboard:**
```typescript
// src/components/dashboard/DashboardMobile/DashboardMobile.tsx
import React, { useState } from 'react';
import { useResponsive } from '../../../hooks/useResponsive';
import DashboardHeader from '../DashboardHeader/DashboardHeader';
import RobotStatus from '../RobotStatus/RobotStatus';
import ControlPanel from '../ControlPanel/ControlPanel';
import AlertBanner from '../AlertBanner/AlertBanner';

const DashboardMobile: React.FC = () => {
  const [activeTab, setActiveTab] = useState('status');
  const { isMobile } = useResponsive();
  
  if (!isMobile) return null;
  
  return (
    <div className="h-screen flex flex-col">
      {/* Header */}
      <DashboardHeader />
      
      {/* Main Content */}
      <div className="flex-1 overflow-y-auto p-4">
        <AlertBanner />
        
        {/* Tab Content */}
        <div className="mt-4">
          {activeTab === 'status' && <RobotStatus />}
          {activeTab === 'control' && <ControlPanel />}
          {activeTab === 'map' && <div>Map Interface (Mobile)</div>}
        </div>
      </div>
      
      {/* Bottom Navigation */}
      <div className="bg-white border-t border-industrial-gray-200">
        <div className="flex">
          <button
            onClick={() => setActiveTab('status')}
            className={cn(
              'flex-1 py-3 px-4 text-center text-sm font-medium',
              activeTab === 'status' 
                ? 'text-primary-orange border-t-2 border-primary-orange' 
                : 'text-industrial-gray-500'
            )}
          >
            📊 Status
          </button>
          <button
            onClick={() => setActiveTab('control')}
            className={cn(
              'flex-1 py-3 px-4 text-center text-sm font-medium',
              activeTab === 'control' 
                ? 'text-primary-orange border-t-2 border-primary-orange' 
                : 'text-industrial-gray-500'
            )}
          >
            🎮 Control
          </button>
          <button
            onClick={() => setActiveTab('map')}
            className={cn(
              'flex-1 py-3 px-4 text-center text-sm font-medium',
              activeTab === 'map' 
                ? 'text-primary-orange border-t-2 border-primary-orange' 
                : 'text-industrial-gray-500'
            )}
          >
            🗺️ Map
          </button>
        </div>
      </div>
    </div>
  );
};

export default DashboardMobile;
```

---

## 📚 **REFERENCES:**

### **UI/UX Requirements:**
- [Dashboard Design](../../../../02-REQUIREMENTS/07-UI-UX-DESIGN-REQUIREMENTS/REQ_UI_UX_Dashboard_Design.md)
- [Control Panel](../../../../02-REQUIREMENTS/07-UI-UX-DESIGN-REQUIREMENTS/REQ_UI_UX_Control_Panel.md)
- [Alert System](../../../../02-REQUIREMENTS/07-UI-UX-DESIGN-REQUIREMENTS/REQ_UI_UX_Alert_System.md)

### **Design Assets:**
- [SuperDesign Iterations](../../../../../.superdesign/design_iterations/)
- [Design Plan](../../../../../.superdesign/OHT-50_DESIGN_PLAN.md)

---

**Changelog v1.0:**
- ✅ Created comprehensive dashboard implementation
- ✅ Implemented responsive layout với grid system
- ✅ Created core dashboard components
- ✅ Added safety-first design principles
- ✅ Implemented real-time data display
- ✅ Added mobile-responsive design
- ✅ Created emergency controls integration
- ✅ Added alert system integration

**🚨 Lưu ý:** Dashboard implementation tuân thủ strict safety-first principles với real-time data handling ưu tiên cho robot OHT-50.

