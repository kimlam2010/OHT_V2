# API Integration Implementation

**Version:** 1.0  
**Date:** 2024-12-19  
**Team:** Frontend  
**Status:** In Progress

## 📋 **Overview**

API integration implementation cho OHT-50 frontend, bao gồm REST API client, WebSocket client, error handling, và authentication.

## 🏗️ **Architecture Overview**

### **API Integration Layers**
```typescript
// API Integration Architecture
┌─────────────────────────────────────┐
│           UI Components             │
├─────────────────────────────────────┤
│         Zustand Stores              │
├─────────────────────────────────────┤
│         API Client Layer            │
├─────────────────────────────────────┤
│        HTTP Client (Axios)          │
├─────────────────────────────────────┤
│        WebSocket Client             │
├─────────────────────────────────────┤
│         Backend API                 │
└─────────────────────────────────────┘
```

### **API Client Structure**
```typescript
// API Client Organization
services/
├── api/
│   ├── client.ts              // Base API client
│   ├── auth.ts               // Authentication API
│   ├── system.ts             // System API
│   ├── control.ts            // Control API
│   ├── telemetry.ts          // Telemetry API
│   ├── missions.ts           // Missions API
│   └── config.ts             // Configuration API
├── websocket/
│   ├── client.ts             // WebSocket client
│   └── handlers.ts           // Message handlers
└── types/
    ├── api.ts                // API types
    └── responses.ts          // Response types
```

## 🔧 **Base API Client**

### **API Client Implementation**
```typescript
// services/api/client.ts
import axios, { AxiosInstance, AxiosRequestConfig, AxiosResponse } from 'axios';
import { useAuthStore } from '../../stores/auth/authStore';

// API Configuration
const API_CONFIG = {
  baseURL: process.env.REACT_APP_API_BASE_URL || 'http://localhost:8000/api',
  timeout: 10000,
  headers: {
    'Content-Type': 'application/json'
  }
};

// Response Types
interface ApiResponse<T = any> {
  data: T;
  message?: string;
  success: boolean;
}

interface ApiError {
  message: string;
  code?: string;
  details?: any;
}

// Error Types
class ApiError extends Error {
  public status: number;
  public code?: string;
  public details?: any;

  constructor(message: string, status: number, code?: string, details?: any) {
    super(message);
    this.name = 'ApiError';
    this.status = status;
    this.code = code;
    this.details = details;
  }
}

// Request Interceptor
const requestInterceptor = (config: AxiosRequestConfig) => {
  const token = useAuthStore.getState().token;
  
  if (token) {
    config.headers = {
      ...config.headers,
      Authorization: `Bearer ${token}`
    };
  }
  
  return config;
};

// Response Interceptor
const responseInterceptor = (response: AxiosResponse) => {
  return response;
};

const errorInterceptor = (error: any) => {
  if (error.response) {
    const { status, data } = error.response;
    
    // Handle authentication errors
    if (status === 401) {
      useAuthStore.getState().logout();
      throw new ApiError('Authentication required', status, 'AUTH_REQUIRED');
    }
    
    // Handle forbidden errors
    if (status === 403) {
      throw new ApiError('Access denied', status, 'ACCESS_DENIED');
    }
    
    // Handle server errors
    if (status >= 500) {
      throw new ApiError('Server error', status, 'SERVER_ERROR');
    }
    
    // Handle other errors
    const message = data?.message || 'Request failed';
    throw new ApiError(message, status, data?.code);
  }
  
  if (error.request) {
    throw new ApiError('Network error', 0, 'NETWORK_ERROR');
  }
  
  throw new ApiError('Unknown error', 0, 'UNKNOWN_ERROR');
};

// Create API Client
export const createApiClient = (): AxiosInstance => {
  const client = axios.create(API_CONFIG);
  
  // Add interceptors
  client.interceptors.request.use(requestInterceptor);
  client.interceptors.response.use(responseInterceptor, errorInterceptor);
  
  return client;
};

// Default API client instance
export const apiClient = createApiClient();

// API Client Class
export class ApiClient {
  private client: AxiosInstance;
  
  constructor(client?: AxiosInstance) {
    this.client = client || apiClient;
  }
  
  // Generic GET request
  async get<T>(url: string, config?: AxiosRequestConfig): Promise<T> {
    const response = await this.client.get<ApiResponse<T>>(url, config);
    return response.data.data;
  }
  
  // Generic POST request
  async post<T>(url: string, data?: any, config?: AxiosRequestConfig): Promise<T> {
    const response = await this.client.post<ApiResponse<T>>(url, data, config);
    return response.data.data;
  }
  
  // Generic PUT request
  async put<T>(url: string, data?: any, config?: AxiosRequestConfig): Promise<T> {
    const response = await this.client.put<ApiResponse<T>>(url, data, config);
    return response.data.data;
  }
  
  // Generic DELETE request
  async delete<T>(url: string, config?: AxiosRequestConfig): Promise<T> {
    const response = await this.client.delete<ApiResponse<T>>(url, config);
    return response.data.data;
  }
  
  // Generic PATCH request
  async patch<T>(url: string, data?: any, config?: AxiosRequestConfig): Promise<T> {
    const response = await this.client.patch<ApiResponse<T>>(url, data, config);
    return response.data.data;
  }
}

// Default API client instance
export const api = new ApiClient();
```

