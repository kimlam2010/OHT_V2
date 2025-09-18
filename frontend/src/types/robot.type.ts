export interface Robot {
  robot_id: string
  status: 'idle' | 'moving' | 'docking' | 'charging' | 'error' | 'emergency_stop' | 'paused'
  mode: string
  position: {
    x: number
    y: number
    z: number
  }
  battery_level: number
  temperature: number
  speed: number
  last_command: string
  uptime: number
  health_score: number
}
