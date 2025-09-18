export interface SafetyStatusResponse {
  status: string
  emergency_stop: boolean
  obstacles_detected: boolean
  temperature_normal: boolean
  timestamp: string
}

export interface AlertItem {
  id: number
  title: string
  message: string
  severity: 'medium' | 'high' | 'critical'
  status: 'active' | 'inactive' | 'resolved'
  source: string
  timestamp: string
  acknowledged_by: string | null
  acknowledged_at: string | null
  resolved_at: string | null
  metadata: {
    battery_level: number
    threshold: number
  }
}