## 🔐 **Authentication API**

### **Auth API Implementation**
```typescript
// services/api/auth.ts
import { api } from './client';

// Types
interface LoginRequest {
  username: string;
  password: string;
}

interface LoginResponse {
  user: {
    id: string;
    username: string;
    role: 'admin' | 'operator' | 'guest';
    permissions: string[];
    lastLogin: string;
  };
  token: string;
  refreshToken: string;
}

interface RefreshTokenRequest {
  refreshToken: string;
}

interface RefreshTokenResponse {
  token: string;
  refreshToken: string;
}

interface UserProfile {
  id: string;
  username: string;
  role: 'admin' | 'operator' | 'guest';
  permissions: string[];
  lastLogin: string;
  email?: string;
  firstName?: string;
  lastName?: string;
}

// Auth API Class
export class AuthApi {
  // Login
  static async login(credentials: LoginRequest): Promise<LoginResponse> {
    return api.post<LoginResponse>('/auth/login', credentials);
  }
  
  // Logout
  static async logout(): Promise<void> {
    return api.post<void>('/auth/logout');
  }
  
  // Refresh token
  static async refreshToken(refreshToken: string): Promise<RefreshTokenResponse> {
    return api.post<RefreshTokenResponse>('/auth/refresh', { refreshToken });
  }
  
  // Get current user profile
  static async getProfile(): Promise<UserProfile> {
    return api.get<UserProfile>('/auth/me');
  }
  
  // Change password
  static async changePassword(currentPassword: string, newPassword: string): Promise<void> {
    return api.post<void>('/auth/change-password', {
      currentPassword,
      newPassword
    });
  }
  
  // Update profile
  static async updateProfile(profile: Partial<UserProfile>): Promise<UserProfile> {
    return api.put<UserProfile>('/auth/profile', profile);
  }
}

// Auth API Hooks
export const useAuthApi = () => {
  return {
    login: AuthApi.login,
    logout: AuthApi.logout,
    refreshToken: AuthApi.refreshToken,
    getProfile: AuthApi.getProfile,
    changePassword: AuthApi.changePassword,
    updateProfile: AuthApi.updateProfile
  };
};
```

## 🖥️ **System API**

