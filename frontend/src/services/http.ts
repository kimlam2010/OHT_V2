/// <reference types="vite/client" />

import { getToken } from '../features/auth/auth'

export type ApiResult<T> =
  | { ok: true; status: number; data: T }
  | { ok: false; status: number; error: string }

function joinUrl(base: string, path: string): string {
  if (!base) return path
  try {
    // Ensure proper joining without double slashes
    const u = new URL(path, base.endsWith('/') ? base : base + '/')
    return u.toString()
  } catch {
    // Fallback simple join
    const b = base.endsWith('/') ? base.slice(0, -1) : base
    const p = path.startsWith('/') ? path : `/${path}`
    return `${b}${p}`
  }
}

export async function apiFetchJson<T>(path: string, init: RequestInit = {}): Promise<ApiResult<T>> {
  const base = (import.meta.env.VITE_API_BASE as string) || '/'
  const url = joinUrl(base, path)

  const headers: Record<string, string> = {
    Accept: 'application/json',
    ...(init.headers as Record<string, string> | undefined),
  }
  if (!('Content-Type' in headers) && init.body) headers['Content-Type'] = 'application/json'

  const token = getToken()
  if (token) headers['Authorization'] = `Bearer ${token}`

  let res: Response
  try {
    res = await fetch(url, { ...init, headers })
  } catch (err) {
    return { ok: false, status: 0, error: (err as Error)?.message || 'Network error' }
  }

  const status = res.status
  let payload: any = null
  const text = await res.text().catch(() => '')
  try { payload = text ? JSON.parse(text) : null } catch { payload = text }

  if (!res.ok) {
    const detail = (payload && (payload.detail || payload.message)) || `HTTP ${status}`
    return { ok: false, status, error: String(detail) }
  }

  return { ok: true, status, data: payload as T }
}


