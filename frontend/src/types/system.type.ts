export interface System {
  total_alerts: number
  active_alerts: number
  critical_alerts: number
  system_uptime: number
  total_users: number
  active_users: number
  last_backup: string
  database_size: number
}

export interface SystemHealthData {
  status: 'online' | 'warning' | 'error' | 'offline'
  error_rate: number
  response_time_p95: number
  throughput: number
  last_scan: string
  total_modules: number
  active_modules: number
  failed_modules: number
}
