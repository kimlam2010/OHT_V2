# OHT-50 Frontend State Management Setup

**Phiên bản:** v1.0  
**Ngày tạo:** 2024-08-24  
**Trạng thái:** Implementation Phase  
**Phase:** 1 - Foundation & Setup

---

## 🗃️ **ZUSTAND STORES SETUP**

### **1. Auth Store (src/stores/authStore.ts)**

```typescript
import { create } from 'zustand'
import { persist } from 'zustand/middleware'
import { AuthState, User } from '@types'
import { STORAGE_KEYS } from '@utils/constants'
import { authService } from '@services/api/auth'

interface AuthStore extends AuthState {
  // Actions
  login: (username: string, password: string) => Promise<void>
  logout: () => void
  refreshToken: () => Promise<void>
  updateUser: (user: User) => void
  clearError: () => void
}

export const useAuthStore = create<AuthStore>()(
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
        set({ isLoading: true, error: null })
        
        try {
          const response = await authService.login(username, password)
          
          set({
            user: response.data.user,
            token: response.data.token,
            isAuthenticated: true,
            isLoading: false,
            error: null,
          })
        } catch (error) {
          set({
            isLoading: false,
            error: error instanceof Error ? error.message : 'Login failed',
          })
          throw error
        }
      },

      logout: () => {
        set({
          user: null,
          token: null,
          isAuthenticated: false,
          isLoading: false,
          error: null,
        })
      },

      refreshToken: async () => {
        const { token } = get()
        if (!token) return

        try {
          const response = await authService.refreshToken(token)
          
          set({
            token: response.data.token,
            user: response.data.user,
          })
        } catch (error) {
          // If refresh fails, logout user
          get().logout()
        }
      },

      updateUser: (user: User) => {
        set({ user })
      },

      clearError: () => {
        set({ error: null })
      },
    }),
    {
      name: STORAGE_KEYS.AUTH_TOKEN,
      partialize: (state) => ({
        token: state.token,
        user: state.user,
        isAuthenticated: state.isAuthenticated,
      }),
    }
  )
)
```

### **2. System Store (src/stores/systemStore.ts)**

```typescript
import { create } from 'zustand'
import { SystemStatus, SystemConfig } from '@types'
import { systemService } from '@services/api/system'

interface SystemStore {
  // State
  status: SystemStatus | null
  config: SystemConfig | null
  isLoading: boolean
  error: string | null
  lastUpdate: string | null

  // Actions
  fetchStatus: () => Promise<void>
  fetchConfig: () => Promise<void>
  updateStatus: (status: SystemStatus) => void
  updateConfig: (config: SystemConfig) => void
  clearError: () => void
}

export const useSystemStore = create<SystemStore>((set, get) => ({
  // Initial state
  status: null,
  config: null,
  isLoading: false,
  error: null,
  lastUpdate: null,

  // Actions
  fetchStatus: async () => {
    set({ isLoading: true, error: null })
    
    try {
      const response = await systemService.getStatus()
      set({
        status: response.data,
        isLoading: false,
        lastUpdate: new Date().toISOString(),
      })
    } catch (error) {
      set({
        isLoading: false,
        error: error instanceof Error ? error.message : 'Failed to fetch status',
      })
    }
  },

  fetchConfig: async () => {
    set({ isLoading: true, error: null })
    
    try {
      const response = await systemService.getConfig()
      set({
        config: response.data,
        isLoading: false,
      })
    } catch (error) {
      set({
        isLoading: false,
        error: error instanceof Error ? error.message : 'Failed to fetch config',
      })
    }
  },

  updateStatus: (status: SystemStatus) => {
    set({ status, lastUpdate: new Date().toISOString() })
  },

  updateConfig: (config: SystemConfig) => {
    set({ config })
  },

  clearError: () => {
    set({ error: null })
  },
}))
```

### **3. Control Store (src/stores/controlStore.ts)**

