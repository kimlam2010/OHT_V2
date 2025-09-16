export interface Robot {
  robot_id: string
  status: string
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
