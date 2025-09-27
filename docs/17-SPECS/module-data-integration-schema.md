# Module Data Integration Schema - OHT-50

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Mục tiêu:** Định nghĩa schema cho module data integration giữa firmware và frontend

---

## 🎯 **MODULE DATA ARCHITECTURE**

### **Data Flow:**
```
Firmware Modules → Backend API → Frontend State → UI Components
     ↓                ↓              ↓              ↓
  Raw Data      →  Processed    →  State      →  Display
  (RS485)       →  Data (JSON)  →  Management →  Components
```

---

## 📊 **MODULE DATA TYPES**

### **1. Travel Motor Module Data:**
```typescript
interface TravelMotorModuleData {
  module_id: string;
  timestamp: string;
  status: {
    state: 'idle' | 'moving' | 'stopping' | 'error' | 'maintenance';
    position: number;        // mm
    velocity: number;        // mm/s
    acceleration: number;    // mm/s²
    target_position: number; // mm
  };
  motor: {
    current: number;         // A
    voltage: number;         // V
    temperature: number;     // °C
    power: number;          // W
  };
  encoder: {
    position: number;        // pulses
    velocity: number;        // pulses/s
    direction: 'forward' | 'backward' | 'stopped';
  };
  safety: {
    e_stop_active: boolean;
    limit_switch_forward: boolean;
    limit_switch_backward: boolean;
    emergency_stop: boolean;
  };
  diagnostics: {
    error_code: number;
    warning_flags: number;
    uptime: number;          // seconds
    cycle_count: number;
  };
}
```

### **2. Power Module Data:**
```typescript
interface PowerModuleData {
  module_id: string;
  timestamp: string;
  status: {
    state: 'normal' | 'warning' | 'error' | 'maintenance';
    power_supply_status: 'ok' | 'low' | 'high' | 'fault';
  };
  power: {
    input_voltage: number;   // V
    input_current: number;   // A
    output_voltage: number;  // V
    output_current: number;  // A
    power_consumption: number; // W
    efficiency: number;      // %
  };
  battery: {
    voltage: number;         // V
    current: number;         // A
    capacity: number;        // Ah
    state_of_charge: number; // %
    temperature: number;     // °C
    health: number;          // %
  };
  charging: {
    charging_status: 'idle' | 'charging' | 'discharging' | 'fault';
    charge_current: number;  // A
    charge_voltage: number;  // V
    charge_power: number;    // W
  };
  diagnostics: {
    error_code: number;
    warning_flags: number;
    uptime: number;          // seconds
    cycle_count: number;
  };
}
```

### **3. Safety Module Data:**
```typescript
interface SafetyModuleData {
  module_id: string;
  timestamp: string;
  status: {
    state: 'normal' | 'warning' | 'error' | 'emergency';
    safety_system_status: 'active' | 'bypass' | 'fault';
  };
  e_stop: {
    e_stop_active: boolean;
    e_stop_source: 'hardware' | 'software' | 'network';
    e_stop_timestamp: string;
  };
  sensors: {
    obstacle_detection: boolean;
    proximity_sensors: {
      front: boolean;
      back: boolean;
      left: boolean;
      right: boolean;
    };
    limit_switches: {
      forward: boolean;
      backward: boolean;
    };
  };
  interlocks: {
    door_interlock: boolean;
    safety_curtain: boolean;
    light_curtain: boolean;
    emergency_stop: boolean;
  };
  diagnostics: {
    error_code: number;
    warning_flags: number;
    uptime: number;          // seconds
    test_results: {
      e_stop_test: boolean;
      sensor_test: boolean;
      interlock_test: boolean;
    };
  };
}
```

---

## 🔌 **API ENDPOINTS**

### **REST API Endpoints:**

#### **1. Module Status:**
```http
GET /api/v1/modules/{module_id}/status
GET /api/v1/modules/status/all
```

#### **2. Module Configuration:**
```http
GET /api/v1/modules/{module_id}/config
PUT /api/v1/modules/{module_id}/config
POST /api/v1/modules/{module_id}/config/validate
```