### **System API Implementation**
```typescript
// services/api/system.ts
import { api } from './client';

// Types
interface SystemStatus {
  mode: 'IDLE' | 'MOVE' | 'DOCK' | 'FAULT' | 'ESTOP';
  connection: 'ONLINE' | 'OFFLINE' | 'CONNECTING';
  battery: number;
  temperature: number;
  uptime: number;
  lastUpdate: string;
}

interface SystemHealth {
  cpu: number;
  memory: number;
  disk: number;
  network: number;
  temperature: number;
  voltage: number;
  current: number;
}

interface SystemInfo {
  version: string;
  buildDate: string;
  uptime: number;
  hostname: string;
  platform: string;
  architecture: string;
}

interface SystemLog {
  id: string;
  level: 'DEBUG' | 'INFO' | 'WARNING' | 'ERROR' | 'CRITICAL';
  message: string;
  timestamp: string;
  source: string;
  details?: any;
}

// System API Class
export class SystemApi {
  // Get system status
  static async getStatus(): Promise<SystemStatus> {
    return api.get<SystemStatus>('/system/status');
  }
  
  // Get system health
  static async getHealth(): Promise<SystemHealth> {
    return api.get<SystemHealth>('/system/health');
  }
  
  // Get system information
  static async getInfo(): Promise<SystemInfo> {
    return api.get<SystemInfo>('/system/info');
  }
  
  // Get system logs
  static async getLogs(
    level?: SystemLog['level'],
    startTime?: string,
    endTime?: string,
    limit?: number
  ): Promise<SystemLog[]> {
    const params = new URLSearchParams();
    if (level) params.append('level', level);
    if (startTime) params.append('start_time', startTime);
    if (endTime) params.append('end_time', endTime);
    if (limit) params.append('limit', limit.toString());
    
    return api.get<SystemLog[]>(`/system/logs?${params.toString()}`);
  }
  
  // Export system logs
  static async exportLogs(
    format: 'json' | 'csv' | 'txt',
    startTime?: string,
    endTime?: string
  ): Promise<Blob> {
    const params = new URLSearchParams();
    params.append('format', format);
    if (startTime) params.append('start_time', startTime);
    if (endTime) params.append('end_time', endTime);
    
    const response = await apiClient.get(`/system/logs/export?${params.toString()}`, {
      responseType: 'blob'
    });
    
    return response.data;
  }
  
  // Restart system
  static async restart(): Promise<void> {
    return api.post<void>('/system/restart');
  }
  
  // Shutdown system
  static async shutdown(): Promise<void> {
    return api.post<void>('/system/shutdown');
  }
}

// System API Hooks
export const useSystemApi = () => {
  return {
    getStatus: SystemApi.getStatus,
    getHealth: SystemApi.getHealth,
    getInfo: SystemApi.getInfo,
    getLogs: SystemApi.getLogs,
    exportLogs: SystemApi.exportLogs,
    restart: SystemApi.restart,
    shutdown: SystemApi.shutdown
  };
};
```

## 🎮 **Control API**

### **Control API Implementation**
```typescript
// services/api/control.ts
import { api } from './client';

// Types
interface MoveCommand {
  direction: 'forward' | 'backward' | 'left' | 'right';
  speed: number;
  duration?: number;
}

interface SpeedCommand {
  speed: number;
}

interface ModeCommand {
  mode: 'MANUAL' | 'AUTO' | 'SEMI' | 'MAINT';
}

interface ControlResponse {
  success: boolean;
  message: string;
  timestamp: string;
}

// Control API Class
export class ControlApi {
  // Move command
  static async move(command: MoveCommand): Promise<ControlResponse> {
    return api.post<ControlResponse>('/control/move', command);
  }
  
  // Stop command
  static async stop(): Promise<ControlResponse> {
    return api.post<ControlResponse>('/control/stop');
  }
  
  // Emergency stop
  static async emergencyStop(): Promise<ControlResponse> {
    return api.post<ControlResponse>('/control/emergency-stop');
  }
  
  // Set speed
  static async setSpeed(speed: number): Promise<ControlResponse> {
    return api.post<ControlResponse>('/control/speed', { speed });
  }
  
  // Set control mode
  static async setMode(mode: ModeCommand['mode']): Promise<ControlResponse> {
    return api.post<ControlResponse>('/control/mode', { mode });
  }
  
  // Get current control state
  static async getState(): Promise<{
    mode: ModeCommand['mode'];
    speed: number;
    isMoving: boolean;
    direction?: MoveCommand['direction'];
  }> {
    return api.get('/control/state');
  }
  
  // Reset system
  static async reset(): Promise<ControlResponse> {
    return api.post<ControlResponse>('/control/reset');
  }
  
  // Home position
  static async home(): Promise<ControlResponse> {
    return api.post<ControlResponse>('/control/home');
  }
}

// Control API Hooks
export const useControlApi = () => {
  return {
    move: ControlApi.move,
    stop: ControlApi.stop,
    emergencyStop: ControlApi.emergencyStop,
    setSpeed: ControlApi.setSpeed,
    setMode: ControlApi.setMode,
    getState: ControlApi.getState,
    reset: ControlApi.reset,
    home: ControlApi.home
  };
};
```

## 📊 **Telemetry API**

