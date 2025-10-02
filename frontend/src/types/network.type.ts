export interface NetworkHealth {
  status: string
  service: string
  firmware_connected: boolean
  connection_errors: number
  last_heartbeat: string | null
  uptime_seconds: number | null
  memory_usage_percent: number | null
  cpu_usage_percent: number | null
}