```typescript
import { create } from 'zustand'
import { ControlCommand, ControlMode } from '@types'
import { controlService } from '@services/api/control'
import { SAFETY_CONSTANTS } from '@utils/constants'

interface ControlStore {
  // State
  currentMode: ControlMode | null
  availableModes: ControlMode[]
  lastCommand: ControlCommand | null
  isEmergencyStopActive: boolean
  emergencyStopCooldown: boolean
  isLoading: boolean
  error: string | null

  // Actions
  fetchModes: () => Promise<void>
  setMode: (modeId: string) => Promise<void>
  sendCommand: (command: Omit<ControlCommand, 'timestamp'>) => Promise<void>
  emergencyStop: () => Promise<void>
  resetEmergencyStop: () => void
  clearError: () => void
}

export const useControlStore = create<ControlStore>((set, get) => ({
  // Initial state
  currentMode: null,
  availableModes: [],
  lastCommand: null,
  isEmergencyStopActive: false,
  emergencyStopCooldown: false,
  isLoading: false,
  error: null,

  // Actions
  fetchModes: async () => {
    set({ isLoading: true, error: null })
    
    try {
      const response = await controlService.getModes()
      set({
        availableModes: response.data,
        isLoading: false,
      })
    } catch (error) {
      set({
        isLoading: false,
        error: error instanceof Error ? error.message : 'Failed to fetch modes',
      })
    }
  },

  setMode: async (modeId: string) => {
    const { availableModes } = get()
    const mode = availableModes.find(m => m.id === modeId)
    if (!mode) return

    set({ isLoading: true, error: null })
    
    try {
      await controlService.setMode(modeId)
      set({
        currentMode: mode,
        isLoading: false,
      })
    } catch (error) {
      set({
        isLoading: false,
        error: error instanceof Error ? error.message : 'Failed to set mode',
      })
    }
  },

  sendCommand: async (command: Omit<ControlCommand, 'timestamp'>) => {
    const { currentMode, isEmergencyStopActive } = get()
    
    // Safety checks
    if (isEmergencyStopActive) {
      set({ error: 'Emergency stop is active' })
      return
    }

    if (!currentMode) {
      set({ error: 'No control mode selected' })
      return
    }

    if (!currentMode.allowedCommands.includes(command.type)) {
      set({ error: `Command ${command.type} not allowed in current mode` })
      return
    }

    set({ isLoading: true, error: null })
    
    try {
      const fullCommand: ControlCommand = {
        ...command,
        timestamp: new Date().toISOString(),
      }
      
      await controlService.sendCommand(fullCommand)
      set({
        lastCommand: fullCommand,
        isLoading: false,
      })
    } catch (error) {
      set({
        isLoading: false,
        error: error instanceof Error ? error.message : 'Failed to send command',
      })
    }
  },

  emergencyStop: async () => {
    const { isEmergencyStopActive, emergencyStopCooldown } = get()
    
    if (isEmergencyStopActive || emergencyStopCooldown) {
      return
    }

    set({ 
      isEmergencyStopActive: true, 
      emergencyStopCooldown: true,
      isLoading: true,
      error: null 
    })
    
    try {
      await controlService.emergencyStop()
      set({ isLoading: false })
      
      // Start cooldown timer
      setTimeout(() => {
        set({ emergencyStopCooldown: false })
      }, SAFETY_CONSTANTS.EMERGENCY_STOP_COOLDOWN)
      
    } catch (error) {
      set({
        isLoading: false,
        error: error instanceof Error ? error.message : 'Emergency stop failed',
      })
    }
  },

  resetEmergencyStop: () => {
    set({ isEmergencyStopActive: false })
  },

  clearError: () => {
    set({ error: null })
  },
}))
```

### **4. Telemetry Store (src/stores/telemetryStore.ts)**

