# 🔄 REQ_FE_State_Management - Quản lý State Frontend

**Mục đích:** Định nghĩa yêu cầu quản lý state cho OHT-50 Master Module Dashboard

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
1. **Real-time State Management** - Quản lý state thời gian thực cho 11 modules
2. **Predictable State Updates** - Cập nhật state có thể dự đoán được
3. **Performance Optimization** - Tối ưu hiệu suất state management
4. **Safety-Critical State** - Ưu tiên state an toàn
5. **Scalable Architecture** - Kiến trúc có thể mở rộng

---

## 🏗️ **State Architecture:**

### **1. State Structure:**
```
┌─────────────────────────────────────┐
│         Global State                │
│  ┌─────────────┬─────────────────┐  │
│  │ System      │  User           │  │
│  │ State       │  State          │  │
│  ├─────────────┼─────────────────┤  │
│  │ Module      │  Configuration  │  │
│  │ States      │  State          │  │
│  ├─────────────┼─────────────────┤  │
│  │ Safety      │  Communication  │  │
│  │ State       │  State          │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

### **2. State Categories:**

#### **2.1 System State:**
```typescript
interface SystemState {
  status: 'online' | 'offline' | 'error' | 'maintenance';
  version: string;
  uptime: number;
  lastUpdate: Date;
  errors: Error[];
  warnings: Warning[];
}
```

#### **2.2 Module States:**
```typescript
interface ModuleStates {
  masterControl: MasterControlState;
  power: PowerState;
  safety: SafetyState;
  travelMotor: TravelMotorState;
  dockLocation: DockLocationState;
  lifterMotor?: LifterMotorState;
  cargoDoor?: CargoDoorState;
  safetyExtended?: SafetyExtendedState;
  rfidReader?: RFIDReaderState;
  camera?: CameraState;
  environmental?: EnvironmentalState;
}
```

#### **2.3 User State:**
```typescript
interface UserState {
  isAuthenticated: boolean;
  user: User | null;
  permissions: Permission[];
  preferences: UserPreferences;
  session: Session;
}
```

#### **2.4 Configuration State:**
```typescript
interface ConfigurationState {
  systemConfig: SystemConfiguration;
  moduleConfigs: Record<string, ModuleConfiguration>;
  uiConfig: UIConfiguration;
  theme: Theme;
  language: string;
}
```

---

## 📊 **Module-Specific States:**

### **1. Safety Module State:**
```typescript
interface SafetyState {
  // Analog sensors (4 zones)
  distanceSensors: {
    zone1: { distance: number; threshold: number; status: 'safe' | 'warning' | 'danger' };
    zone2: { distance: number; threshold: number; status: 'safe' | 'warning' | 'danger' };
    zone3: { distance: number; threshold: number; status: 'safe' | 'warning' | 'danger' };
    zone4: { distance: number; threshold: number; status: 'safe' | 'warning' | 'danger' };
  };
  
  // Digital sensors (4 proximity sensors)
  proximitySensors: {
    sensor1: { isActive: boolean; distance?: number };
    sensor2: { isActive: boolean; distance?: number };
    sensor3: { isActive: boolean; distance?: number };
    sensor4: { isActive: boolean; distance?: number };
  };
  
  // Relay outputs (4 relays)
  relays: {
    relay1: { isActive: boolean; function: 'emergency_brake' };
    relay2: { isActive: boolean; function: 'warning_lights' };
    relay3: { isActive: boolean; function: 'alarm' };
    relay4: { isActive: boolean; function: 'e_stop' };
  };
  
  // Safety system status
  systemStatus: 'normal' | 'warning' | 'emergency' | 'maintenance';
  lastSafetyEvent?: SafetyEvent;
  safetyViolations: SafetyViolation[];
}
```

### **2. Travel Motor State:**
```typescript
interface TravelMotorState {
  // Motor 1
  motor1: {
    speed: number;
    direction: 'forward' | 'reverse' | 'stop';
    isEnabled: boolean;
    isBrakeApplied: boolean;
    current: number;
    temperature: number;
    status: 'idle' | 'running' | 'error' | 'maintenance';
  };
  
