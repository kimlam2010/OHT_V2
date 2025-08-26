# OHT-50 Frontend Initial Files

**Phiên bản:** v1.0  
**Ngày tạo:** 2024-08-24  
**Trạng thái:** Implementation Phase  
**Phase:** 1 - Foundation & Setup

---

## 📁 **CORE FILES TO CREATE**

### **1. Main Entry Point (src/main.tsx)**

```typescript
import React from 'react'
import ReactDOM from 'react-dom/client'
import { BrowserRouter } from 'react-router-dom'
import App from './App.tsx'
import './styles/globals.css'
import './styles/design-system.css'

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <BrowserRouter>
      <App />
    </BrowserRouter>
  </React.StrictMode>,
)
```

### **2. App Component (src/App.tsx)**

```typescript
import React from 'react'
import { Routes, Route } from 'react-router-dom'
import { Flowbite } from 'flowbite-react'
import Layout from '@components/layout/Layout'
import Dashboard from '@pages/Dashboard'
import Control from '@pages/Control'
import Missions from '@pages/Missions'
import Configuration from '@pages/Configuration'
import Login from '@pages/Login'
import { useAuthStore } from '@stores/authStore'

const App: React.FC = () => {
  const { isAuthenticated } = useAuthStore()

  if (!isAuthenticated) {
    return <Login />
  }

  return (
    <Flowbite>
      <Layout>
        <Routes>
          <Route path="/" element={<Dashboard />} />
          <Route path="/control" element={<Control />} />
          <Route path="/missions" element={<Missions />} />
          <Route path="/configuration" element={<Configuration />} />
        </Routes>
      </Layout>
    </Flowbite>
  )
}

export default App
```

### **3. Type Definitions (src/types/index.ts)**

```typescript
// System Types
export interface SystemStatus {
  id: string
  status: 'idle' | 'moving' | 'docking' | 'fault' | 'estop'
  battery: number
  temperature: number
  position: {
    x: number
    y: number
    z: number
  }
  speed: number
  acceleration: number
  timestamp: string
}

export interface SystemConfig {
  id: string
  name: string
  version: string
  maxSpeed: number
  maxAcceleration: number
  safetyZones: SafetyZone[]
  updatedAt: string
}

export interface SafetyZone {
  id: string
  name: string
  type: 'restricted' | 'warning' | 'safe'
  coordinates: Array<{ x: number; y: number; z: number }>
}

// Control Types
export interface ControlCommand {
  type: 'move' | 'stop' | 'emergency_stop' | 'mode_change'
  direction?: 'forward' | 'backward' | 'left' | 'right' | 'up' | 'down'
  speed?: number
  mode?: 'auto' | 'manual' | 'semi' | 'maint'
  timestamp: string
}

export interface ControlMode {
  id: string
  name: string
  description: string
  allowedCommands: string[]
  safetyLevel: 'low' | 'medium' | 'high'
}

// Telemetry Types
export interface TelemetryData {
  id: string
  timestamp: string
  position: { x: number; y: number; z: number }
  velocity: { x: number; y: number; z: number }
  acceleration: { x: number; y: number; z: number }
  battery: number
  temperature: number
  status: SystemStatus['status']
}

export interface TelemetryChart {
  id: string
  title: string
  type: 'line' | 'bar' | 'scatter'
  data: TelemetryData[]
  timeRange: '1h' | '6h' | '24h' | '7d'
}

// Mission Types
export interface Mission {
  id: string
  name: string
  description: string
  status: 'pending' | 'running' | 'completed' | 'failed' | 'cancelled'
  waypoints: Waypoint[]
  createdAt: string
  startedAt?: string
  completedAt?: string
  priority: 'low' | 'medium' | 'high' | 'critical'
}

export interface Waypoint {
  id: string
  position: { x: number; y: number; z: number }
  action?: 'pick' | 'place' | 'wait' | 'scan'
  parameters?: Record<string, any>
  order: number
}

// Alert Types
export interface Alert {
  id: string
  type: 'info' | 'warning' | 'error' | 'critical'
  title: string
  message: string
  timestamp: string
  acknowledged: boolean
  acknowledgedBy?: string
  acknowledgedAt?: string
  source: 'system' | 'user' | 'sensor' | 'mission'
  category: 'safety' | 'operation' | 'maintenance' | 'communication'
}

// User Types
export interface User {
  id: string
  username: string
  email: string
  role: 'admin' | 'operator' | 'guest'
  permissions: string[]
  lastLogin?: string
  createdAt: string
}

export interface AuthState {
  user: User | null
  token: string | null
  isAuthenticated: boolean
  isLoading: boolean
  error: string | null
}

// API Response Types
export interface ApiResponse<T> {
  success: boolean
  data?: T
  error?: string
  message?: string
  timestamp: string
}

export interface PaginatedResponse<T> extends ApiResponse<T[]> {
  pagination: {
    page: number
    limit: number
    total: number
    totalPages: number
  }
}

// WebSocket Types
export interface WebSocketMessage {
  type: 'telemetry' | 'status' | 'alert' | 'command' | 'error'
  data: any
  timestamp: string
}

export interface WebSocketState {
  isConnected: boolean
  isConnecting: boolean
  error: string | null
  reconnectAttempts: number
}
```

