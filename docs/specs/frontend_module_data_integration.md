# Frontend Module Data Integration Plan - OHT-50

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Mục tiêu:** Frontend integration plan cho module data display và management

---

## 🎯 **FRONTEND INTEGRATION STRATEGY**

### **Integration Approach:**
```
Backend API → Frontend Services → State Management → UI Components
     ↓              ↓                ↓                ↓
  REST/WS      →  API Client    →  Redux Store   →  React Components
  Data         →  Services      →  State         →  User Interface
```

---

## 🔌 **API CLIENT IMPLEMENTATION**

### **Module Data API Client:**
```typescript
// services/moduleDataApi.ts
import axios from 'axios';
import { WebSocketClient } from './websocketClient';

export class ModuleDataApiClient {
  private baseURL: string;
  private wsClient: WebSocketClient;
  
  constructor(baseURL: string) {
    this.baseURL = baseURL;
    this.wsClient = new WebSocketClient(`${baseURL.replace('http', 'ws')}/ws/modules`);
  }
  
  // REST API Methods
  async getModuleStatus(moduleId: string): Promise<ModuleData> {
    const response = await axios.get(`${this.baseURL}/api/v1/modules/${moduleId}/status`);
    return response.data.data;
  }
  
  async getAllModulesStatus(): Promise<Record<string, ModuleData>> {
    const response = await axios.get(`${this.baseURL}/api/v1/modules/status/all`);
    return response.data.data;
  }
  
  async getModuleConfig(moduleId: string): Promise<ModuleConfig> {
    const response = await axios.get(`${this.baseURL}/api/v1/modules/${moduleId}/config`);
    return response.data.data;
  }
  
  async updateModuleConfig(moduleId: string, config: ModuleConfig): Promise<void> {
    await axios.put(`${this.baseURL}/api/v1/modules/${moduleId}/config`, config);
  }
  
  async executeModuleCommand(moduleId: string, command: ModuleCommand): Promise<CommandResult> {
    const response = await axios.post(`${this.baseURL}/api/v1/modules/${moduleId}/commands`, command);
    return response.data.data;
  }
  
  async emergencyStopModule(moduleId: string): Promise<void> {
    await axios.post(`${this.baseURL}/api/v1/modules/${moduleId}/emergency_stop`);
  }
  
  // WebSocket Methods
  subscribeToModule(moduleId: string, callback: (data: ModuleData) => void): void {
    this.wsClient.subscribe(`module_data_update:${moduleId}`, callback);
  }
  
  unsubscribeFromModule(moduleId: string): void {
    this.wsClient.unsubscribe(`module_data_update:${moduleId}`);
  }
  
  subscribeToStatusChanges(callback: (change: StatusChange) => void): void {
    this.wsClient.subscribe('module_status_change', callback);
  }
  
  subscribeToAlerts(callback: (alert: ModuleAlert) => void): void {
    this.wsClient.subscribe('module_alert', callback);
  }
}
```

### **WebSocket Client:**
```typescript
// services/websocketClient.ts
export class WebSocketClient {
  private ws: WebSocket | null = null;
  private subscriptions: Map<string, Function[]> = new Map();
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 5;
  
  constructor(private url: string) {
    this.connect();
  }
  
  private connect(): void {
    try {
      this.ws = new WebSocket(this.url);
      
      this.ws.onopen = () => {
        console.log('WebSocket connected');
        this.reconnectAttempts = 0;
      };
      
      this.ws.onmessage = (event) => {
        const message = JSON.parse(event.data);
        this.handleMessage(message);
      };
      
      this.ws.onclose = () => {
        console.log('WebSocket disconnected');
        this.handleReconnect();
      };
      
      this.ws.onerror = (error) => {
        console.error('WebSocket error:', error);
      };
    } catch (error) {
      console.error('Failed to connect WebSocket:', error);
      this.handleReconnect();
    }
  }
  
  private handleMessage(message: any): void {
    const { event, data } = message;
    const callbacks = this.subscriptions.get(event) || [];
    callbacks.forEach(callback => callback(data));
  }
  
  private handleReconnect(): void {
    if (this.reconnectAttempts < this.maxReconnectAttempts) {
      this.reconnectAttempts++;
      const delay = Math.pow(2, this.reconnectAttempts) * 1000; // Exponential backoff
      setTimeout(() => this.connect(), delay);
    }
  }
  
  subscribe(event: string, callback: Function): void {
    if (!this.subscriptions.has(event)) {
      this.subscriptions.set(event, []);
    }
    this.subscriptions.get(event)!.push(callback);
  }
  
  unsubscribe(event: string, callback?: Function): void {
    if (callback) {
      const callbacks = this.subscriptions.get(event) || [];
      const index = callbacks.indexOf(callback);
      if (index > -1) {
        callbacks.splice(index, 1);
      }
    } else {
      this.subscriptions.delete(event);
    }
  }
  
  send(data: any): void {
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(JSON.stringify(data));
    }
  }
}
```