  // Motor 2
  motor2: {
    speed: number;
    direction: 'forward' | 'reverse' | 'stop';
    isEnabled: boolean;
    isBrakeApplied: boolean;
    current: number;
    temperature: number;
    status: 'idle' | 'running' | 'error' | 'maintenance';
  };
  
  // Digital Inputs (8 channels)
  digitalInputs: {
    [key: number]: { status: boolean; label?: string; function?: string };
  };
  
  // Digital Outputs (8 channels)
  digitalOutputs: {
    [key: number]: { status: boolean; label?: string; function?: string };
  };
  
  // System status
  systemStatus: 'idle' | 'moving' | 'stopping' | 'error' | 'maintenance';
  movementMode: 'manual' | 'automatic' | 'emergency';
}
```

### **3. Power Module State:**
```typescript
interface PowerState {
  // Battery information
  battery: {
    level: number;
    voltage: number;
    current: number;
    temperature: number;
    timeRemaining: number;
    cycleCount: number;
    health: number;
  };
  
  // Charger information
  charger: {
    isConnected: boolean;
    isCharging: boolean;
    mode: 'fast' | 'normal' | 'trickle';
    inputVoltage: number;
    outputVoltage: number;
    efficiency: number;
    temperature: number;
  };
  
  // Power consumption
  consumption: {
    current: number;
    average: number;
    peak: number;
    total: number;
  };
  
  // System status
  systemStatus: 'normal' | 'low_battery' | 'charging' | 'error' | 'maintenance';
  lastChargeTime?: Date;
  estimatedRuntime: number;
}
```

### **4. Dock & Location State:**
```typescript
interface DockLocationState {
  // Position data
  position: {
    x: number;
    y: number;
    z: number;
    accuracy: number;
    timestamp: Date;
  };
  
  // Orientation data
  orientation: {
    roll: number;
    pitch: number;
    yaw: number;
    accuracy: number;
  };
  
  // Magnetic sensors
  magneticSensors: {
    [key: number]: { isDetected: boolean; strength?: number };
  };
  
  // Limit switches
  limitSwitches: {
    [key: number]: { isTriggered: boolean; label?: string };
  };
  
  // System status
  systemStatus: 'searching' | 'docking' | 'docked' | 'undocking' | 'error';
  dockStatus: 'available' | 'occupied' | 'maintenance';
  locationAccuracy: 'high' | 'medium' | 'low';
}
```

---

## 🔄 **State Management Patterns:**

### **1. Redux Toolkit Pattern:**
```typescript
// Store configuration
import { configureStore } from '@reduxjs/toolkit';

const store = configureStore({
  reducer: {
    system: systemReducer,
    modules: modulesReducer,
    user: userReducer,
    config: configReducer,
    safety: safetyReducer,
  },
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware({
      serializableCheck: {
        ignoredActions: ['persist/PERSIST'],
      },
    }),
});

// Slice example for Safety Module
import { createSlice, PayloadAction } from '@reduxjs/toolkit';

const safetySlice = createSlice({
  name: 'safety',
  initialState: initialSafetyState,
  reducers: {
    updateDistanceSensor: (state, action: PayloadAction<{
      zone: number;
      distance: number;
      threshold: number;
    }>) => {
      const { zone, distance, threshold } = action.payload;
      state.distanceSensors[`zone${zone}`] = {
        distance,
        threshold,
        status: distance < threshold ? 'danger' : distance < threshold * 2 ? 'warning' : 'safe'
      };
    },
    updateProximitySensor: (state, action: PayloadAction<{
      sensorId: number;
      isActive: boolean;
      distance?: number;
    }>) => {
      const { sensorId, isActive, distance } = action.payload;
      state.proximitySensors[`sensor${sensorId}`] = { isActive, distance };
    },
    toggleRelay: (state, action: PayloadAction<{
      relayId: number;
      isActive: boolean;
    }>) => {
      const { relayId, isActive } = action.payload;
      state.relays[`relay${relayId}`].isActive = isActive;
    },
  },
});
```

### **2. Zustand Pattern (Alternative):**
```typescript
import create from 'zustand';
import { subscribeWithSelector } from 'zustand/middleware';

