# State Management Implementation

**Version:** 1.0  
**Date:** 2024-12-19  
**Team:** Frontend  
**Status:** In Progress

## 📋 **Overview**

State management implementation cho OHT-50 frontend sử dụng Zustand, tuân thủ safety-first principles và real-time requirements.

## 🏗️ **Architecture Overview**

### **State Management Layers**
```typescript
// State Management Architecture
┌─────────────────────────────────────┐
│           UI Components             │
├─────────────────────────────────────┤
│         Zustand Stores              │
├─────────────────────────────────────┤
│         API Services                │
├─────────────────────────────────────┤
│        WebSocket Client             │
├─────────────────────────────────────┤
│         Backend API                 │
└─────────────────────────────────────┘
```

### **Store Structure**
```typescript
// Store Organization
stores/
├── auth/
│   ├── authStore.ts          // Authentication state
│   └── authActions.ts        // Auth actions
├── system/
│   ├── systemStore.ts        // System status & health
│   └── systemActions.ts      // System actions
├── control/
│   ├── controlStore.ts       // Control state
│   └── controlActions.ts     // Control actions
├── telemetry/
│   ├── telemetryStore.ts     // Real-time telemetry
│   └── telemetryActions.ts   // Telemetry actions
├── alerts/
│   ├── alertStore.ts         // Alert management
│   └── alertActions.ts       // Alert actions
├── missions/
│   ├── missionStore.ts       // Mission management
│   └── missionActions.ts     // Mission actions
└── config/
    ├── configStore.ts        // Configuration state
    └── configActions.ts      // Config actions
```

## 🔐 **Authentication Store**

### **Auth Store Implementation**
```typescript
// stores/auth/authStore.ts
import { create } from 'zustand';
import { persist } from 'zustand/middleware';

interface User {
  id: string;
  username: string;
  role: 'admin' | 'operator' | 'guest';
  permissions: string[];
  lastLogin: string;
}

interface AuthState {
  // State
  user: User | null;
  token: string | null;
  isAuthenticated: boolean;
  isLoading: boolean;
  error: string | null;
  
  // Actions
  login: (username: string, password: string) => Promise<void>;
  logout: () => void;
  refreshToken: () => Promise<void>;
  clearError: () => void;
}

export const useAuthStore = create<AuthState>()(
  persist(
    (set, get) => ({
      // Initial state
      user: null,
      token: null,
      isAuthenticated: false,
      isLoading: false,
      error: null,
      
      // Actions
      login: async (username: string, password: string) => {
        set({ isLoading: true, error: null });
        
        try {
          const response = await fetch('/api/auth/login', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ username, password })
          });
          
          if (!response.ok) {
            throw new Error('Login failed');
          }
          
          const data = await response.json();
          
          set({
            user: data.user,
            token: data.token,
            isAuthenticated: true,
            isLoading: false,
            error: null
          });
          
          // Store token in localStorage for persistence
          localStorage.setItem('auth_token', data.token);
          
        } catch (error) {
          set({
            isLoading: false,
            error: error instanceof Error ? error.message : 'Login failed'
          });
        }
      },
      
      logout: () => {
        set({
          user: null,
          token: null,
          isAuthenticated: false,
          error: null
        });
        
        localStorage.removeItem('auth_token');
      },
      
      refreshToken: async () => {
        const { token } = get();
        if (!token) return;
        
        try {
          const response = await fetch('/api/auth/refresh', {
            method: 'POST',
            headers: {
              'Authorization': `Bearer ${token}`,
              'Content-Type': 'application/json'
            }
          });
          
          if (response.ok) {
            const data = await response.json();
            set({ token: data.token });
            localStorage.setItem('auth_token', data.token);
          } else {
            get().logout();
          }
        } catch (error) {
          get().logout();
        }
      },
      
      clearError: () => set({ error: null })
    }),
    {
      name: 'auth-storage',
      partialize: (state) => ({ 
        token: state.token,
        user: state.user,
        isAuthenticated: state.isAuthenticated
      })
    }
  )
);
```