#### **3. Module Commands:**
```http
POST /api/v1/modules/{module_id}/commands
POST /api/v1/modules/{module_id}/emergency_stop
POST /api/v1/modules/{module_id}/reset
```

#### **4. Module History:**
```http
GET /api/v1/modules/{module_id}/history?from={timestamp}&to={timestamp}
GET /api/v1/modules/{module_id}/alerts?from={timestamp}&to={timestamp}
```

### **WebSocket Events:**

#### **1. Real-time Module Data:**
```typescript
// Event: module_data_update
{
  event: 'module_data_update',
  data: {
    module_id: string;
    module_type: 'travel_motor' | 'power' | 'safety';
    data: TravelMotorModuleData | PowerModuleData | SafetyModuleData;
    timestamp: string;
  }
}
```

#### **2. Module Status Changes:**
```typescript
// Event: module_status_change
{
  event: 'module_status_change',
  data: {
    module_id: string;
    old_status: string;
    new_status: string;
    timestamp: string;
    reason: string;
  }
}
```

#### **3. Module Alerts:**
```typescript
// Event: module_alert
{
  event: 'module_alert',
  data: {
    module_id: string;
    alert_type: 'error' | 'warning' | 'info';
    message: string;
    timestamp: string;
    severity: 'low' | 'medium' | 'high' | 'critical';
  }
}
```

---

## 🎨 **FRONTEND STATE MANAGEMENT**

### **Redux State Structure:**
```typescript
interface ModuleState {
  modules: {
    [moduleId: string]: {
      data: ModuleData;
      status: 'connected' | 'disconnected' | 'error';
      lastUpdate: string;
      error?: string;
    };
  };
  ui: {
    selectedModule: string | null;
    filter: {
      moduleType: string | null;
      status: string | null;
    };
    view: 'dashboard' | 'detail' | 'history';
  };
}
```

### **State Actions:**
```typescript
// Actions
const moduleActions = {
  updateModuleData: (moduleId: string, data: ModuleData) => ({ type: 'UPDATE_MODULE_DATA', payload: { moduleId, data } }),
  setModuleStatus: (moduleId: string, status: string) => ({ type: 'SET_MODULE_STATUS', payload: { moduleId, status } }),
  selectModule: (moduleId: string) => ({ type: 'SELECT_MODULE', payload: { moduleId } }),
  setFilter: (filter: FilterOptions) => ({ type: 'SET_FILTER', payload: { filter } }),
  setView: (view: ViewType) => ({ type: 'SET_VIEW', payload: { view } }),
};
```

---

## 🔧 **IMPLEMENTATION GUIDELINES**

### **1. Backend Implementation:**
- Use MessagePack for high-frequency data
- Implement caching with Redis
- Add rate limiting (1000 req/min per user)
- Implement audit logging

### **2. Frontend Implementation:**
- Use WebSocket for real-time updates
- Implement optimistic updates
- Add error handling and retry logic
- Use TypeScript for type safety

### **3. Performance Optimization:**
- Batch WebSocket updates (max 10 updates/second)
- Implement data compression
- Use virtual scrolling for large datasets
- Cache frequently accessed data

---

## 📋 **TESTING REQUIREMENTS**

### **1. Unit Tests:**
- Module data parsing
- State management actions
- API endpoint responses

### **2. Integration Tests:**
- End-to-end data flow
- WebSocket connection handling
- Error scenarios

### **3. Performance Tests:**
- High-frequency data updates
- Large dataset handling
- Memory usage optimization

---

## 🚨 **ERROR HANDLING**

### **1. Network Errors:**
- Retry with exponential backoff
- Fallback to cached data
- Show connection status

### **2. Data Errors:**
- Validate data schema
- Log invalid data
- Show error notifications

### **3. Module Errors:**
- Handle module disconnection
- Show error states
- Provide recovery options

---

**Changelog v1.0:**
- ✅ Created comprehensive module data schema
- ✅ Defined API endpoints for module data
- ✅ Specified WebSocket events for real-time updates
- ✅ Designed frontend state management structure
- ✅ Added implementation guidelines and testing requirements