interface SafetyStore {
  // State
  distanceSensors: Record<string, DistanceSensorData>;
  proximitySensors: Record<string, ProximitySensorData>;
  relays: Record<string, RelayData>;
  systemStatus: SafetySystemStatus;
  
  // Actions
  updateDistanceSensor: (zone: number, data: DistanceSensorData) => void;
  updateProximitySensor: (sensorId: number, data: ProximitySensorData) => void;
  toggleRelay: (relayId: number, isActive: boolean) => void;
  setSystemStatus: (status: SafetySystemStatus) => void;
}

const useSafetyStore = create<SafetyStore>()(
  subscribeWithSelector((set, get) => ({
    // Initial state
    distanceSensors: {},
    proximitySensors: {},
    relays: {},
    systemStatus: 'normal',
    
    // Actions
    updateDistanceSensor: (zone, data) =>
      set((state) => ({
        distanceSensors: {
          ...state.distanceSensors,
          [`zone${zone}`]: data,
        },
      })),
      
    updateProximitySensor: (sensorId, data) =>
      set((state) => ({
        proximitySensors: {
          ...state.proximitySensors,
          [`sensor${sensorId}`]: data,
        },
      })),
      
    toggleRelay: (relayId, isActive) =>
      set((state) => ({
        relays: {
          ...state.relays,
          [`relay${relayId}`]: {
            ...state.relays[`relay${relayId}`],
            isActive,
          },
        },
      })),
      
    setSystemStatus: (status) => set({ systemStatus: status }),
  }))
);
```

---

## 🔄 **State Synchronization:**

### **1. WebSocket Integration:**
```typescript
// WebSocket service for real-time updates
class WebSocketService {
  private ws: WebSocket | null = null;
  private store: Store;
  
  constructor(store: Store) {
    this.store = store;
  }
  
  connect(url: string) {
    this.ws = new WebSocket(url);
    
    this.ws.onmessage = (event) => {
      const data = JSON.parse(event.data);
      this.handleMessage(data);
    };
    
    this.ws.onerror = (error) => {
      console.error('WebSocket error:', error);
      this.store.dispatch(setConnectionStatus('error'));
    };
  }
  
  private handleMessage(data: any) {
    switch (data.type) {
      case 'SAFETY_UPDATE':
        this.store.dispatch(updateSafetyState(data.payload));
        break;
      case 'MOTOR_UPDATE':
        this.store.dispatch(updateMotorState(data.payload));
        break;
      case 'POWER_UPDATE':
        this.store.dispatch(updatePowerState(data.payload));
        break;
      case 'LOCATION_UPDATE':
        this.store.dispatch(updateLocationState(data.payload));
        break;
      default:
        console.warn('Unknown message type:', data.type);
    }
  }
}
```

### **2. Optimistic Updates:**
```typescript
// Optimistic updates for better UX
const optimisticRelayToggle = (relayId: number, isActive: boolean) => {
  return async (dispatch: Dispatch, getState: () => RootState) => {
    // Optimistic update
    dispatch(toggleRelay({ relayId, isActive }));
    
    try {
      // API call
      await api.toggleRelay(relayId, isActive);
      
      // Success - state is already updated
    } catch (error) {
      // Revert on error
      dispatch(toggleRelay({ relayId, isActive: !isActive }));
      dispatch(showError('Failed to toggle relay'));
    }
  };
};
```

---

## 🚨 **Safety-Critical State Management:**

### **1. Safety State Priority:**
```typescript
// Safety state takes priority over other states
const safetyMiddleware = (store: Store) => (next: Dispatch) => (action: AnyAction) => {
  const result = next(action);
  
  // Check safety state after every action
  const safetyState = store.getState().safety;
  
  if (safetyState.systemStatus === 'emergency') {
    // Emergency mode - disable non-safety actions
    store.dispatch(setEmergencyMode(true));
  }
  
  return result;
};
```

### **2. State Validation:**
```typescript
// Validate state changes for safety
const validateSafetyState = (currentState: SafetyState, newState: Partial<SafetyState>) => {
  const violations: string[] = [];
  
  // Check distance sensors
  Object.entries(newState.distanceSensors || {}).forEach(([zone, data]) => {
    if (data.distance < data.threshold && data.status !== 'danger') {
      violations.push(`Zone ${zone} distance below threshold`);
    }
  });
  
  // Check proximity sensors
  Object.entries(newState.proximitySensors || {}).forEach(([sensor, data]) => {
    if (data.isActive && data.distance && data.distance < 50) {
      violations.push(`Proximity sensor ${sensor} too close`);
    }
  });
  
  return violations;
};
```

---

## 📊 **State Performance Optimization:**

### **1. Selective Updates:**
```typescript
// Only update changed parts of state
const selectiveUpdate = (currentState: any, updates: any) => {
  const changed = {};
  
  Object.keys(updates).forEach(key => {
    if (JSON.stringify(currentState[key]) !== JSON.stringify(updates[key])) {
      changed[key] = updates[key];
    }
  });
  
  return Object.keys(changed).length > 0 ? changed : null;
};
```

### **2. State Memoization:**
```typescript
// Memoize expensive state computations
const useMemoizedState = (state: any, computeFn: (state: any) => any) => {
  return useMemo(() => computeFn(state), [state]);
};