### **Auth Actions**
```typescript
// stores/auth/authActions.ts
import { useAuthStore } from './authStore';

export const useAuthActions = () => {
  const { login, logout, refreshToken, clearError } = useAuthStore();
  
  return {
    login,
    logout,
    refreshToken,
    clearError,
    
    // Permission checks
    hasPermission: (permission: string) => {
      const { user } = useAuthStore.getState();
      return user?.permissions.includes(permission) ?? false;
    },
    
    hasRole: (role: string) => {
      const { user } = useAuthStore.getState();
      return user?.role === role;
    },
    
    // Auto-refresh token
    setupTokenRefresh: () => {
      const interval = setInterval(() => {
        const { token, isAuthenticated } = useAuthStore.getState();
        if (isAuthenticated && token) {
          refreshToken();
        }
      }, 14 * 60 * 1000); // Refresh every 14 minutes
      
      return () => clearInterval(interval);
    }
  };
};
```

## 🖥️ **System Store**

### **System Store Implementation**
```typescript
// stores/system/systemStore.ts
import { create } from 'zustand';

interface SystemStatus {
  mode: 'IDLE' | 'MOVE' | 'DOCK' | 'FAULT' | 'ESTOP';
  connection: 'ONLINE' | 'OFFLINE' | 'CONNECTING';
  battery: number;
  temperature: number;
  uptime: number;
  lastUpdate: string;
}

interface SystemState {
  // State
  status: SystemStatus;
  health: {
    cpu: number;
    memory: number;
    disk: number;
    network: number;
  };
  isLoading: boolean;
  error: string | null;
  
  // Actions
  updateStatus: (status: Partial<SystemStatus>) => void;
  updateHealth: (health: Partial<SystemState['health']>) => void;
  setLoading: (loading: boolean) => void;
  setError: (error: string | null) => void;
  fetchSystemStatus: () => Promise<void>;
}

export const useSystemStore = create<SystemState>((set, get) => ({
  // Initial state
  status: {
    mode: 'IDLE',
    connection: 'OFFLINE',
    battery: 0,
    temperature: 0,
    uptime: 0,
    lastUpdate: new Date().toISOString()
  },
  health: {
    cpu: 0,
    memory: 0,
    disk: 0,
    network: 0
  },
  isLoading: false,
  error: null,
  
  // Actions
  updateStatus: (status) => {
    set((state) => ({
      status: { ...state.status, ...status, lastUpdate: new Date().toISOString() }
    }));
  },
  
  updateHealth: (health) => {
    set((state) => ({
      health: { ...state.health, ...health }
    }));
  },
  
  setLoading: (loading) => set({ isLoading: loading }),
  
  setError: (error) => set({ error }),
  
  fetchSystemStatus: async () => {
    const { setLoading, setError, updateStatus, updateHealth } = get();
    
    setLoading(true);
    setError(null);
    
    try {
      const response = await fetch('/api/system/status', {
        headers: {
          'Authorization': `Bearer ${useAuthStore.getState().token}`
        }
      });
      
      if (!response.ok) {
        throw new Error('Failed to fetch system status');
      }
      
      const data = await response.json();
      
      updateStatus(data.status);
      updateHealth(data.health);
      
    } catch (error) {
      setError(error instanceof Error ? error.message : 'System status fetch failed');
    } finally {
      setLoading(false);
    }
  }
}));
```

## 🎮 **Control Store**