---

## 🗄️ **STATE MANAGEMENT**

### **Redux Store Structure:**
```typescript
// store/moduleDataSlice.ts
import { createSlice, createAsyncThunk, PayloadAction } from '@reduxjs/toolkit';
import { ModuleDataApiClient } from '../services/moduleDataApi';

interface ModuleDataState {
  modules: Record<string, ModuleData>;
  status: 'idle' | 'loading' | 'succeeded' | 'failed';
  error: string | null;
  selectedModule: string | null;
  filter: {
    moduleType: string | null;
    status: string | null;
  };
  view: 'dashboard' | 'detail' | 'history';
  lastUpdate: string | null;
}

const initialState: ModuleDataState = {
  modules: {},
  status: 'idle',
  error: null,
  selectedModule: null,
  filter: {
    moduleType: null,
    status: null
  },
  view: 'dashboard',
  lastUpdate: null
};

// Async Thunks
export const fetchModuleStatus = createAsyncThunk(
  'moduleData/fetchModuleStatus',
  async (moduleId: string, { rejectWithValue }) => {
    try {
      const apiClient = new ModuleDataApiClient(process.env.REACT_APP_API_URL!);
      return await apiClient.getModuleStatus(moduleId);
    } catch (error) {
      return rejectWithValue(error.message);
    }
  }
);

export const fetchAllModulesStatus = createAsyncThunk(
  'moduleData/fetchAllModulesStatus',
  async (_, { rejectWithValue }) => {
    try {
      const apiClient = new ModuleDataApiClient(process.env.REACT_APP_API_URL!);
      return await apiClient.getAllModulesStatus();
    } catch (error) {
      return rejectWithValue(error.message);
    }
  }
);

export const updateModuleConfig = createAsyncThunk(
  'moduleData/updateModuleConfig',
  async ({ moduleId, config }: { moduleId: string; config: ModuleConfig }, { rejectWithValue }) => {
    try {
      const apiClient = new ModuleDataApiClient(process.env.REACT_APP_API_URL!);
      await apiClient.updateModuleConfig(moduleId, config);
      return { moduleId, config };
    } catch (error) {
      return rejectWithValue(error.message);
    }
  }
);

export const executeModuleCommand = createAsyncThunk(
  'moduleData/executeModuleCommand',
  async ({ moduleId, command }: { moduleId: string; command: ModuleCommand }, { rejectWithValue }) => {
    try {
      const apiClient = new ModuleDataApiClient(process.env.REACT_APP_API_URL!);
      return await apiClient.executeModuleCommand(moduleId, command);
    } catch (error) {
      return rejectWithValue(error.message);
    }
  }
);

// Slice
const moduleDataSlice = createSlice({
  name: 'moduleData',
  initialState,
  reducers: {
    updateModuleData: (state, action: PayloadAction<{ moduleId: string; data: ModuleData }>) => {
      const { moduleId, data } = action.payload;
      state.modules[moduleId] = data;
      state.lastUpdate = new Date().toISOString();
    },
    setModuleStatus: (state, action: PayloadAction<{ moduleId: string; status: string }>) => {
      const { moduleId, status } = action.payload;
      if (state.modules[moduleId]) {
        state.modules[moduleId].status.state = status;
      }
    },
    selectModule: (state, action: PayloadAction<string>) => {
      state.selectedModule = action.payload;
    },
    setFilter: (state, action: PayloadAction<{ moduleType?: string; status?: string }>) => {
      state.filter = { ...state.filter, ...action.payload };
    },
    setView: (state, action: PayloadAction<'dashboard' | 'detail' | 'history'>) => {
      state.view = action.payload;
    },
    clearError: (state) => {
      state.error = null;
    }
  },
  extraReducers: (builder) => {
    builder
      .addCase(fetchModuleStatus.pending, (state) => {
        state.status = 'loading';
      })
      .addCase(fetchModuleStatus.fulfilled, (state, action) => {
        state.status = 'succeeded';
        state.modules[action.payload.module_id] = action.payload;
      })
      .addCase(fetchModuleStatus.rejected, (state, action) => {
        state.status = 'failed';
        state.error = action.payload as string;
      })
      .addCase(fetchAllModulesStatus.fulfilled, (state, action) => {
        state.modules = action.payload;
        state.lastUpdate = new Date().toISOString();
      })
      .addCase(updateModuleConfig.fulfilled, (state, action) => {
        const { moduleId, config } = action.payload;
        if (state.modules[moduleId]) {
          state.modules[moduleId].config = config;
        }
      });
  }
});

export const {
  updateModuleData,
  setModuleStatus,
  selectModule,
  setFilter,
  setView,
  clearError
} = moduleDataSlice.actions;

export default moduleDataSlice.reducer;
```

