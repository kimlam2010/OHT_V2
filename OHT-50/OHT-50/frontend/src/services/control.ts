import { apiFetchJson } from './http'
export type ControlCommand =
  | 'start'
  | 'stop'
  | 'estop'
  | 'reset_fault'
  | 'dock'
  | 'undock'
  | 'pause'
  | 'resume'
  | 'auto'
  | 'manual'

export type ControlResponse = {
  ok: boolean
  status?: number
  message?: string
}

export async function sendControlCommand(command: ControlCommand): Promise<ControlResponse> {
  const r = await apiFetchJson<any>('/api/v1/control/command', {
    method: 'POST',
    body: JSON.stringify({ command })
  })
  if (!r.ok) {
    return { ok: false, status: r.status, message: r.error }
  }
  return { ok: true, status: r.status }
}