### **Telemetry API Implementation**
```typescript
// services/api/telemetry.ts
import { api } from './client';

// Types
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

interface TelemetryQuery {
  startTime?: string;
  endTime?: string;
  limit?: number;
  interval?: number; // seconds
}

interface TelemetryExport {
  format: 'json' | 'csv' | 'xlsx';
  startTime: string;
  endTime: string;
  metrics: string[];
}

// Telemetry API Class
export class TelemetryApi {
  // Get current telemetry
  static async getCurrent(): Promise<TelemetryData> {
    return api.get<TelemetryData>('/telemetry/current');
  }
  
  // Get historical telemetry
  static async getHistory(query: TelemetryQuery): Promise<TelemetryData[]> {
    const params = new URLSearchParams();
    if (query.startTime) params.append('start_time', query.startTime);
    if (query.endTime) params.append('end_time', query.endTime);
    if (query.limit) params.append('limit', query.limit.toString());
    if (query.interval) params.append('interval', query.interval.toString());
    
    return api.get<TelemetryData[]>(`/telemetry/history?${params.toString()}`);
  }
  
  // Export telemetry data
  static async exportData(exportConfig: TelemetryExport): Promise<Blob> {
    const response = await apiClient.post('/telemetry/export', exportConfig, {
      responseType: 'blob'
    });
    
    return response.data;
  }
  
  // Get telemetry statistics
  static async getStatistics(
    startTime: string,
    endTime: string,
    metrics: string[]
  ): Promise<Record<string, { min: number; max: number; avg: number; sum: number }>> {
    const params = new URLSearchParams();
    params.append('start_time', startTime);
    params.append('end_time', endTime);
    params.append('metrics', metrics.join(','));
    
    return api.get(`/telemetry/statistics?${params.toString()}`);
  }
  
  // Get real-time telemetry stream (Server-Sent Events)
  static getStream(): EventSource {
    const token = useAuthStore.getState().token;
    return new EventSource(`/api/telemetry/stream?token=${token}`);
  }
}

// Telemetry API Hooks
export const useTelemetryApi = () => {
  return {
    getCurrent: TelemetryApi.getCurrent,
    getHistory: TelemetryApi.getHistory,
    exportData: TelemetryApi.exportData,
    getStatistics: TelemetryApi.getStatistics,
    getStream: TelemetryApi.getStream
  };
};
```

## 📋 **Missions API**

### **Missions API Implementation**
```typescript
// services/api/missions.ts
import { api } from './client';

// Types
interface Mission {
  id: string;
  name: string;
  description?: string;
  status: 'PENDING' | 'RUNNING' | 'COMPLETED' | 'FAILED' | 'CANCELLED';
  priority: 'LOW' | 'NORMAL' | 'HIGH' | 'CRITICAL';
  waypoints: Array<{
    id: string;
    position: { x: number; y: number; z: number };
    action?: string;
    parameters?: Record<string, any>;
  }>;
  createdAt: string;
  startedAt?: string;
  completedAt?: string;
  createdBy: string;
  progress: number;
  estimatedDuration?: number;
  actualDuration?: number;
}

interface CreateMissionRequest {
  name: string;
  description?: string;
  priority?: Mission['priority'];
  waypoints: Mission['waypoints'];
}

interface UpdateMissionRequest {
  name?: string;
  description?: string;
  priority?: Mission['priority'];
  waypoints?: Mission['waypoints'];
}

// Missions API Class
export class MissionsApi {
  // Get all missions
  static async getAll(
    status?: Mission['status'],
    limit?: number,
    offset?: number
  ): Promise<{ missions: Mission[]; total: number }> {
    const params = new URLSearchParams();
    if (status) params.append('status', status);
    if (limit) params.append('limit', limit.toString());
    if (offset) params.append('offset', offset.toString());
    
    return api.get(`/missions?${params.toString()}`);
  }
  
  // Get mission by ID
  static async getById(id: string): Promise<Mission> {
    return api.get<Mission>(`/missions/${id}`);
  }
  
  // Create new mission
  static async create(mission: CreateMissionRequest): Promise<Mission> {
    return api.post<Mission>('/missions', mission);
  }
  
  // Update mission
  static async update(id: string, updates: UpdateMissionRequest): Promise<Mission> {
    return api.put<Mission>(`/missions/${id}`, updates);
  }
  
  // Delete mission
  static async delete(id: string): Promise<void> {
    return api.delete<void>(`/missions/${id}`);
  }
  
  // Start mission
  static async start(id: string): Promise<ControlResponse> {
    return api.post<ControlResponse>(`/missions/${id}/start`);
  }
  
  // Pause mission
  static async pause(id: string): Promise<ControlResponse> {
    return api.post<ControlResponse>(`/missions/${id}/pause`);
  }
  
  // Resume mission
  static async resume(id: string): Promise<ControlResponse> {
    return api.post<ControlResponse>(`/missions/${id}/resume`);
  }
  
  // Cancel mission
  static async cancel(id: string): Promise<ControlResponse> {
    return api.post<ControlResponse>(`/missions/${id}/cancel`);
  }
  
  // Get mission logs
  static async getLogs(id: string): Promise<SystemLog[]> {
    return api.get<SystemLog[]>(`/missions/${id}/logs`);
  }
  
  // Duplicate mission
  static async duplicate(id: string, name?: string): Promise<Mission> {
    return api.post<Mission>(`/missions/${id}/duplicate`, { name });
  }
}

// Missions API Hooks
export const useMissionsApi = () => {
  return {
    getAll: MissionsApi.getAll,
    getById: MissionsApi.getById,
    create: MissionsApi.create,
    update: MissionsApi.update,
    delete: MissionsApi.delete,
    start: MissionsApi.start,
    pause: MissionsApi.pause,
    resume: MissionsApi.resume,
    cancel: MissionsApi.cancel,
    getLogs: MissionsApi.getLogs,
    duplicate: MissionsApi.duplicate
  };
};
```