### **4. Constants (src/utils/constants.ts)**

```typescript
// API Configuration
export const API_CONFIG = {
  BASE_URL: import.meta.env.VITE_API_BASE_URL || 'http://localhost:8000',
  TIMEOUT: 10000,
  RETRY_ATTEMPTS: 3,
  RETRY_DELAY: 1000,
} as const

// WebSocket Configuration
export const WEBSOCKET_CONFIG = {
  URL: import.meta.env.VITE_WS_URL || 'ws://localhost:8000/ws',
  RECONNECT_INTERVAL: 5000,
  MAX_RECONNECT_ATTEMPTS: 10,
  HEARTBEAT_INTERVAL: 30000,
} as const

// System Constants
export const SYSTEM_CONSTANTS = {
  MAX_SPEED: 2.0, // m/s
  MAX_ACCELERATION: 1.0, // m/s²
  BATTERY_WARNING_THRESHOLD: 20, // %
  BATTERY_CRITICAL_THRESHOLD: 10, // %
  TEMPERATURE_WARNING_THRESHOLD: 60, // °C
  TEMPERATURE_CRITICAL_THRESHOLD: 80, // °C
  EMERGENCY_STOP_LATENCY: 100, // ms
} as const

// UI Constants
export const UI_CONSTANTS = {
  ANIMATION_DURATION: 200, // ms
  DEBOUNCE_DELAY: 300, // ms
  TOAST_DURATION: 5000, // ms
  REFRESH_INTERVAL: 1000, // ms
  CHART_UPDATE_INTERVAL: 100, // ms
} as const

// Safety Constants
export const SAFETY_CONSTANTS = {
  EMERGENCY_STOP_COOLDOWN: 5000, // ms
  SAFETY_CONFIRMATION_TIMEOUT: 10000, // ms
  MAX_COMMAND_FREQUENCY: 10, // commands per second
  SAFETY_ZONE_BUFFER: 0.5, // meters
} as const

// Chart Configuration
export const CHART_CONFIG = {
  COLORS: {
    primary: '#f97316',
    secondary: '#64748b',
    success: '#10b981',
    warning: '#f59e0b',
    error: '#ef4444',
    info: '#3b82f6',
  },
  ANIMATION: {
    duration: 750,
    easing: 'easeInOutQuart',
  },
  RESPONSIVE: true,
  MAINTAIN_ASPECT_RATIO: false,
} as const

// Local Storage Keys
export const STORAGE_KEYS = {
  AUTH_TOKEN: 'oht50_auth_token',
  USER_PREFERENCES: 'oht50_user_preferences',
  DASHBOARD_LAYOUT: 'oht50_dashboard_layout',
  THEME: 'oht50_theme',
} as const

// Route Paths
export const ROUTES = {
  HOME: '/',
  DASHBOARD: '/',
  CONTROL: '/control',
  MISSIONS: '/missions',
  CONFIGURATION: '/configuration',
  LOGIN: '/login',
} as const

// Permission Constants
export const PERMISSIONS = {
  // System permissions
  SYSTEM_VIEW: 'system:view',
  SYSTEM_CONTROL: 'system:control',
  SYSTEM_CONFIG: 'system:config',
  
  // Mission permissions
  MISSION_VIEW: 'mission:view',
  MISSION_CREATE: 'mission:create',
  MISSION_EDIT: 'mission:edit',
  MISSION_DELETE: 'mission:delete',
  MISSION_EXECUTE: 'mission:execute',
  
  // User permissions
  USER_VIEW: 'user:view',
  USER_CREATE: 'user:create',
  USER_EDIT: 'user:edit',
  USER_DELETE: 'user:delete',
  
  // Safety permissions
  SAFETY_OVERRIDE: 'safety:override',
  EMERGENCY_STOP: 'emergency:stop',
} as const

// Role Permissions Mapping
export const ROLE_PERMISSIONS = {
  admin: Object.values(PERMISSIONS),
  operator: [
    PERMISSIONS.SYSTEM_VIEW,
    PERMISSIONS.SYSTEM_CONTROL,
    PERMISSIONS.MISSION_VIEW,
    PERMISSIONS.MISSION_CREATE,
    PERMISSIONS.MISSION_EDIT,
    PERMISSIONS.MISSION_EXECUTE,
    PERMISSIONS.EMERGENCY_STOP,
  ],
  guest: [
    PERMISSIONS.SYSTEM_VIEW,
    PERMISSIONS.MISSION_VIEW,
  ],
} as const
```

