export const moduleType = ['unknown', 'power', 'safety', 'travel', 'dock_location', 'master', 'lifter', 'cargo'] as const
export type ModuleType = (typeof moduleType)[number]

export interface ModuleRealTime {
  battery: number
  voltage: number
  current: number
  temperature: number
  timestamp: string
}

export interface ModuleData {
  address: number
  addr: string
  name: string
  type: ModuleType
  status: 'healthy' | 'warning' | 'error' | 'lost' | 'offline'
  last_seen: string
  last_seen_ms: number
  error_rate: number
  response_time: number
  fw_version: string
  mandatory: boolean
  capabilities: number
  version: string
  real_time: ModuleRealTime
}

export interface DiscoveryData {
  is_running: boolean
  progress: number
  status_message: string
  modules_found: number
  conflicts: string[]
  start_time: string | null
  end_time: string | null
}

export interface DiscoveryDevice {
  address: number
  addr: string
  name: string
  type: ModuleType
  found: boolean
  conflict: boolean
  response_time: number | null
}

export interface TelemetryData {
  module_addr: string
  module_name: string
  registers: TelemetryRegister[]
  last_updated: string
}

export interface TelemetryRegister {
  address: string
  name: string
  value: number
  unit: string
  mode: 'R' | 'RW' | 'W'
  status: 'normal' | 'warning' | 'critical'
  threshold_warning: number | null
  threshold_critical: number | null
}