// Usage
const safetySummary = useMemoizedState(safetyState, (state) => ({
  totalViolations: Object.values(state.distanceSensors)
    .filter(sensor => sensor.status === 'danger').length,
  systemStatus: state.systemStatus,
}));
```

### **3. State Persistence:**
```typescript
// Persist important state to localStorage
import { persistReducer } from 'redux-persist';
import storage from 'redux-persist/lib/storage';

const persistConfig = {
  key: 'oht50-state',
  storage,
  whitelist: ['user', 'config'], // Only persist user and config
  blacklist: ['safety', 'modules'], // Don't persist real-time data
};

const persistedReducer = persistReducer(persistConfig, rootReducer);
```

---

## 🔍 **State Debugging & Monitoring:**

### **1. State Logging:**
```typescript
// Log state changes for debugging
const loggingMiddleware = (store: Store) => (next: Dispatch) => (action: AnyAction) => {
  console.group(`Action: ${action.type}`);
  console.log('Previous State:', store.getState());
  console.log('Action:', action);
  
  const result = next(action);
  
  console.log('Next State:', store.getState());
  console.groupEnd();
  
  return result;
};
```

### **2. State Analytics:**
```typescript
// Track state changes for analytics
const analyticsMiddleware = (store: Store) => (next: Dispatch) => (action: AnyAction) => {
  const result = next(action);
  
  // Track safety events
  if (action.type === 'safety/updateDistanceSensor') {
    const { zone, distance, threshold } = action.payload;
    if (distance < threshold) {
      analytics.track('safety_violation', {
        zone,
        distance,
        threshold,
        timestamp: new Date().toISOString(),
      });
    }
  }
  
  return result;
};
```

---

## 🎯 **Success Criteria:**

### **1. Performance:**
- ✅ State update time < 16ms
- ✅ Memory usage < 100MB
- ✅ Bundle size < 200KB for state management

### **2. Reliability:**
- ✅ 99.9% state consistency
- ✅ Zero state corruption
- ✅ Automatic state recovery

### **3. Safety:**
- ✅ Safety state priority enforcement
- ✅ Real-time safety validation
- ✅ Emergency state handling

### **4. Developer Experience:**
- ✅ Type-safe state management
- ✅ Comprehensive debugging tools
- ✅ Clear state documentation

---

**Changelog v2.0:**
- ✅ Updated theo System Architecture v2.0
- ✅ Added Safety Module state với 4 analog + 4 digital sensors
- ✅ Added comprehensive state management patterns
- ✅ Added safety-critical state handling
- ✅ Added performance optimization strategies
- ✅ Added debugging and monitoring tools