### **5. Utility Functions (src/utils/helpers.ts)**

```typescript
import { clsx, type ClassValue } from 'clsx'
import { twMerge } from 'tailwind-merge'
import { format, formatDistanceToNow } from 'date-fns'

// Tailwind CSS class merging utility
export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs))
}

// Date formatting utilities
export function formatDateTime(date: string | Date): string {
  const dateObj = typeof date === 'string' ? new Date(date) : date
  return format(dateObj, 'yyyy-MM-dd HH:mm:ss')
}

export function formatRelativeTime(date: string | Date): string {
  const dateObj = typeof date === 'string' ? new Date(date) : date
  return formatDistanceToNow(dateObj, { addSuffix: true })
}

export function formatDuration(seconds: number): string {
  const hours = Math.floor(seconds / 3600)
  const minutes = Math.floor((seconds % 3600) / 60)
  const secs = seconds % 60

  if (hours > 0) {
    return `${hours}h ${minutes}m ${secs}s`
  } else if (minutes > 0) {
    return `${minutes}m ${secs}s`
  } else {
    return `${secs}s`
  }
}

// Number formatting utilities
export function formatNumber(value: number, decimals: number = 2): string {
  return value.toFixed(decimals)
}

export function formatPercentage(value: number, decimals: number = 1): string {
  return `${(value * 100).toFixed(decimals)}%`
}

export function formatSpeed(speed: number): string {
  return `${formatNumber(speed)} m/s`
}

export function formatAcceleration(acceleration: number): string {
  return `${formatNumber(acceleration)} m/s²`
}

// Validation utilities
export function isValidEmail(email: string): boolean {
  const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/
  return emailRegex.test(email)
}

export function isValidPassword(password: string): boolean {
  // At least 8 characters, 1 uppercase, 1 lowercase, 1 number
  const passwordRegex = /^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)[a-zA-Z\d@$!%*?&]{8,}$/
  return passwordRegex.test(password)
}

// Array utilities
export function chunk<T>(array: T[], size: number): T[][] {
  const chunks: T[][] = []
  for (let i = 0; i < array.length; i += size) {
    chunks.push(array.slice(i, i + size))
  }
  return chunks
}

export function groupBy<T, K extends keyof any>(
  array: T[],
  key: (item: T) => K
): Record<K, T[]> {
  return array.reduce((groups, item) => {
    const group = key(item)
    groups[group] = groups[group] || []
    groups[group].push(item)
    return groups
  }, {} as Record<K, T[]>)
}

// Object utilities
export function deepClone<T>(obj: T): T {
  if (obj === null || typeof obj !== 'object') {
    return obj
  }
  
  if (obj instanceof Date) {
    return new Date(obj.getTime()) as T
  }
  
  if (obj instanceof Array) {
    return obj.map(item => deepClone(item)) as T
  }
  
  if (typeof obj === 'object') {
    const clonedObj = {} as T
    for (const key in obj) {
      if (obj.hasOwnProperty(key)) {
        clonedObj[key] = deepClone(obj[key])
      }
    }
    return clonedObj
  }
  
  return obj
}

// Debounce utility
export function debounce<T extends (...args: any[]) => any>(
  func: T,
  wait: number
): (...args: Parameters<T>) => void {
  let timeout: NodeJS.Timeout
  return (...args: Parameters<T>) => {
    clearTimeout(timeout)
    timeout = setTimeout(() => func(...args), wait)
  }
}

// Throttle utility
export function throttle<T extends (...args: any[]) => any>(
  func: T,
  limit: number
): (...args: Parameters<T>) => void {
  let inThrottle: boolean
  return (...args: Parameters<T>) => {
    if (!inThrottle) {
      func(...args)
      inThrottle = true
      setTimeout(() => (inThrottle = false), limit)
    }
  }
}

// Error handling utilities
export function getErrorMessage(error: unknown): string {
  if (error instanceof Error) {
    return error.message
  }
  if (typeof error === 'string') {
    return error
  }
  return 'An unknown error occurred'
}

// Local storage utilities
export function getStorageItem<T>(key: string, defaultValue: T): T {
  try {
    const item = localStorage.getItem(key)
    return item ? JSON.parse(item) : defaultValue
  } catch {
    return defaultValue
  }
}

export function setStorageItem<T>(key: string, value: T): void {
  try {
    localStorage.setItem(key, JSON.stringify(value))
  } catch (error) {
    console.error('Failed to save to localStorage:', error)
  }
}

export function removeStorageItem(key: string): void {
  try {
    localStorage.removeItem(key)
  } catch (error) {
    console.error('Failed to remove from localStorage:', error)
  }
}

// Color utilities
export function getStatusColor(status: string): string {
  const statusColors = {
    idle: 'bg-status-idle',
    moving: 'bg-status-moving',
    docking: 'bg-status-docking',
    fault: 'bg-status-fault',
    estop: 'bg-status-estop',
  }
  return statusColors[status as keyof typeof statusColors] || 'bg-industrial-500'
}

export function getAlertColor(type: string): string {
  const alertColors = {
    info: 'bg-safety-info',
    warning: 'bg-safety-warning',
    error: 'bg-safety-error',
    critical: 'bg-red-800',
  }
  return alertColors[type as keyof typeof alertColors] || 'bg-industrial-500'
}
```