### **Control Store Implementation**
```typescript
// stores/control/controlStore.ts
import { create } from 'zustand';

interface ControlState {
  // State
  currentSpeed: number;
  targetSpeed: number;
  isMoving: boolean;
  movementDirection: 'forward' | 'backward' | 'left' | 'right' | null;
  emergencyStopActive: boolean;
  controlMode: 'MANUAL' | 'AUTO' | 'SEMI' | 'MAINT';
  isLoading: boolean;
  error: string | null;
  
  // Actions
  setSpeed: (speed: number) => void;
  setTargetSpeed: (speed: number) => void;
  move: (direction: 'forward' | 'backward' | 'left' | 'right') => Promise<void>;
  stop: () => Promise<void>;
  emergencyStop: () => Promise<void>;
  setControlMode: (mode: ControlState['controlMode']) => void;
  setLoading: (loading: boolean) => void;
  setError: (error: string | null) => void;
}

export const useControlStore = create<ControlState>((set, get) => ({
  // Initial state
  currentSpeed: 0,
  targetSpeed: 0,
  isMoving: false,
  movementDirection: null,
  emergencyStopActive: false,
  controlMode: 'MANUAL',
  isLoading: false,
  error: null,
  
  // Actions
  setSpeed: (speed) => {
    set({ currentSpeed: Math.max(0, Math.min(100, speed)) });
  },
  
  setTargetSpeed: (speed) => {
    set({ targetSpeed: Math.max(0, Math.min(100, speed)) });
  },
  
  move: async (direction) => {
    const { setLoading, setError, setTargetSpeed } = get();
    const { targetSpeed } = get();
    
    setLoading(true);
    setError(null);
    
    try {
      const response = await fetch('/api/control/move', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${useAuthStore.getState().token}`
        },
        body: JSON.stringify({
          direction,
          speed: targetSpeed
        })
      });
      
      if (!response.ok) {
        throw new Error('Move command failed');
      }
      
      set({
        isMoving: true,
        movementDirection: direction,
        emergencyStopActive: false
      });
      
    } catch (error) {
      setError(error instanceof Error ? error.message : 'Move command failed');
    } finally {
      setLoading(false);
    }
  },
  
  stop: async () => {
    const { setLoading, setError } = get();
    
    setLoading(true);
    setError(null);
    
    try {
      const response = await fetch('/api/control/stop', {
        method: 'POST',
        headers: {
          'Authorization': `Bearer ${useAuthStore.getState().token}`
        }
      });
      
      if (!response.ok) {
        throw new Error('Stop command failed');
      }
      
      set({
        isMoving: false,
        movementDirection: null,
        currentSpeed: 0
      });
      
    } catch (error) {
      setError(error instanceof Error ? error.message : 'Stop command failed');
    } finally {
      setLoading(false);
    }
  },
  
  emergencyStop: async () => {
    const { setLoading, setError } = get();
    
    setLoading(true);
    setError(null);
    
    try {
      const response = await fetch('/api/control/emergency-stop', {
        method: 'POST',
        headers: {
          'Authorization': `Bearer ${useAuthStore.getState().token}`
        }
      });
      
      if (!response.ok) {
        throw new Error('Emergency stop failed');
      }
      
      set({
        isMoving: false,
        movementDirection: null,
        currentSpeed: 0,
        emergencyStopActive: true
      });
      
      // Add emergency alert
      useAlertStore.getState().addAlert({
        level: 'critical',
        title: 'Emergency Stop Activated',
        message: 'Robot has been stopped due to emergency stop command',
        timestamp: new Date().toISOString()
      });
      
    } catch (error) {
      setError(error instanceof Error ? error.message : 'Emergency stop failed');
    } finally {
      setLoading(false);
    }
  },
  
  setControlMode: (mode) => {
    set({ controlMode: mode });
  },
  
  setLoading: (loading) => set({ isLoading: loading }),
  
  setError: (error) => set({ error })
}));
```

## 📊 **Telemetry Store**

### **Telemetry Store Implementation**
```typescript
// stores/telemetry/telemetryStore.ts
import { create } from 'zustand';

interface TelemetryData {
  timestamp: string;
  battery: number;
  position: { x: number; y: number; z: number };
  speed: number;
  temperature: number;
  voltage: number;
  current: number;
  power: number;
}

interface TelemetryState {
  // State
  currentData: TelemetryData | null;
  historicalData: TelemetryData[];
  maxDataPoints: number;
  isConnected: boolean;
  lastUpdate: string;
  error: string | null;
  
  // Actions
  updateTelemetry: (data: TelemetryData) => void;
  addHistoricalData: (data: TelemetryData) => void;
  clearHistoricalData: () => void;
  setConnected: (connected: boolean) => void;
  setError: (error: string | null) => void;
  fetchHistoricalData: (startTime: string, endTime: string) => Promise<void>;
}

