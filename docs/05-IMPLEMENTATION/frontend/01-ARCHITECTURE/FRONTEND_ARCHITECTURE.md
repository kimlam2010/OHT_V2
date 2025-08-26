# 🏗️ Frontend Architecture - OHT-50 Dashboard

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** Frontend Developers, Architects, Team Leads

---

## 🎯 **Mục tiêu kiến trúc:**

### **Mục đích chính:**
- Thiết kế kiến trúc frontend scalable cho OHT-50 dashboard
- Đảm bảo performance và maintainability
- Tuân thủ safety-first design principles
- Tối ưu hóa cho real-time data và emergency controls

### **Nguyên tắc kiến trúc:**
1. **Safety First:** Emergency controls luôn ưu tiên cao nhất
2. **Real-time Priority:** Real-time data handling ưu tiên
3. **Modular Design:** Component-based architecture
4. **Performance Focus:** Optimized cho mobile và desktop
5. **Accessibility:** WCAG 2.1 AA compliance built-in

---

## 🏗️ **KIẾN TRÚC TỔNG THỂ:**

### **High-Level Architecture:**
```
┌─────────────────────────────────────────────────────────────┐
│                    OHT-50 Frontend                          │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐        │
│  │   React     │  │  TypeScript │  │   Tailwind  │        │
│  │    18       │  │             │  │     CSS     │        │
│  └─────────────┘  └─────────────┘  └─────────────┘        │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐        │
│  │   Zustand   │  │  React      │  │   Custom    │        │
│  │   State     │  │  Router     │  │   Hooks     │        │
│  │ Management  │  │             │  │             │        │
│  └─────────────┘  └─────────────┘  └─────────────┘        │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐        │
│  │  WebSocket  │  │   REST API  │  │   SSE       │        │
│  │  Client     │  │   Client    │  │   Client    │        │
│  └─────────────┘  └─────────────┘  └─────────────┘        │
├─────────────────────────────────────────────────────────────┤
│                    Backend Services                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐        │
│  │   Auth      │  │   Robot     │  │  Telemetry  │        │
│  │   API       │  │   Control   │  │    API      │        │
│  └─────────────┘  └─────────────┘  └─────────────┘        │
└─────────────────────────────────────────────────────────────┘
```

---

## 📁 **PROJECT STRUCTURE:**

```
src/
├── components/                 # Reusable UI components
│   ├── ui/                    # Base UI components
│   │   ├── Button/
│   │   ├── Input/
│   │   ├── Card/
│   │   └── Modal/
│   ├── layout/                # Layout components
│   │   ├── Sidebar/
│   │   ├── Header/
│   │   ├── Footer/
│   │   └── Navigation/
│   ├── dashboard/             # Dashboard specific components
│   │   ├── RobotStatus/
│   │   ├── AlertBanner/
│   │   ├── ControlPanel/
│   │   └── MapInterface/
│   ├── safety/                # Safety-critical components
│   │   ├── EmergencyStop/
│   │   ├── AlertSystem/
│   │   └── SafetyIndicators/
│   └── features/              # Feature-specific components
│       ├── Configuration/
│       ├── Reports/
│       └── Missions/
├── pages/                     # Page components
│   ├── Dashboard/
│   ├── RobotControl/
│   ├── MapView/
│   ├── Configuration/
│   ├── Reports/
│   └── Missions/
├── hooks/                     # Custom React hooks
│   ├── useWebSocket.ts
│   ├── useRobotStatus.ts
│   ├── useAlerts.ts
│   └── useRealTimeData.ts
├── stores/                    # Zustand state stores
│   ├── robotStore.ts
│   ├── alertStore.ts
│   ├── configStore.ts
│   └── missionStore.ts
├── services/                  # API and external services
│   ├── api/
│   │   ├── auth.ts
│   │   ├── robot.ts
│   │   ├── telemetry.ts
│   │   └── config.ts
│   ├── websocket/
│   │   ├── client.ts
│   │   └── events.ts
│   └── utils/
│       ├── validation.ts
│       ├── formatting.ts
│       └── constants.ts
├── types/                     # TypeScript type definitions
│   ├── robot.ts
│   ├── alerts.ts
│   ├── config.ts
│   └── api.ts
├── styles/                    # Global styles and design system
│   ├── globals.css
│   ├── design-tokens.css
│   └── components.css
└── utils/                     # Utility functions
    ├── accessibility.ts
    ├── performance.ts
    └── safety.ts
```

---

## 🧩 **COMPONENT ARCHITECTURE:**