```typescript
import { create } from 'zustand'
import { TelemetryData, TelemetryChart } from '@types'
import { telemetryService } from '@services/api/telemetry'

interface TelemetryStore {
  // State
  currentData: TelemetryData | null
  historicalData: TelemetryData[]
  charts: TelemetryChart[]
  isLoading: boolean
  error: string | null
  lastUpdate: string | null

  // Actions
  fetchCurrentData: () => Promise<void>
  fetchHistoricalData: (timeRange: string) => Promise<void>
  fetchCharts: () => Promise<void>
  updateCurrentData: (data: TelemetryData) => void
  addHistoricalData: (data: TelemetryData) => void
  clearHistoricalData: () => void
  clearError: () => void
}

export const useTelemetryStore = create<TelemetryStore>((set, get) => ({
  // Initial state
  currentData: null,
  historicalData: [],
  charts: [],
  isLoading: false,
  error: null,
  lastUpdate: null,

  // Actions
  fetchCurrentData: async () => {
    set({ isLoading: true, error: null })
    
    try {
      const response = await telemetryService.getCurrentData()
      set({
        currentData: response.data,
        isLoading: false,
        lastUpdate: new Date().toISOString(),
      })
    } catch (error) {
      set({
        isLoading: false,
        error: error instanceof Error ? error.message : 'Failed to fetch current data',
      })
    }
  },

  fetchHistoricalData: async (timeRange: string) => {
    set({ isLoading: true, error: null })
    
    try {
      const response = await telemetryService.getHistoricalData(timeRange)
      set({
        historicalData: response.data,
        isLoading: false,
      })
    } catch (error) {
      set({
        isLoading: false,
        error: error instanceof Error ? error.message : 'Failed to fetch historical data',
      })
    }
  },

  fetchCharts: async () => {
    set({ isLoading: true, error: null })
    
    try {
      const response = await telemetryService.getCharts()
      set({
        charts: response.data,
        isLoading: false,
      })
    } catch (error) {
      set({
        isLoading: false,
        error: error instanceof Error ? error.message : 'Failed to fetch charts',
      })
    }
  },

  updateCurrentData: (data: TelemetryData) => {
    set({ 
      currentData: data, 
      lastUpdate: new Date().toISOString() 
    })
  },

  addHistoricalData: (data: TelemetryData) => {
    const { historicalData } = get()
    const newData = [...historicalData, data]
    
    // Keep only last 1000 data points
    if (newData.length > 1000) {
      newData.splice(0, newData.length - 1000)
    }
    
    set({ historicalData: newData })
  },

  clearHistoricalData: () => {
    set({ historicalData: [] })
  },

  clearError: () => {
    set({ error: null })
  },
}))
```

### **5. Alert Store (src/stores/alertStore.ts)**

