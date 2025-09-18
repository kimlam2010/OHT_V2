import type { AlertItem, LogItem, Performance, Robot, System } from '@/types'
import http from '@/lib/http'

export interface AlertRequest {
  page: number
  page_size: number
  severity?: 'medium' | 'high' | 'critical'
}

export interface AlertResponse {
  alerts: AlertItem[]
  total_count: number
  page: number
  page_size: number
  has_next: boolean
}

export interface LogRequest {
  limit: number
  level: string
  page: number
}

export interface LogResponse {
  logs: LogItem[]
  total_count: number
  page: number
  page_size: number
  has_next: boolean
}

export interface SummaryResponse {
  robot: Robot
  system: System
  performance: Performance
  last_updated: string
}

export const dashboardApi = {
  getSummary: (): Promise<SummaryResponse> => {
    return http.get('/dashboard/summary', {
      headers: {
        'x-api': 'software',
      },
    })
  },
  getActiveAlerts: (params: AlertRequest): Promise<AlertResponse> => {
    return http.get('/dashboard/alerts', {
      params,
      headers: {
        'x-api': 'software',
      },
    })
  },
  getSystemLogs: (params: LogRequest): Promise<LogResponse> => {
    return http.get('/dashboard/logs', {
      params,
      headers: {
        'x-api': 'software',
      },
    })
  },
  acknowledgeAlert: (alertId: number): Promise<void> => {
    return http.post(`/dashboard/alerts/${alertId}/acknowledge`, undefined, {
      headers: {
        'x-api': 'software',
      },
    })
  },
  resolveAlert: (alertId: number): Promise<void> => {
    return http.post(`/dashboard/alerts/${alertId}/resolve`, undefined, {
      headers: {
        'x-api': 'software',
      },
    })
  },
}
