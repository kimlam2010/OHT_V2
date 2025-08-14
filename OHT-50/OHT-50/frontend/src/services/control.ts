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
  try {
    const res = await fetch('/api/v1/control/command', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ command })
    })
    if (!res.ok) {
      return { ok: false, status: res.status, message: `HTTP ${res.status}` }
    }
    return { ok: true, status: res.status }
  } catch (err) {
    return { ok: false, message: (err as Error)?.message }
  }
}