```typescript
import { create } from 'zustand'
import { Alert } from '@types'
import { alertService } from '@services/api/alerts'

interface AlertStore {
  // State
  alerts: Alert[]
  unacknowledgedCount: number
  isLoading: boolean
  error: string | null

  // Actions
  fetchAlerts: () => Promise<void>
  acknowledgeAlert: (alertId: string) => Promise<void>
  addAlert: (alert: Alert) => void
  removeAlert: (alertId: string) => void
  clearAlerts: () => void
  clearError: () => void
}

export const useAlertStore = create<AlertStore>((set, get) => ({
  // Initial state
  alerts: [],
  unacknowledgedCount: 0,
  isLoading: false,
  error: null,

  // Actions
  fetchAlerts: async () => {
    set({ isLoading: true, error: null })
    
    try {
      const response = await alertService.getAlerts()
      const alerts = response.data
      const unacknowledgedCount = alerts.filter(alert => !alert.acknowledged).length
      
      set({
        alerts,
        unacknowledgedCount,
        isLoading: false,
      })
    } catch (error) {
      set({
        isLoading: false,
        error: error instanceof Error ? error.message : 'Failed to fetch alerts',
      })
    }
  },

  acknowledgeAlert: async (alertId: string) => {
    try {
      await alertService.acknowledgeAlert(alertId)
      
      const { alerts } = get()
      const updatedAlerts = alerts.map(alert =>
        alert.id === alertId
          ? { ...alert, acknowledged: true, acknowledgedAt: new Date().toISOString() }
          : alert
      )
      
      const unacknowledgedCount = updatedAlerts.filter(alert => !alert.acknowledged).length
      
      set({
        alerts: updatedAlerts,
        unacknowledgedCount,
      })
    } catch (error) {
      set({
        error: error instanceof Error ? error.message : 'Failed to acknowledge alert',
      })
    }
  },

  addAlert: (alert: Alert) => {
    const { alerts } = get()
    const newAlerts = [alert, ...alerts]
    
    // Keep only last 100 alerts
    if (newAlerts.length > 100) {
      newAlerts.splice(100)
    }
    
    const unacknowledgedCount = newAlerts.filter(a => !a.acknowledged).length
    
    set({
      alerts: newAlerts,
      unacknowledgedCount,
    })

    // Play sound for critical alerts
    if (alert.type === 'critical') {
      playAlertSound()
    }
  },

  removeAlert: (alertId: string) => {
    const { alerts } = get()
    const updatedAlerts = alerts.filter(alert => alert.id !== alertId)
    const unacknowledgedCount = updatedAlerts.filter(alert => !alert.acknowledged).length
    
    set({
      alerts: updatedAlerts,
      unacknowledgedCount,
    })
  },

  clearAlerts: () => {
    set({
      alerts: [],
      unacknowledgedCount: 0,
    })
  },

  clearError: () => {
    set({ error: null })
  },
}))

// Helper function to play alert sound
function playAlertSound() {
  try {
    const audio = new Audio('/sounds/alert-critical.mp3')
    audio.volume = 0.5
    audio.play().catch(() => {
      // Ignore errors if audio fails to play
    })
  } catch (error) {
    // Ignore errors if audio is not supported
  }
}
```

### **6. WebSocket Store (src/stores/websocketStore.ts)**

