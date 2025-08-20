import { apiFetchJson } from './http'

export type SystemConfig = {
  version?: number
  hardware?: unknown
  safety?: unknown
  telemetry?: unknown
  comm?: unknown
}

export type ConfigResponse = {
  success: boolean
  message: string
  config?: SystemConfig
  version?: number
}

export async function getConfig(): Promise<SystemConfig | null> {
  const r = await apiFetchJson<SystemConfig>('/api/v1/config/')
  if (!r.ok) return null
  return r.data
}

export async function updateConfig(cfg: SystemConfig, username: string): Promise<boolean> {
  const r = await apiFetchJson<ConfigResponse>(`/api/v1/config/?user=${encodeURIComponent(username)}`, {
    method: 'PUT',
    body: JSON.stringify(cfg)
  })
  if (!r.ok) return false
  return Boolean(r.data?.success)
}

export type ConfigHistoryItem = {
  version: number
  timestamp: string
  action: string
  user: string
  config_data: Record<string, any>
}

export async function getConfigHistory(): Promise<ConfigHistoryItem[] | null> {
  const r = await apiFetchJson<ConfigHistoryItem[]>(`/api/v1/config/history`)
  return r.ok ? r.data : null
}

export async function validateConfig(cfg: SystemConfig): Promise<ConfigResponse | null> {
  const r = await apiFetchJson<ConfigResponse>(`/api/v1/config/validate`, {
    method: 'POST',
    body: JSON.stringify(cfg)
  })
  return r.ok ? r.data : null
}

export async function rollbackConfig(version: number): Promise<ConfigResponse | null> {
  const r = await apiFetchJson<ConfigResponse>(`/api/v1/config/rollback/${encodeURIComponent(String(version))}`, {
    method: 'POST'
  })
  return r.ok ? r.data : null
}

export async function exportConfig(format: 'json'|'yaml' = 'json'): Promise<Blob | null> {
  const base = (import.meta.env.VITE_API_BASE as string) || '/'
  const url = (new URL(`/api/v1/config/export?format=${encodeURIComponent(format)}`, base.endsWith('/')? base : base + '/')).toString()
  try {
    const res = await fetch(url)
    if (!res.ok) return null
    return await res.blob()
  } catch { return null }
}

export async function importConfig(content: string, format: 'json'|'yaml', username: string): Promise<ConfigResponse | null> {
  const r = await apiFetchJson<ConfigResponse>(`/api/v1/config/import?format=${encodeURIComponent(format)}&user=${encodeURIComponent(username)}`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ content })
  })
  return r.ok ? r.data : null
}