### **Component Hierarchy:**
```
App
├── Layout
│   ├── Sidebar
│   ├── Header
│   └── MainContent
├── Router
│   ├── Dashboard
│   │   ├── RobotStatus
│   │   ├── AlertBanner
│   │   ├── QuickActions
│   │   └── SystemHealth
│   ├── RobotControl
│   │   ├── ControlPanel
│   │   ├── EmergencyControls
│   │   └── MovementControls
│   ├── MapView
│   │   ├── LiDARVisualization
│   │   ├── NavigationPath
│   │   └── ObstacleDetection
│   └── Configuration
│       ├── SettingsPanel
│       ├── ValidationSystem
│       └── ConfigForm
└── GlobalComponents
    ├── AlertSystem
    ├── EmergencyStop
    └── LoadingStates
```

### **Component Design Principles:**
1. **Single Responsibility:** Mỗi component có một trách nhiệm rõ ràng
2. **Composition over Inheritance:** Sử dụng composition pattern
3. **Props Interface:** Strict TypeScript interfaces cho props
4. **Error Boundaries:** Error handling ở mỗi level
5. **Performance Optimization:** React.memo, useMemo, useCallback

---

## 🔄 **STATE MANAGEMENT:**

### **Zustand Store Structure:**
```typescript
// Robot Store
interface RobotStore {
  // State
  status: RobotStatus;
  position: Position;
  battery: BatteryInfo;
  temperature: number;
  connection: ConnectionStatus;
  
  // Actions
  updateStatus: (status: RobotStatus) => void;
  updatePosition: (position: Position) => void;
  updateBattery: (battery: BatteryInfo) => void;
  emergencyStop: () => void;
  reset: () => void;
}

// Alert Store
interface AlertStore {
  // State
  alerts: Alert[];
  criticalAlerts: Alert[];
  notificationSettings: NotificationSettings;
  
  // Actions
  addAlert: (alert: Alert) => void;
  removeAlert: (id: string) => void;
  acknowledgeAlert: (id: string) => void;
  clearAllAlerts: () => void;
}

// Configuration Store
interface ConfigStore {
  // State
  settings: RobotSettings;
  userPreferences: UserPreferences;
  systemConfig: SystemConfig;
  
  // Actions
  updateSettings: (settings: Partial<RobotSettings>) => void;
  saveSettings: () => Promise<void>;
  resetToDefaults: () => void;
  validateSettings: () => ValidationResult;
}
```

### **State Management Patterns:**
1. **Centralized State:** Zustand stores cho global state
2. **Local State:** useState cho component-specific state
3. **Derived State:** useMemo cho computed values
4. **Async State:** Custom hooks cho async operations
5. **Persistence:** LocalStorage cho user preferences

---

## 🌐 **API INTEGRATION:**

### **API Client Structure:**
```typescript
// Base API Client
class ApiClient {
  private baseURL: string;
  private token: string | null;
  
  constructor(baseURL: string) {
    this.baseURL = baseURL;
    this.token = localStorage.getItem('auth_token');
  }
  
  async request<T>(endpoint: string, options?: RequestOptions): Promise<T> {
    // Implementation with error handling, retry logic, etc.
  }
}

// Specialized API Clients
class RobotApiClient extends ApiClient {
  async getStatus(): Promise<RobotStatus> {
    return this.request<RobotStatus>('/robot/status');
  }
  
  async sendCommand(command: RobotCommand): Promise<CommandResponse> {
    return this.request<CommandResponse>('/robot/command', {
      method: 'POST',
      body: JSON.stringify(command)
    });
  }
  
  async emergencyStop(): Promise<EmergencyResponse> {
    return this.request<EmergencyResponse>('/robot/emergency-stop', {
      method: 'POST'
    });
  }
}
```

### **WebSocket Integration:**
```typescript
// WebSocket Client
class WebSocketClient {
  private ws: WebSocket | null = null;
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 5;
  
  connect(url: string): void {
    this.ws = new WebSocket(url);
    this.setupEventHandlers();
  }
  
  private setupEventHandlers(): void {
    if (!this.ws) return;
    
    this.ws.onopen = () => {
      console.log('WebSocket connected');
      this.reconnectAttempts = 0;
    };
    
    this.ws.onmessage = (event) => {
      const data = JSON.parse(event.data);
      this.handleMessage(data);
    };
    
    this.ws.onclose = () => {
      console.log('WebSocket disconnected');
      this.handleReconnect();
    };
  }
  
  private handleMessage(data: WebSocketMessage): void {
    switch (data.type) {
      case 'robot.status':
        robotStore.updateStatus(data.payload);
        break;
      case 'robot.telemetry':
        robotStore.updateTelemetry(data.payload);
        break;
      case 'system.alert':
        alertStore.addAlert(data.payload);
        break;
      default:
        console.warn('Unknown message type:', data.type);
    }
  }
}
```

---

## 🎨 **DESIGN SYSTEM INTEGRATION:**