```typescript
import { create } from 'zustand'
import { WebSocketState, WebSocketMessage } from '@types'
import { WEBSOCKET_CONFIG } from '@utils/constants'

interface WebSocketStore extends WebSocketState {
  // Actions
  connect: () => void
  disconnect: () => void
  sendMessage: (message: WebSocketMessage) => void
  handleMessage: (message: WebSocketMessage) => void
}

export const useWebSocketStore = create<WebSocketStore>((set, get) => ({
  // Initial state
  isConnected: false,
  isConnecting: false,
  error: null,
  reconnectAttempts: 0,

  // WebSocket instance
  socket: null as WebSocket | null,

  // Actions
  connect: () => {
    const { isConnected, isConnecting, reconnectAttempts } = get()
    
    if (isConnected || isConnecting) return
    
    if (reconnectAttempts >= WEBSOCKET_CONFIG.MAX_RECONNECT_ATTEMPTS) {
      set({ error: 'Max reconnection attempts reached' })
      return
    }

    set({ isConnecting: true, error: null })
    
    try {
      const socket = new WebSocket(WEBSOCKET_CONFIG.URL)
      
      socket.onopen = () => {
        set({
          isConnected: true,
          isConnecting: false,
          reconnectAttempts: 0,
          error: null,
        })
        
        // Start heartbeat
        startHeartbeat(socket)
      }
      
      socket.onmessage = (event) => {
        try {
          const message: WebSocketMessage = JSON.parse(event.data)
          get().handleMessage(message)
        } catch (error) {
          console.error('Failed to parse WebSocket message:', error)
        }
      }
      
      socket.onclose = () => {
        set({ isConnected: false, isConnecting: false })
        
        // Attempt to reconnect
        setTimeout(() => {
          const { reconnectAttempts } = get()
          set({ reconnectAttempts: reconnectAttempts + 1 })
          get().connect()
        }, WEBSOCKET_CONFIG.RECONNECT_INTERVAL)
      }
      
      socket.onerror = (error) => {
        set({
          isConnecting: false,
          error: 'WebSocket connection failed',
        })
      }
      
      set({ socket })
      
    } catch (error) {
      set({
        isConnecting: false,
        error: 'Failed to create WebSocket connection',
      })
    }
  },

  disconnect: () => {
    const { socket } = get()
    
    if (socket) {
      socket.close()
      set({
        socket: null,
        isConnected: false,
        isConnecting: false,
      })
    }
  },

  sendMessage: (message: WebSocketMessage) => {
    const { socket, isConnected } = get()
    
    if (!isConnected || !socket) {
      console.warn('WebSocket not connected, cannot send message')
      return
    }
    
    try {
      socket.send(JSON.stringify(message))
    } catch (error) {
      console.error('Failed to send WebSocket message:', error)
    }
  },

  handleMessage: (message: WebSocketMessage) => {
    // Import stores here to avoid circular dependencies
    const { useSystemStore } = require('./systemStore')
    const { useTelemetryStore } = require('./telemetryStore')
    const { useAlertStore } = require('./alertStore')
    const { useControlStore } = require('./controlStore')
    
    switch (message.type) {
      case 'telemetry':
        useTelemetryStore.getState().updateCurrentData(message.data)
        useTelemetryStore.getState().addHistoricalData(message.data)
        break
        
      case 'status':
        useSystemStore.getState().updateStatus(message.data)
        break
        
      case 'alert':
        useAlertStore.getState().addAlert(message.data)
        break
        
      case 'command':
        // Handle command responses
        if (message.data.status === 'completed') {
          useControlStore.getState().clearError()
        } else if (message.data.status === 'failed') {
          useControlStore.getState().set({ error: message.data.error })
        }
        break
        
      case 'error':
        console.error('WebSocket error:', message.data)
        break
        
      default:
        console.warn('Unknown WebSocket message type:', message.type)
    }
  },
}))

// Helper function to start heartbeat
function startHeartbeat(socket: WebSocket) {
  const heartbeat = setInterval(() => {
    if (socket.readyState === WebSocket.OPEN) {
      socket.send(JSON.stringify({ type: 'heartbeat', data: {}, timestamp: new Date().toISOString() }))
    } else {
      clearInterval(heartbeat)
    }
  }, WEBSOCKET_CONFIG.HEARTBEAT_INTERVAL)
}
```

### **7. Store Integration (src/stores/index.ts)**

```typescript
// Export all stores
export { useAuthStore } from './authStore'
export { useSystemStore } from './systemStore'
export { useControlStore } from './controlStore'
export { useTelemetryStore } from './telemetryStore'
export { useAlertStore } from './alertStore'
export { useWebSocketStore } from './websocketStore'

// Store initialization
export function initializeStores() {
  // Initialize WebSocket connection when user is authenticated
  const { isAuthenticated } = useAuthStore.getState()
  
  if (isAuthenticated) {
    useWebSocketStore.getState().connect()
  }
}

// Store cleanup
export function cleanupStores() {
  useWebSocketStore.getState().disconnect()
  useAuthStore.getState().logout()
}
```

---

## 🔧 **NEXT STEPS**

### **Immediate Actions:**
1. ✅ Create all Zustand stores
2. ✅ Implement state management logic
3. ✅ Set up persistence for auth store
4. ✅ Configure WebSocket integration
5. ✅ Test store interactions

### **Phase 1 Week 1 Day 5 Checklist:**
- [x] Auth store with persistence
- [x] System store for status/config
- [x] Control store with safety checks
- [x] Telemetry store for real-time data
- [x] Alert store with sound notifications
- [x] WebSocket store with reconnection
- [x] Store integration and initialization

### **Ready for Next Phase:**
- Component Library Development (Week 3)
- API Integration Implementation (Week 2)

---

**Changelog:**
- v1.0: Zustand stores implementation completed

**Status:** Ready for component development