### **6. Validation Schemas (src/utils/validation.ts)**

```typescript
import { z } from 'zod'

// User validation schemas
export const loginSchema = z.object({
  username: z.string().min(1, 'Username is required'),
  password: z.string().min(1, 'Password is required'),
})

export const userSchema = z.object({
  username: z.string().min(3, 'Username must be at least 3 characters'),
  email: z.string().email('Invalid email address'),
  role: z.enum(['admin', 'operator', 'guest']),
  permissions: z.array(z.string()).optional(),
})

// Mission validation schemas
export const waypointSchema = z.object({
  position: z.object({
    x: z.number().min(-1000).max(1000),
    y: z.number().min(-1000).max(1000),
    z: z.number().min(-100).max(100),
  }),
  action: z.enum(['pick', 'place', 'wait', 'scan']).optional(),
  parameters: z.record(z.any()).optional(),
  order: z.number().min(0),
})

export const missionSchema = z.object({
  name: z.string().min(1, 'Mission name is required'),
  description: z.string().optional(),
  waypoints: z.array(waypointSchema).min(1, 'At least one waypoint is required'),
  priority: z.enum(['low', 'medium', 'high', 'critical']),
})

// Control validation schemas
export const controlCommandSchema = z.object({
  type: z.enum(['move', 'stop', 'emergency_stop', 'mode_change']),
  direction: z.enum(['forward', 'backward', 'left', 'right', 'up', 'down']).optional(),
  speed: z.number().min(0).max(2.0).optional(),
  mode: z.enum(['auto', 'manual', 'semi', 'maint']).optional(),
})

// Configuration validation schemas
export const systemConfigSchema = z.object({
  name: z.string().min(1, 'System name is required'),
  maxSpeed: z.number().min(0.1).max(5.0),
  maxAcceleration: z.number().min(0.1).max(3.0),
  safetyZones: z.array(z.object({
    name: z.string().min(1, 'Zone name is required'),
    type: z.enum(['restricted', 'warning', 'safe']),
    coordinates: z.array(z.object({
      x: z.number(),
      y: z.number(),
      z: z.number(),
    })).min(3, 'At least 3 coordinates required'),
  })),
})

// Alert validation schemas
export const alertSchema = z.object({
  type: z.enum(['info', 'warning', 'error', 'critical']),
  title: z.string().min(1, 'Alert title is required'),
  message: z.string().min(1, 'Alert message is required'),
  source: z.enum(['system', 'user', 'sensor', 'mission']),
  category: z.enum(['safety', 'operation', 'maintenance', 'communication']),
})

// Type exports
export type LoginForm = z.infer<typeof loginSchema>
export type UserForm = z.infer<typeof userSchema>
export type MissionForm = z.infer<typeof missionSchema>
export type ControlCommandForm = z.infer<typeof controlCommandSchema>
export type SystemConfigForm = z.infer<typeof systemConfigSchema>
export type AlertForm = z.infer<typeof alertSchema>
```

---

## 🔧 **NEXT STEPS**

### **Immediate Actions:**
1. ✅ Create all initial files
2. ✅ Set up TypeScript types
3. ✅ Configure utility functions
4. ✅ Set up validation schemas
5. ✅ Test basic routing

### **Phase 1 Week 1 Day 1-2 Checklist:**
- [x] Project structure created
- [x] Core files implemented
- [x] Type definitions complete
- [x] Utility functions ready
- [x] Validation schemas defined
- [x] Basic routing setup

### **Ready for Next Phase:**
- Design System Implementation (Day 3-4)
- State Management Foundation (Day 5)

---

**Changelog:**
- v1.0: Initial core files created

**Status:** Ready for component development
