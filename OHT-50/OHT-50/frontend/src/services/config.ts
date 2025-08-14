export type SystemConfig = {
  version?: number
  hardware?: unknown
  safety?: unknown
  telemetry?: unknown
  comm?: unknown
}

export async function getConfig(): Promise<SystemConfig | null> {
  const res = await fetch('/api/v1/config/')
  if (!res.ok) return null
  const data = await res.json()
  return data.data ?? null
}

export async function updateConfig(cfg: SystemConfig): Promise<boolean> {
  const res = await fetch('/api/v1/config/', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(cfg)
  })
  return res.ok
}