## ⚙️ **Configuration API**

### **Configuration API Implementation**
```typescript
// services/api/config.ts
import { api } from './client';

// Types
interface SafetyConfig {
  maxSpeed: number;
  emergencyStopTimeout: number;
  safetyZones: Array<{
    id: string;
    name: string;
    type: 'RESTRICTED' | 'PROHIBITED';
    boundaries: Array<{ x: number; y: number; z: number }>;
  }>;
  collisionDetection: {
    enabled: boolean;
    sensitivity: number;
    safeDistance: number;
  };
}

interface OperationalConfig {
  defaultSpeed: number;
  acceleration: number;
  deceleration: number;
  turningRadius: number;
  batteryThreshold: number;
  temperatureThreshold: number;
}

interface SystemConfig {
  telemetryInterval: number;
  logLevel: 'DEBUG' | 'INFO' | 'WARNING' | 'ERROR';
  autoBackup: boolean;
  backupInterval: number;
  networkTimeout: number;
}

interface ConfigCategory {
  safety: SafetyConfig;
  operational: OperationalConfig;
  system: SystemConfig;
}

// Configuration API Class
export class ConfigApi {
  // Get all configuration
  static async getAll(): Promise<ConfigCategory> {
    return api.get<ConfigCategory>('/config');
  }
  
  // Get configuration by category
  static async getCategory(category: keyof ConfigCategory): Promise<any> {
    return api.get(`/config/${category}`);
  }
  
  // Update configuration category
  static async updateCategory(
    category: keyof ConfigCategory,
    config: any
  ): Promise<void> {
    return api.put<void>(`/config/${category}`, config);
  }
  
  // Reset configuration to defaults
  static async resetCategory(category: keyof ConfigCategory): Promise<void> {
    return api.post<void>(`/config/${category}/reset`);
  }
  
  // Export configuration
  static async export(): Promise<Blob> {
    const response = await apiClient.get('/config/export', {
      responseType: 'blob'
    });
    
    return response.data;
  }
  
  // Import configuration
  static async import(configFile: File): Promise<void> {
    const formData = new FormData();
    formData.append('config', configFile);
    
    return api.post<void>('/config/import', formData, {
      headers: {
        'Content-Type': 'multipart/form-data'
      }
    });
  }
  
  // Validate configuration
  static async validate(config: any): Promise<{
    valid: boolean;
    errors: string[];
    warnings: string[];
  }> {
    return api.post('/config/validate', config);
  }
  
  // Get configuration schema
  static async getSchema(): Promise<any> {
    return api.get('/config/schema');
  }
}

// Configuration API Hooks
export const useConfigApi = () => {
  return {
    getAll: ConfigApi.getAll,
    getCategory: ConfigApi.getCategory,
    updateCategory: ConfigApi.updateCategory,
    resetCategory: ConfigApi.resetCategory,
    export: ConfigApi.export,
    import: ConfigApi.import,
    validate: ConfigApi.validate,
    getSchema: ConfigApi.getSchema
  };
};
```

## 📡 **WebSocket Client**