### **WebSocket Middleware:**
```typescript
// middleware/websocketMiddleware.ts
import { Middleware } from '@reduxjs/toolkit';
import { updateModuleData, setModuleStatus } from '../store/moduleDataSlice';

export const websocketMiddleware: Middleware = (store) => (next) => (action) => {
  if (action.type === 'websocket/connect') {
    const wsClient = new WebSocketClient(action.payload.url);
    
    // Subscribe to module data updates
    wsClient.subscribe('module_data_update', (data: any) => {
      store.dispatch(updateModuleData({
        moduleId: data.module_id,
        data: data.data
      }));
    });
    
    // Subscribe to status changes
    wsClient.subscribe('module_status_change', (data: any) => {
      store.dispatch(setModuleStatus({
        moduleId: data.module_id,
        status: data.new_status
      }));
    });
    
    // Subscribe to alerts
    wsClient.subscribe('module_alert', (data: any) => {
      // Handle alerts (show notifications, etc.)
      console.log('Module alert:', data);
    });
  }
  
  return next(action);
};
```

---

## 🎨 **UI COMPONENTS**

### **Module Status Card:**
```typescript
// components/ModuleStatusCard.tsx
import React from 'react';
import { Card, CardContent, CardHeader, CardTitle } from './ui/card';
import { Badge } from './ui/badge';
import { ModuleData } from '../types/moduleData';

interface ModuleStatusCardProps {
  moduleData: ModuleData;
  onSelect: (moduleId: string) => void;
  isSelected: boolean;
}

export const ModuleStatusCard: React.FC<ModuleStatusCardProps> = ({
  moduleData,
  onSelect,
  isSelected
}) => {
  const getStatusColor = (status: string) => {
    switch (status) {
      case 'idle': return 'bg-green-500';
      case 'moving': return 'bg-blue-500';
      case 'stopping': return 'bg-yellow-500';
      case 'error': return 'bg-red-500';
      case 'maintenance': return 'bg-gray-500';
      default: return 'bg-gray-500';
    }
  };
  
  return (
    <Card 
      className={`cursor-pointer transition-all duration-200 ${
        isSelected ? 'ring-2 ring-blue-500' : 'hover:shadow-lg'
      }`}
      onClick={() => onSelect(moduleData.module_id)}
    >
      <CardHeader className="pb-2">
        <CardTitle className="flex items-center justify-between">
          <span className="text-lg font-semibold">{moduleData.module_id}</span>
          <Badge className={getStatusColor(moduleData.status.state)}>
            {moduleData.status.state}
          </Badge>
        </CardTitle>
      </CardHeader>
      <CardContent>
        <div className="grid grid-cols-2 gap-4">
          <div>
            <p className="text-sm text-gray-600">Position</p>
            <p className="text-lg font-medium">{moduleData.status.position} mm</p>
          </div>
          <div>
            <p className="text-sm text-gray-600">Velocity</p>
            <p className="text-lg font-medium">{moduleData.status.velocity} mm/s</p>
          </div>
          <div>
            <p className="text-sm text-gray-600">Temperature</p>
            <p className="text-lg font-medium">{moduleData.motor.temperature}°C</p>
          </div>
          <div>
            <p className="text-sm text-gray-600">Power</p>
            <p className="text-lg font-medium">{moduleData.motor.power} W</p>
          </div>
        </div>
      </CardContent>
    </Card>
  );
};
```