export const useTelemetryStore = create<TelemetryState>((set, get) => ({
  // Initial state
  currentData: null,
  historicalData: [],
  maxDataPoints: 1000, // Keep last 1000 data points
  isConnected: false,
  lastUpdate: new Date().toISOString(),
  error: null,
  
  // Actions
  updateTelemetry: (data) => {
    set({
      currentData: data,
      lastUpdate: new Date().toISOString()
    });
    
    // Add to historical data
    get().addHistoricalData(data);
  },
  
  addHistoricalData: (data) => {
    set((state) => {
      const newHistoricalData = [...state.historicalData, data];
      
      // Keep only the last maxDataPoints
      if (newHistoricalData.length > state.maxDataPoints) {
        newHistoricalData.splice(0, newHistoricalData.length - state.maxDataPoints);
      }
      
      return { historicalData: newHistoricalData };
    });
  },
  
  clearHistoricalData: () => {
    set({ historicalData: [] });
  },
  
  setConnected: (connected) => {
    set({ isConnected: connected });
  },
  
  setError: (error) => {
    set({ error });
  },
  
  fetchHistoricalData: async (startTime: string, endTime: string) => {
    const { setError } = get();
    
    try {
      const response = await fetch(
        `/api/telemetry/history?start=${startTime}&end=${endTime}`,
        {
          headers: {
            'Authorization': `Bearer ${useAuthStore.getState().token}`
          }
        }
      );
      
      if (!response.ok) {
        throw new Error('Failed to fetch historical data');
      }
      
      const data = await response.json();
      set({ historicalData: data });
      
    } catch (error) {
      setError(error instanceof Error ? error.message : 'Historical data fetch failed');
    }
  }
}));
```

## 🚨 **Alert Store**

### **Alert Store Implementation**
```typescript
// stores/alerts/alertStore.ts
import { create } from 'zustand';

interface Alert {
  id: string;
  level: 'critical' | 'warning' | 'info' | 'success';
  title: string;
  message: string;
  timestamp: string;
  dismissed: boolean;
  autoDismiss?: boolean;
  dismissAfter?: number; // milliseconds
}

interface AlertState {
  // State
  alerts: Alert[];
  maxAlerts: number;
  
  // Actions
  addAlert: (alert: Omit<Alert, 'id' | 'timestamp' | 'dismissed'>) => void;
  dismissAlert: (id: string) => void;
  clearAlerts: () => void;
  clearAlertsByLevel: (level: Alert['level']) => void;
}

export const useAlertStore = create<AlertState>((set, get) => ({
  // Initial state
  alerts: [],
  maxAlerts: 50,
  
  // Actions
  addAlert: (alertData) => {
    const id = crypto.randomUUID();
    const timestamp = new Date().toISOString();
    
    const newAlert: Alert = {
      ...alertData,
      id,
      timestamp,
      dismissed: false
    };
    
    set((state) => {
      const newAlerts = [...state.alerts, newAlert];
      
      // Keep only the last maxAlerts
      if (newAlerts.length > state.maxAlerts) {
        newAlerts.splice(0, newAlerts.length - state.maxAlerts);
      }
      
      return { alerts: newAlerts };
    });
    
    // Auto-dismiss if configured
    if (newAlert.autoDismiss && newAlert.dismissAfter) {
      setTimeout(() => {
        get().dismissAlert(id);
      }, newAlert.dismissAfter);
    }
    
    // Play sound for critical alerts
    if (newAlert.level === 'critical') {
      playAlertSound();
    }
  },
  
  dismissAlert: (id) => {
    set((state) => ({
      alerts: state.alerts.map(alert =>
        alert.id === id ? { ...alert, dismissed: true } : alert
      )
    }));
  },
  
  clearAlerts: () => {
    set({ alerts: [] });
  },
  
  clearAlertsByLevel: (level) => {
    set((state) => ({
      alerts: state.alerts.filter(alert => alert.level !== level)
    }));
  }
}));

// Utility function for playing alert sounds
const playAlertSound = () => {
  try {
    const audio = new Audio('/sounds/alert-critical.mp3');
    audio.volume = 0.5;
    audio.play().catch(() => {
      // Fallback: use browser's built-in audio
      console.log('Critical alert triggered');
    });
  } catch (error) {
    console.warn('Could not play alert sound:', error);
  }
};
```

## 📡 **WebSocket Integration**

### **WebSocket Store**
```typescript
// stores/websocket/websocketStore.ts
import { create } from 'zustand';

