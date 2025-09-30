import type { DiscoveryData, DiscoveryDevice, HttpResponse, ModuleData, SystemHealthData, TelemetryData } from '@/types'
import http from '@/lib/http'

interface UpdateTelemetryRequest {
  address: number
  register_address: string
  value: number
  force: boolean
}

export const moduleApi = {
  getBusHealth: (): Promise<HttpResponse<SystemHealthData>> => {
    return http.get('/rs485/bus/health', {
      headers: {
        'x-api': 'software',
      },
    })
  },
  getModules: (): Promise<HttpResponse<ModuleData[]>> => {
    return http.get('/rs485/modules', {
      headers: {
        'x-api': 'software',
      },
    })
  },
  getModule: (address: number): Promise<HttpResponse<ModuleData>> => {
    return http.get(`/rs485/modules/${address}`, {
      headers: {
        'x-api': 'software',
      },
    })
  },
  startDiscovery: (options?: { doNotShowLoading?: boolean }): Promise<HttpResponse<DiscoveryData>> => {
    return http.post('/rs485/discovery/start', undefined, {
      doNotShowLoading: options?.doNotShowLoading,
      headers: {
        'x-api': 'software',
      },
    })
  },
  statusDiscovery: (options?: { doNotShowLoading?: boolean }): Promise<HttpResponse<DiscoveryData>> => {
    return http.get('/rs485/discovery/status', {
      doNotShowLoading: options?.doNotShowLoading,
      headers: {
        'x-api': 'software',
      },
    })
  },
  getDiscoveryResult: (options?: { doNotShowLoading?: boolean }): Promise<HttpResponse<DiscoveryDevice[]>> => {
    return http.get('/rs485/discovery/results', {
      doNotShowLoading: options?.doNotShowLoading,
      headers: {
        'x-api': 'software',
      },
    })
  },
  getTelemetry: (address: number): Promise<HttpResponse<TelemetryData>> => {
    return http.get(`/rs485/modules/${address}/telemetry`, {
      headers: {
        'x-api': 'software',
      },
    })
  },
  updateTelemetry: (data: UpdateTelemetryRequest): Promise<HttpResponse<TelemetryData>> => {
    const { address, ...rest } = data
    return http.post(`/rs485/modules/${address}/telemetry`, rest, {
      headers: {
        'x-api': 'software',
      },
    })
  },
}