### **Design Tokens Implementation:**
```css
/* design-tokens.css */
:root {
  /* Colors */
  --primary-orange: #FF6B35;
  --primary-orange-light: #FF8A5C;
  --primary-orange-dark: #E55A2B;
  
  /* Industrial Grays */
  --industrial-gray-50: #F9FAFB;
  --industrial-gray-100: #F3F4F6;
  --industrial-gray-200: #E5E7EB;
  --industrial-gray-300: #D1D5DB;
  --industrial-gray-400: #9CA3AF;
  --industrial-gray-500: #6B7280;
  --industrial-gray-600: #4B5563;
  --industrial-gray-700: #374151;
  --industrial-gray-800: #1F2937;
  --industrial-gray-900: #111827;
  
  /* Semantic Colors */
  --success-green: #10B981;
  --warning-yellow: #F59E0B;
  --error-red: #EF4444;
  --info-blue: #3B82F6;
  
  /* Safety Colors */
  --safety-red: #DC2626;
  --safety-orange: #EA580C;
  --safety-yellow: #CA8A04;
  --safety-green: #16A34A;
  
  /* Typography */
  --font-sans: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
  --font-mono: 'JetBrains Mono', 'Fira Code', 'Source Code Pro', monospace;
  --font-display: 'Space Grotesk', 'Inter', sans-serif;
  
  /* Spacing */
  --space-xs: 0.25rem;
  --space-sm: 0.5rem;
  --space-md: 1rem;
  --space-lg: 1.5rem;
  --space-xl: 2rem;
  --space-2xl: 3rem;
  
  /* Shadows */
  --shadow-xs: 0 1px 2px 0 rgba(0, 0, 0, 0.05);
  --shadow-sm: 0 1px 3px 0 rgba(0, 0, 0, 0.1), 0 1px 2px 0 rgba(0, 0, 0, 0.06);
  --shadow-md: 0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06);
  --shadow-lg: 0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -2px rgba(0, 0, 0, 0.05);
  --shadow-xl: 0 20px 25px -5px rgba(0, 0, 0, 0.1), 0 10px 10px -5px rgba(0, 0, 0, 0.04);
}
```

### **Component Styling Strategy:**
1. **Tailwind CSS:** Utility-first approach
2. **CSS Modules:** Component-specific styles
3. **Design Tokens:** Consistent design system
4. **Responsive Design:** Mobile-first approach
5. **Dark Mode Support:** Theme switching capability

---

## 🚨 **SAFETY-FIRST ARCHITECTURE:**

### **Emergency Controls Priority:**
```typescript
// Emergency Stop Component
const EmergencyStop: React.FC = () => {
  const { emergencyStop } = useRobotStore();
  
  const handleEmergencyStop = useCallback(() => {
    // Immediate action - no confirmation needed
    emergencyStop();
    
    // Visual feedback
    showEmergencyAlert();
    
    // Audio feedback
    playEmergencySound();
  }, [emergencyStop]);
  
  return (
    <button
      className="emergency-stop-button"
      onClick={handleEmergencyStop}
      aria-label="Emergency Stop - Immediate robot halt"
      data-testid="emergency-stop"
    >
      <EmergencyIcon />
      <span>EMERGENCY STOP</span>
    </button>
  );
};
```

### **Safety Patterns:**
1. **Immediate Response:** Emergency controls respond instantly
2. **Visual Feedback:** Clear visual indicators for safety status
3. **Audio Alerts:** Configurable audio notifications
4. **Confirmation Dialogs:** Required for dangerous actions
5. **Error Recovery:** Graceful error handling và recovery

---

## 📱 **RESPONSIVE ARCHITECTURE:**

### **Breakpoint Strategy:**
```typescript
// Responsive hooks
const useBreakpoint = () => {
  const [breakpoint, setBreakpoint] = useState<Breakpoint>('mobile');
  
  useEffect(() => {
    const handleResize = () => {
      const width = window.innerWidth;
      if (width < 768) setBreakpoint('mobile');
      else if (width < 1024) setBreakpoint('tablet');
      else if (width < 1440) setBreakpoint('desktop');
      else setBreakpoint('large');
    };
    
    handleResize();
    window.addEventListener('resize', handleResize);
    return () => window.removeEventListener('resize', handleResize);
  }, []);
  
  return breakpoint;
};
```

### **Responsive Patterns:**
1. **Mobile-First:** Design for mobile first, enhance for larger screens
2. **Progressive Enhancement:** Add features for larger screens
3. **Touch-Friendly:** Minimum 44px touch targets
4. **Adaptive Layout:** Layout changes based on screen size
5. **Performance Optimization:** Optimize for mobile performance

---

## 🧪 **TESTING ARCHITECTURE:**

