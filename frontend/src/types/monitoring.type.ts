export interface LogItem {
  id: number
  level: 'info' | 'warning' | 'error' | 'debug'
  message: string
  source: string
  timestamp: string
  user_id: number | null
  session_id: string | null
  metadata: object
}
