import { useMutation, useQuery } from '@tanstack/react-query'
import { moduleApi } from '@/api/module'

// Export query keys for reuse
export const MODULES_QUERY_KEY = 'modules'
export const BUS_HEALTH_QUERY_KEY = 'bus-health'
export const MODULE_QUERY_KEY = 'module'
export const DISCOVERY_STATUS_QUERY_KEY = 'discovery-status'
export const DISCOVERY_RESULT_QUERY_KEY = 'discovery-result'
export const TELEMETRY_QUERY_KEY = 'telemetry'

export function useModulesQuery() {
  return useQuery({
    queryKey: [MODULES_QUERY_KEY],
    queryFn: moduleApi.getModules,
  })
}

export function useBusHealthQuery() {
  return useQuery({
    queryKey: [BUS_HEALTH_QUERY_KEY],
    queryFn: moduleApi.getBusHealth,
  })
}

export function useModuleQuery(address: number) {
  return useQuery({
    queryKey: [MODULE_QUERY_KEY, address],
    queryFn: () => moduleApi.getModule(address),
    enabled: !!address,
  })
}

export function useStartDiscoveryMutation() {
  return useMutation({
    mutationFn: (options?: { doNotShowLoading?: boolean }) => moduleApi.startDiscovery(options),
  })
}

export function useDiscoveryStatusQuery(enabled?: boolean, refetchInterval?: number, options?: { doNotShowLoading?: boolean }) {
  return useQuery({
    queryKey: [DISCOVERY_STATUS_QUERY_KEY],
    queryFn: () => moduleApi.statusDiscovery(options),
    enabled,
    refetchInterval,
    staleTime: 0,
  })
}

export function useDiscoveryResultQuery(options?: { doNotShowLoading?: boolean }) {
  return useQuery({
    queryKey: [DISCOVERY_RESULT_QUERY_KEY],
    queryFn: () => moduleApi.getDiscoveryResult(options),
    enabled: false,
  })
}

export function useTelemetryQuery(address: number) {
  return useQuery({
    queryKey: [TELEMETRY_QUERY_KEY, address],
    queryFn: () => moduleApi.getTelemetry(address),
    enabled: !!address,
    staleTime: 0,
  })
}

export function useUpdateTelemetryMutation() {
  return useMutation({
    mutationFn: moduleApi.updateTelemetry,
  })
}
