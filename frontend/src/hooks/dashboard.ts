import { useMutation, useQuery } from '@tanstack/react-query'
import { dashboardApi } from '@/api/dashboard'
import { STALE_TIME } from '@/plugins/react-query'

export const DASHBOARD_SUMMARY_QUERY_KEY = 'dashboard-summary'
export const SYSTEM_LOG_QUERY_KEY = 'system-logs'
export const ACTIVE_ALERT_QUERY_KEY = 'active-alerts'

export function useDashboardSummaryQuery() {
  return useQuery({
    queryKey: [DASHBOARD_SUMMARY_QUERY_KEY],
    queryFn: dashboardApi.getSummary,
  })
}

export function useSystemLogQuery(limit: number, level: string, page: number, { staleTime = STALE_TIME }: { staleTime?: number } = {}) {
  return useQuery({
    queryKey: [SYSTEM_LOG_QUERY_KEY, limit, level, page],
    queryFn: () => dashboardApi.getSystemLogs({ limit, level, page }),
    staleTime,
  })
}

export function useActiveAlertQuery(page_size: number, page: number, { staleTime = STALE_TIME }: { staleTime?: number } = {}) {
  return useQuery({
    queryKey: [ACTIVE_ALERT_QUERY_KEY, page_size, page],
    queryFn: () => dashboardApi.getActiveAlerts({ page_size, page }),
    staleTime,
  })
}

export function useAcknowledgeAlertMutation() {
  return useMutation({
    mutationFn: dashboardApi.acknowledgeAlert,
  })
}

export function useResolveAlertMutation() {
  return useMutation({
    mutationFn: dashboardApi.resolveAlert,
  })
}