interface WebSocketState {
  // State
  isConnected: boolean;
  isConnecting: boolean;
  error: string | null;
  reconnectAttempts: number;
  maxReconnectAttempts: number;
  
  // Actions
  connect: () => void;
  disconnect: () => void;
  sendMessage: (message: any) => void;
  setConnected: (connected: boolean) => void;
  setConnecting: (connecting: boolean) => void;
  setError: (error: string | null) => void;
}

export const useWebSocketStore = create<WebSocketState>((set, get) => {
  let ws: WebSocket | null = null;
  let reconnectTimeout: NodeJS.Timeout | null = null;
  
  const connect = () => {
    const { setConnecting, setError, setConnected, reconnectAttempts, maxReconnectAttempts } = get();
    
    if (ws?.readyState === WebSocket.OPEN) {
      return; // Already connected
    }
    
    setConnecting(true);
    setError(null);
    
    try {
      const token = useAuthStore.getState().token;
      if (!token) {
        throw new Error('No authentication token');
      }
      
      ws = new WebSocket(`ws://localhost:8000/ws?token=${token}`);
      
      ws.onopen = () => {
        set({
          isConnected: true,
          isConnecting: false,
          error: null,
          reconnectAttempts: 0
        });
        
        // Update telemetry connection status
        useTelemetryStore.getState().setConnected(true);
      };
      
      ws.onmessage = (event) => {
        try {
          const data = JSON.parse(event.data);
          handleWebSocketMessage(data);
        } catch (error) {
          console.error('Failed to parse WebSocket message:', error);
        }
      };
      
      ws.onclose = () => {
        set({
          isConnected: false,
          isConnecting: false
        });
        
        // Update telemetry connection status
        useTelemetryStore.getState().setConnected(false);
        
        // Attempt to reconnect
        const { reconnectAttempts, maxReconnectAttempts } = get();
        if (reconnectAttempts < maxReconnectAttempts) {
          const delay = Math.min(1000 * Math.pow(2, reconnectAttempts), 30000);
          reconnectTimeout = setTimeout(() => {
            set({ reconnectAttempts: reconnectAttempts + 1 });
            connect();
          }, delay);
        }
      };
      
      ws.onerror = (error) => {
        setError('WebSocket connection error');
        console.error('WebSocket error:', error);
      };
      
    } catch (error) {
      set({
        isConnecting: false,
        error: error instanceof Error ? error.message : 'WebSocket connection failed'
      });
    }
  };
  
  const disconnect = () => {
    if (reconnectTimeout) {
      clearTimeout(reconnectTimeout);
      reconnectTimeout = null;
    }
    
    if (ws) {
      ws.close();
      ws = null;
    }
    
    set({
      isConnected: false,
      isConnecting: false,
      reconnectAttempts: 0
    });
  };
  
  const sendMessage = (message: any) => {
    if (ws?.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify(message));
    } else {
      console.warn('WebSocket is not connected');
    }
  };
  
  return {
    // Initial state
    isConnected: false,
    isConnecting: false,
    error: null,
    reconnectAttempts: 0,
    maxReconnectAttempts: 5,
    
    // Actions
    connect,
    disconnect,
    sendMessage,
    setConnected: (connected) => set({ isConnected: connected }),
    setConnecting: (connecting) => set({ isConnecting: connecting }),
    setError: (error) => set({ error })
  };
});

// WebSocket message handler
const handleWebSocketMessage = (data: any) => {
  switch (data.type) {
    case 'telemetry:update':
      useTelemetryStore.getState().updateTelemetry(data.payload);
      break;
      
    case 'status:change':
      useSystemStore.getState().updateStatus(data.payload);
      break;
      
    case 'emergency:triggered':
      useControlStore.getState().emergencyStop();
      useAlertStore.getState().addAlert({
        level: 'critical',
        title: 'Emergency Stop Triggered',
        message: data.payload.reason,
        autoDismiss: false
      });
      break;
      
    case 'alert:new':
      useAlertStore.getState().addAlert({
        level: data.payload.level,
        title: data.payload.title,
        message: data.payload.message,
        autoDismiss: data.payload.level !== 'critical'
      });
      break;
      
    default:
      console.warn('Unknown WebSocket message type:', data.type);
  }
};
```

## 🔄 **Store Integration**

### **Root Store**
```typescript
// stores/index.ts
export { useAuthStore } from './auth/authStore';
export { useAuthActions } from './auth/authActions';
export { useSystemStore } from './system/systemStore';
export { useControlStore } from './control/controlStore';
export { useTelemetryStore } from './telemetry/telemetryStore';
export { useAlertStore } from './alerts/alertStore';
export { useWebSocketStore } from './websocket/websocketStore';

