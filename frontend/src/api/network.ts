import type { NetworkHealth } from './../types/network.type'
import http from '@/lib/http'

interface NetworkHealthResponse {
  success: boolean
  data: NetworkHealth | null
  error: string | null
  timestamp: string
}

export const networkApi = {
  getNetworkHealth: (): Promise<NetworkHealthResponse> => {
    return http.get('/network/health', {
      headers: {
        'x-api': 'software',
      },
    })
  },
}