### **WebSocket Client Implementation**
```typescript
// services/websocket/client.ts
import { useAuthStore } from '../../stores/auth/authStore';
import { useWebSocketStore } from '../../stores/websocket/websocketStore';
import { handleWebSocketMessage } from './handlers';

// WebSocket Configuration
const WS_CONFIG = {
  url: process.env.REACT_APP_WS_URL || 'ws://localhost:8000/ws',
  reconnectInterval: 1000,
  maxReconnectAttempts: 5,
  heartbeatInterval: 30000
};

// WebSocket Message Types
interface WebSocketMessage {
  type: string;
  payload: any;
  timestamp: string;
  id?: string;
}

interface WebSocketRequest {
  type: string;
  payload: any;
  id: string;
}

interface WebSocketResponse {
  type: string;
  payload: any;
  id: string;
  success: boolean;
  error?: string;
}

// WebSocket Client Class
export class WebSocketClient {
  private ws: WebSocket | null = null;
  private reconnectAttempts = 0;
  private heartbeatInterval: NodeJS.Timeout | null = null;
  private messageQueue: WebSocketRequest[] = [];
  private pendingRequests = new Map<string, {
    resolve: (value: any) => void;
    reject: (error: any) => void;
    timeout: NodeJS.Timeout;
  }>();
  
  constructor() {
    this.connect();
  }
  
  // Connect to WebSocket
  private connect(): void {
    const { setConnecting, setError } = useWebSocketStore.getState();
    
    try {
      const token = useAuthStore.getState().token;
      if (!token) {
        throw new Error('No authentication token');
      }
      
      const url = `${WS_CONFIG.url}?token=${token}`;
      this.ws = new WebSocket(url);
      
      this.ws.onopen = this.handleOpen.bind(this);
      this.ws.onmessage = this.handleMessage.bind(this);
      this.ws.onclose = this.handleClose.bind(this);
      this.ws.onerror = this.handleError.bind(this);
      
      setConnecting(true);
      setError(null);
      
    } catch (error) {
      setConnecting(false);
      setError(error instanceof Error ? error.message : 'WebSocket connection failed');
    }
  }
  
  // Handle WebSocket open
  private handleOpen(): void {
    const { setConnected, setConnecting } = useWebSocketStore.getState();
    
    setConnected(true);
    setConnecting(false);
    this.reconnectAttempts = 0;
    
    // Start heartbeat
    this.startHeartbeat();
    
    // Send queued messages
    this.sendQueuedMessages();
    
    console.log('WebSocket connected');
  }
  
  // Handle WebSocket message
  private handleMessage(event: MessageEvent): void {
    try {
      const data: WebSocketMessage = JSON.parse(event.data);
      
      // Handle response messages
      if (data.id && this.pendingRequests.has(data.id)) {
        const request = this.pendingRequests.get(data.id)!;
        clearTimeout(request.timeout);
        
        if (data.type === 'error') {
          request.reject(new Error(data.payload.message));
        } else {
          request.resolve(data.payload);
        }
        
        this.pendingRequests.delete(data.id);
        return;
      }
      
      // Handle event messages
      handleWebSocketMessage(data);
      
    } catch (error) {
      console.error('Failed to parse WebSocket message:', error);
    }
  }
  
  // Handle WebSocket close
  private handleClose(): void {
    const { setConnected, setConnecting } = useWebSocketStore.getState();
    
    setConnected(false);
    setConnecting(false);
    
    // Stop heartbeat
    this.stopHeartbeat();
    
    // Attempt to reconnect
    if (this.reconnectAttempts < WS_CONFIG.maxReconnectAttempts) {
      const delay = Math.min(
        WS_CONFIG.reconnectInterval * Math.pow(2, this.reconnectAttempts),
        30000
      );
      
      setTimeout(() => {
        this.reconnectAttempts++;
        this.connect();
      }, delay);
    }
    
    console.log('WebSocket disconnected');
  }
  
  // Handle WebSocket error
  private handleError(error: Event): void {
    const { setError } = useWebSocketStore.getState();
    setError('WebSocket connection error');
    console.error('WebSocket error:', error);
  }
  
  // Send message
  public send(type: string, payload: any): void {
    const message: WebSocketMessage = {
      type,
      payload,
      timestamp: new Date().toISOString()
    };
    
    if (this.ws?.readyState === WebSocket.OPEN) {
      this.ws.send(JSON.stringify(message));
    } else {
      // Queue message for later
      this.messageQueue.push(message as WebSocketRequest);
    }
  }
  
  // Send request and wait for response
  public async request(type: string, payload: any, timeout = 5000): Promise<any> {
    return new Promise((resolve, reject) => {
      const id = crypto.randomUUID();
      const message: WebSocketRequest = {
        type,
        payload,
        id
      };
      
      // Set up timeout
      const timeoutId = setTimeout(() => {
        this.pendingRequests.delete(id);
        reject(new Error('Request timeout'));
      }, timeout);
      
      // Store pending request
      this.pendingRequests.set(id, {
        resolve,
        reject,
        timeout: timeoutId
      });
      
      // Send message
      this.send(type, payload);
    });
  }
  
  // Send queued messages
  private sendQueuedMessages(): void {
    while (this.messageQueue.length > 0) {
      const message = this.messageQueue.shift()!;
      this.ws?.send(JSON.stringify(message));
    }
  }
  
  // Start heartbeat
  private startHeartbeat(): void {
    this.heartbeatInterval = setInterval(() => {
      this.send('heartbeat', { timestamp: new Date().toISOString() });
    }, WS_CONFIG.heartbeatInterval);
  }
  
  // Stop heartbeat
  private stopHeartbeat(): void {
    if (this.heartbeatInterval) {
      clearInterval(this.heartbeatInterval);
      this.heartbeatInterval = null;
    }
  }
  
  // Disconnect
  public disconnect(): void {
    this.stopHeartbeat();
    
    if (this.ws) {
      this.ws.close();
      this.ws = null;
    }
    
    // Clear pending requests
    this.pendingRequests.forEach((request) => {
      clearTimeout(request.timeout);
      request.reject(new Error('WebSocket disconnected'));
    });
    this.pendingRequests.clear();
    
    // Clear message queue
    this.messageQueue = [];
  }
  
  // Get connection status
  public isConnected(): boolean {
    return this.ws?.readyState === WebSocket.OPEN;
  }
}

// WebSocket client instance
export const wsClient = new WebSocketClient();
```