// Store initialization
export const initializeStores = () => {
  // Setup WebSocket connection
  const { connect, disconnect } = useWebSocketStore.getState();
  
  // Connect when authenticated
  const unsubscribe = useAuthStore.subscribe((state) => {
    if (state.isAuthenticated) {
      connect();
    } else {
      disconnect();
    }
  });
  
  // Setup token refresh
  const { setupTokenRefresh } = useAuthActions();
  const cleanup = setupTokenRefresh();
  
  return () => {
    unsubscribe();
    cleanup();
    disconnect();
  };
};
```

### **Store Hooks**
```typescript
// hooks/useStores.ts
import { useAuthStore, useSystemStore, useControlStore, useTelemetryStore, useAlertStore } from '../stores';

// Combined store hooks for common use cases
export const useDashboardData = () => {
  const auth = useAuthStore();
  const system = useSystemStore();
  const control = useControlStore();
  const telemetry = useTelemetryStore();
  const alerts = useAlertStore();
  
  return {
    auth,
    system,
    control,
    telemetry,
    alerts,
    
    // Computed values
    isOnline: system.status.connection === 'ONLINE',
    canControl: auth.isAuthenticated && system.status.mode !== 'ESTOP',
    hasCriticalAlerts: alerts.alerts.some(alert => 
      alert.level === 'critical' && !alert.dismissed
    )
  };
};

export const useControlPermissions = () => {
  const { hasPermission } = useAuthActions();
  
  return {
    canMove: hasPermission('control:manual'),
    canEmergencyStop: hasPermission('emergency:stop'),
    canConfigure: hasPermission('system:config'),
    canViewLogs: hasPermission('logs:view')
  };
};
```

## 🧪 **Testing Strategy**

### **Store Testing**
```typescript
// __tests__/stores/authStore.test.ts
import { renderHook, act } from '@testing-library/react';
import { useAuthStore } from '../../stores/auth/authStore';

describe('Auth Store', () => {
  beforeEach(() => {
    // Clear store state
    useAuthStore.setState({
      user: null,
      token: null,
      isAuthenticated: false,
      isLoading: false,
      error: null
    });
  });
  
  it('should handle login success', async () => {
    const { result } = renderHook(() => useAuthStore());
    
    // Mock successful login
    global.fetch = jest.fn().mockResolvedValueOnce({
      ok: true,
      json: async () => ({
        user: { id: '1', username: 'test', role: 'operator' },
        token: 'test-token'
      })
    });
    
    await act(async () => {
      await result.current.login('test', 'password');
    });
    
    expect(result.current.isAuthenticated).toBe(true);
    expect(result.current.user?.username).toBe('test');
    expect(result.current.token).toBe('test-token');
  });
  
  it('should handle login failure', async () => {
    const { result } = renderHook(() => useAuthStore());
    
    // Mock failed login
    global.fetch = jest.fn().mockResolvedValueOnce({
      ok: false
    });
    
    await act(async () => {
      await result.current.login('test', 'wrong-password');
    });
    
    expect(result.current.isAuthenticated).toBe(false);
    expect(result.current.error).toBe('Login failed');
  });
});
```

## 🔄 **Changelog**

**v1.0 (2024-12-19)**
- ✅ Created Zustand-based state management
- ✅ Implemented authentication store with persistence
- ✅ Created system status store
- ✅ Implemented control store with safety features
- ✅ Added real-time telemetry store
- ✅ Created alert management system
- ✅ Implemented WebSocket integration
- ✅ Added store testing strategy

---

**Next Steps:**
1. Implement remaining stores (missions, config)
2. Add store middleware for logging
3. Implement store persistence strategies
4. Add store performance optimization
5. Create store documentation