### **Testing Strategy:**
```typescript
// Component testing example
describe('EmergencyStop', () => {
  it('should trigger emergency stop immediately', () => {
    const mockEmergencyStop = jest.fn();
    render(<EmergencyStop onEmergencyStop={mockEmergencyStop} />);
    
    const button = screen.getByTestId('emergency-stop');
    fireEvent.click(button);
    
    expect(mockEmergencyStop).toHaveBeenCalledTimes(1);
  });
  
  it('should be accessible via keyboard', () => {
    render(<EmergencyStop />);
    
    const button = screen.getByTestId('emergency-stop');
    button.focus();
    
    expect(button).toHaveFocus();
    fireEvent.keyDown(button, { key: 'Enter' });
    
    // Verify emergency stop was triggered
  });
});
```

### **Testing Layers:**
1. **Unit Tests:** Component và utility function testing
2. **Integration Tests:** API integration và state management
3. **E2E Tests:** User flow testing
4. **Accessibility Tests:** WCAG compliance testing
5. **Performance Tests:** Load time và responsiveness testing

---

## 📊 **PERFORMANCE ARCHITECTURE:**

### **Performance Optimization:**
```typescript
// Performance monitoring
const usePerformanceMonitor = () => {
  useEffect(() => {
    // Monitor Core Web Vitals
    const observer = new PerformanceObserver((list) => {
      for (const entry of list.getEntries()) {
        if (entry.entryType === 'largest-contentful-paint') {
          console.log('LCP:', entry.startTime);
        }
      }
    });
    
    observer.observe({ entryTypes: ['largest-contentful-paint'] });
    
    return () => observer.disconnect();
  }, []);
};

// Code splitting
const Dashboard = lazy(() => import('./pages/Dashboard'));
const RobotControl = lazy(() => import('./pages/RobotControl'));
const MapView = lazy(() => import('./pages/MapView'));
```

### **Performance Targets:**
1. **Initial Load:** < 2 seconds
2. **Subsequent Navigation:** < 500ms
3. **Real-time Updates:** < 100ms
4. **Bundle Size:** < 500KB main bundle
5. **Core Web Vitals:** LCP < 2.5s, FID < 100ms, CLS < 0.1

---

## 🔒 **SECURITY ARCHITECTURE:**

### **Security Measures:**
```typescript
// Authentication hook
const useAuth = () => {
  const [user, setUser] = useState<User | null>(null);
  const [loading, setLoading] = useState(true);
  
  useEffect(() => {
    const token = localStorage.getItem('auth_token');
    if (token) {
      validateToken(token).then(user => {
        setUser(user);
        setLoading(false);
      });
    } else {
      setLoading(false);
    }
  }, []);
  
  return { user, loading };
};

// Route protection
const ProtectedRoute: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const { user, loading } = useAuth();
  
  if (loading) return <LoadingSpinner />;
  if (!user) return <Navigate to="/login" />;
  
  return <>{children}</>;
};
```

### **Security Patterns:**
1. **Authentication:** JWT token-based authentication
2. **Authorization:** Role-based access control
3. **Input Validation:** Client-side và server-side validation
4. **XSS Prevention:** Content Security Policy
5. **CSRF Protection:** Token-based CSRF protection

---

## 📚 **REFERENCES:**

### **UI/UX Requirements:**
- [Design System](../../../02-REQUIREMENTS/07-UI-UX-DESIGN-REQUIREMENTS/REQ_UI_UX_Design_System.md)
- [Dashboard Design](../../../02-REQUIREMENTS/07-UI-UX-DESIGN-REQUIREMENTS/REQ_UI_UX_Dashboard_Design.md)
- [Control Panel](../../../02-REQUIREMENTS/07-UI-UX-DESIGN-REQUIREMENTS/REQ_UI_UX_Control_Panel.md)
- [Alert System](../../../02-REQUIREMENTS/07-UI-UX-DESIGN-REQUIREMENTS/REQ_UI_UX_Alert_System.md)

### **Backend Integration:**
- [API Specifications](../backend/03-API-SPECIFICATIONS/)
- [WebSocket Events](../backend/03-API-SPECIFICATIONS/WEBSOCKET_EVENTS.md)
- [Authentication System](../backend/11-SECURITY/AUTH_SYSTEM.md)

---

**Changelog v1.0:**
- ✅ Created comprehensive frontend architecture
- ✅ Defined component structure và hierarchy
- ✅ Implemented state management strategy
- ✅ Added API integration patterns
- ✅ Created safety-first architecture
- ✅ Added responsive design strategy
- ✅ Implemented testing architecture
- ✅ Added performance optimization strategy
- ✅ Created security architecture

**🚨 Lưu ý:** Kiến trúc frontend tuân thủ strict safety-first principles với real-time data handling ưu tiên cho robot OHT-50.