### **WebSocket Message Handlers**
```typescript
// services/websocket/handlers.ts
import { useTelemetryStore } from '../../stores/telemetry/telemetryStore';
import { useSystemStore } from '../../stores/system/systemStore';
import { useControlStore } from '../../stores/control/controlStore';
import { useAlertStore } from '../../stores/alerts/alertStore';

// WebSocket message handler
export const handleWebSocketMessage = (data: any): void => {
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
      
    case 'mission:update':
      // Handle mission updates
      break;
      
    case 'config:update':
      // Handle configuration updates
      break;
      
    case 'heartbeat':
      // Handle heartbeat response
      break;
      
    default:
      console.warn('Unknown WebSocket message type:', data.type);
  }
};
```

## 🧪 **Testing Strategy**

### **API Testing**
```typescript
// __tests__/services/api/auth.test.ts
import { AuthApi } from '../../../services/api/auth';
import { apiClient } from '../../../services/api/client';

// Mock axios
jest.mock('../../../services/api/client', () => ({
  apiClient: {
    post: jest.fn(),
    get: jest.fn(),
    put: jest.fn()
  }
}));

describe('AuthApi', () => {
  beforeEach(() => {
    jest.clearAllMocks();
  });
  
  describe('login', () => {
    it('should successfully login user', async () => {
      const mockResponse = {
        data: {
          data: {
            user: { id: '1', username: 'test', role: 'operator' },
            token: 'test-token'
          }
        }
      };
      
      (apiClient.post as jest.Mock).mockResolvedValue(mockResponse);
      
      const result = await AuthApi.login('test', 'password');
      
      expect(apiClient.post).toHaveBeenCalledWith('/auth/login', {
        username: 'test',
        password: 'password'
      });
      expect(result).toEqual(mockResponse.data.data);
    });
    
    it('should handle login failure', async () => {
      const mockError = new Error('Login failed');
      (apiClient.post as jest.Mock).mockRejectedValue(mockError);
      
      await expect(AuthApi.login('test', 'wrong')).rejects.toThrow('Login failed');
    });
  });
});
```

## 🔄 **Changelog**

**v1.0 (2024-12-19)**
- ✅ Created base API client with Axios
- ✅ Implemented authentication API
- ✅ Created system API
- ✅ Implemented control API
- ✅ Added telemetry API
- ✅ Created missions API
- ✅ Implemented configuration API
- ✅ Added WebSocket client
- ✅ Created API testing strategy

---

**Next Steps:**
1. Implement API caching strategies
2. Add API rate limiting
3. Implement API retry logic
4. Add API performance monitoring
5. Create API documentation