### **Module Detail View:**
```typescript
// components/ModuleDetailView.tsx
import React from 'react';
import { Card, CardContent, CardHeader, CardTitle } from './ui/card';
import { Button } from './ui/button';
import { ModuleData } from '../types/moduleData';

interface ModuleDetailViewProps {
  moduleData: ModuleData;
  onEmergencyStop: (moduleId: string) => void;
  onCommand: (moduleId: string, command: any) => void;
}

export const ModuleDetailView: React.FC<ModuleDetailViewProps> = ({
  moduleData,
  onEmergencyStop,
  onCommand
}) => {
  return (
    <div className="space-y-6">
      {/* Status Overview */}
      <Card>
        <CardHeader>
          <CardTitle>Status Overview</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="grid grid-cols-3 gap-4">
            <div>
              <p className="text-sm text-gray-600">State</p>
              <p className="text-xl font-bold">{moduleData.status.state}</p>
            </div>
            <div>
              <p className="text-sm text-gray-600">Position</p>
              <p className="text-xl font-bold">{moduleData.status.position} mm</p>
            </div>
            <div>
              <p className="text-sm text-gray-600">Velocity</p>
              <p className="text-xl font-bold">{moduleData.status.velocity} mm/s</p>
            </div>
          </div>
        </CardContent>
      </Card>
      
      {/* Motor Information */}
      <Card>
        <CardHeader>
          <CardTitle>Motor Information</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="grid grid-cols-2 gap-4">
            <div>
              <p className="text-sm text-gray-600">Current</p>
              <p className="text-lg font-medium">{moduleData.motor.current} A</p>
            </div>
            <div>
              <p className="text-sm text-gray-600">Voltage</p>
              <p className="text-lg font-medium">{moduleData.motor.voltage} V</p>
            </div>
            <div>
              <p className="text-sm text-gray-600">Temperature</p>
              <p className="text-lg font-medium">{moduleData.motor.temperature}°C</p>
            </div>
            <div>
              <p className="text-sm text-gray-600">Power</p>
              <p className="text-lg font-medium">{moduleData.motor.power} W</p>
            </div>
          </div>
        </CardContent>
      </Card>
      
      {/* Safety Status */}
      <Card>
        <CardHeader>
          <CardTitle>Safety Status</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="grid grid-cols-2 gap-4">
            <div>
              <p className="text-sm text-gray-600">E-Stop</p>
              <p className={`text-lg font-medium ${
                moduleData.safety.e_stop_active ? 'text-red-600' : 'text-green-600'
              }`}>
                {moduleData.safety.e_stop_active ? 'Active' : 'Inactive'}
              </p>
            </div>
            <div>
              <p className="text-sm text-gray-600">Limit Switches</p>
              <p className="text-lg font-medium">
                F: {moduleData.safety.limit_switch_forward ? '✓' : '✗'} | 
                B: {moduleData.safety.limit_switch_backward ? '✓' : '✗'}
              </p>
            </div>
          </div>
        </CardContent>
      </Card>
      
      {/* Control Actions */}
      <Card>
        <CardHeader>
          <CardTitle>Control Actions</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="flex space-x-4">
            <Button 
              variant="outline"
              onClick={() => onCommand(moduleData.module_id, { type: 'stop' })}
            >
              Stop
            </Button>
            <Button 
              variant="outline"
              onClick={() => onCommand(moduleData.module_id, { type: 'home' })}
            >
              Home
            </Button>
            <Button 
              variant="destructive"
              onClick={() => onEmergencyStop(moduleData.module_id)}
            >
              Emergency Stop
            </Button>
          </div>
        </CardContent>
      </Card>
    </div>
  );
};
```

