import { useQuery } from '@tanstack/react-query'
import { networkApi } from '@/api/network'

export const NETWORK_HEALTH_QUERY_KEY = 'network-health'

export function useNetworkHealth() {
  return useQuery({
    queryKey: [NETWORK_HEALTH_QUERY_KEY],
    queryFn: networkApi.getNetworkHealth,
  })
}