### **Module Dashboard:**
```typescript
// components/ModuleDashboard.tsx
import React, { useEffect } from 'react';
import { useDispatch, useSelector } from 'react-redux';
import { ModuleStatusCard } from './ModuleStatusCard';
import { ModuleDetailView } from './ModuleDetailView';
import { fetchAllModulesStatus, selectModule, executeModuleCommand } from '../store/moduleDataSlice';
import { RootState } from '../store';

export const ModuleDashboard: React.FC = () => {
  const dispatch = useDispatch();
  const { modules, selectedModule, view } = useSelector((state: RootState) => state.moduleData);
  
  useEffect(() => {
    // Fetch initial data
    dispatch(fetchAllModulesStatus());
    
    // Set up periodic refresh
    const interval = setInterval(() => {
      dispatch(fetchAllModulesStatus());
    }, 5000); // Refresh every 5 seconds
    
    return () => clearInterval(interval);
  }, [dispatch]);
  
  const handleModuleSelect = (moduleId: string) => {
    dispatch(selectModule(moduleId));
  };
  
  const handleEmergencyStop = (moduleId: string) => {
    dispatch(executeModuleCommand({ moduleId, command: { type: 'emergency_stop' } }));
  };
  
  const handleCommand = (moduleId: string, command: any) => {
    dispatch(executeModuleCommand({ moduleId, command }));
  };
  
  const filteredModules = Object.values(modules).filter(module => {
    // Apply filters here if needed
    return true;
  });
  
  return (
    <div className="container mx-auto p-6">
      <h1 className="text-3xl font-bold mb-6">Module Dashboard</h1>
      
      {view === 'dashboard' && (
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
          {filteredModules.map(module => (
            <ModuleStatusCard
              key={module.module_id}
              moduleData={module}
              onSelect={handleModuleSelect}
              isSelected={selectedModule === module.module_id}
            />
          ))}
        </div>
      )}
      
      {view === 'detail' && selectedModule && modules[selectedModule] && (
        <ModuleDetailView
          moduleData={modules[selectedModule]}
          onEmergencyStop={handleEmergencyStop}
          onCommand={handleCommand}
        />
      )}
    </div>
  );
};
```

---

## 🔧 **HOOKS & UTILITIES**

### **Custom Hooks:**
```typescript
// hooks/useModuleData.ts
import { useEffect } from 'react';
import { useDispatch, useSelector } from 'react-redux';
import { fetchModuleStatus, updateModuleData } from '../store/moduleDataSlice';
import { ModuleDataApiClient } from '../services/moduleDataApi';
import { RootState } from '../store';

export const useModuleData = (moduleId: string) => {
  const dispatch = useDispatch();
  const moduleData = useSelector((state: RootState) => state.moduleData.modules[moduleId]);
  
  useEffect(() => {
    if (moduleId) {
      dispatch(fetchModuleStatus(moduleId));
      
      // Set up real-time updates
      const apiClient = new ModuleDataApiClient(process.env.REACT_APP_API_URL!);
      apiClient.subscribeToModule(moduleId, (data) => {
        dispatch(updateModuleData({ moduleId, data }));
      });
      
      return () => {
        apiClient.unsubscribeFromModule(moduleId);
      };
    }
  }, [dispatch, moduleId]);
  
  return moduleData;
};

// hooks/useModuleCommands.ts
import { useCallback } from 'react';
import { useDispatch } from 'react-redux';
import { executeModuleCommand } from '../store/moduleDataSlice';

export const useModuleCommands = () => {
  const dispatch = useDispatch();
  
  const executeCommand = useCallback((moduleId: string, command: any) => {
    dispatch(executeModuleCommand({ moduleId, command }));
  }, [dispatch]);
  
  const emergencyStop = useCallback((moduleId: string) => {
    dispatch(executeModuleCommand({ moduleId, command: { type: 'emergency_stop' } }));
  }, [dispatch]);
  
  return { executeCommand, emergencyStop };
};
```

---

## 📱 **RESPONSIVE DESIGN**

### **Mobile-First Approach:**
```typescript
// components/ModuleDashboardMobile.tsx
import React from 'react';
import { ModuleStatusCard } from './ModuleStatusCard';
import { ModuleDetailView } from './ModuleDetailView';

export const ModuleDashboardMobile: React.FC = () => {
  return (
    <div className="block md:hidden">
      {/* Mobile-specific layout */}
      <div className="space-y-4">
        {/* Mobile module cards */}
        <div className="grid grid-cols-1 gap-4">
          {/* Module cards */}
        </div>
        
        {/* Mobile detail view */}
        <div className="fixed bottom-0 left-0 right-0 bg-white border-t p-4">
          {/* Mobile controls */}
        </div>
      </div>
    </div>
  );
};
```

---

## 🧪 **TESTING IMPLEMENTATION**

### **Component Tests:**
```typescript
// __tests__/ModuleStatusCard.test.tsx
import React from 'react';
import { render, screen, fireEvent } from '@testing-library/react';
import { ModuleStatusCard } from '../components/ModuleStatusCard';
import { mockModuleData } from '../mocks/moduleData';

describe('ModuleStatusCard', () => {
  it('renders module information correctly', () => {
    const mockOnSelect = jest.fn();
    
    render(
      <ModuleStatusCard
        moduleData={mockModuleData}
        onSelect={mockOnSelect}
        isSelected={false}
      />
    );
    
    expect(screen.getByText(mockModuleData.module_id)).toBeInTheDocument();
    expect(screen.getByText(mockModuleData.status.state)).toBeInTheDocument();
    expect(screen.getByText(`${mockModuleData.status.position} mm`)).toBeInTheDocument();
  });
  
  it('calls onSelect when clicked', () => {
    const mockOnSelect = jest.fn();
    
    render(
      <ModuleStatusCard
        moduleData={mockModuleData}
        onSelect={mockOnSelect}
        isSelected={false}
      />
    );
    
    fireEvent.click(screen.getByRole('button'));
    expect(mockOnSelect).toHaveBeenCalledWith(mockModuleData.module_id);
  });
});
```

### **Integration Tests:**
```typescript
// __tests__/ModuleDashboard.integration.test.tsx
import React from 'react';
import { render, screen, waitFor } from '@testing-library/react';
import { Provider } from 'react-redux';
import { store } from '../store';
import { ModuleDashboard } from '../components/ModuleDashboard';
import { server } from '../mocks/server';

describe('ModuleDashboard Integration', () => {
  beforeAll(() => server.listen());
  afterEach(() => server.resetHandlers());
  afterAll(() => server.close());
  
  it('loads and displays module data', async () => {
    render(
      <Provider store={store}>
        <ModuleDashboard />
      </Provider>
    );
    
    await waitFor(() => {
      expect(screen.getByText('Module Dashboard')).toBeInTheDocument();
    });
    
    // Check if module data is loaded
    await waitFor(() => {
      expect(screen.getByText('travel_motor_001')).toBeInTheDocument();
    });
  });
});
```

---

**Changelog v1.0:**
- ✅ Created comprehensive frontend integration plan
- ✅ Implemented API client with REST and WebSocket support
- ✅ Designed Redux state management structure
- ✅ Created UI components for module data display
- ✅ Added custom hooks and utilities
- ✅ Implemented responsive design approach
- ✅ Added testing implementation
