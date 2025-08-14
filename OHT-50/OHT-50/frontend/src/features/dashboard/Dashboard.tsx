import React from 'react'
import { Link } from 'react-router-dom'
import Fe06Chart from '../Fe06Chart'
import ConfigForm from './ConfigForm'
import { sendControlCommand } from '../../services/control'
import { useToast } from '../../components/Toast'
import AppShell from '../../components/AppShell'

export default function Dashboard(){
  const { push } = useToast()
  const [status, setStatus] = React.useState<'Idle'|'Move'|'Dock'|'Fault'|'E-Stop'>('Idle')
  const [wsState, setWsState] = React.useState('connecting')

  React.useEffect(()=>{
    let ws: WebSocket | null = null
    let attempts = 0
    let timer: any
    const url = (import.meta.env.VITE_WS_STATUS_URL as string) || 'ws://localhost:8000/api/v1/telemetry/ws'
    function connect(){
      try {
        ws = new WebSocket(url)
        ws.onopen = ()=>{ setWsState('connected'); attempts = 0 }
        ws.onclose = ()=>{ setWsState('closed'); scheduleReconnect() }
        ws.onerror = ()=>{ setWsState('error'); scheduleReconnect() }
        ws.onmessage = (ev)=>{
          try {
            const msg = JSON.parse(ev.data)
            const s = msg?.status?.state as string | undefined
            if (s) {
              if (['Idle','Move','Dock','Fault','E-Stop'].includes(s)) setStatus(s as any)
            }
          } catch {}
        }
      } catch { scheduleReconnect() }
    }
    function scheduleReconnect(){
      const backoff = Math.min(5000, 500 * Math.pow(2, attempts++))
      clearTimeout(timer)
      timer = setTimeout(connect, backoff)
    }
    connect()
    return ()=>{ clearTimeout(timer); ws?.close() }
  }, [])

  async function confirmAndSend(cmd: Parameters<typeof sendControlCommand>[0], danger=false){
    const msg = danger ? `Xác nhận hành động NGUY HIỂM: ${cmd.toUpperCase()}?` : `Thực hiện lệnh: ${cmd}?`
    const ok = window.confirm(msg)
    if (!ok) return
    const res = await sendControlCommand(cmd)
    if (res.ok) push(`Đã gửi lệnh: ${cmd}`)
    else push(`Lỗi gửi lệnh ${cmd}${res.status ? ` (HTTP ${res.status})` : ''}`)
  }
  return (
    <AppShell>
      <section>
        <div className="grid grid-cols-2 md:grid-cols-4 gap-3 mb-4">
          <div className={`card p-3 border ${status==='Idle'?'':'opacity-60'}`} style={{borderColor:'var(--border)'}}>
            <div className="text-xs text-muted">Trạng thái</div>
            <div className="text-lg font-semibold">{status}</div>
            <div className="text-xs">WS: {wsState}</div>
          </div>
          <div className="card p-3 border" style={{borderColor:'var(--border)'}}>
            <div className="text-xs text-muted">v (m/s)</div>
            <div id="metric-v" className="text-lg font-semibold">—</div>
          </div>
          <div className="card p-3 border" style={{borderColor:'var(--border)'}}>
            <div className="text-xs text-muted">a (m/s²)</div>
            <div id="metric-a" className="text-lg font-semibold">—</div>
          </div>
          <div className="card p-3 border" style={{borderColor:'var(--border)'}}>
            <div className="text-xs text-muted">x (m)</div>
            <div id="metric-x" className="text-lg font-semibold">—</div>
          </div>
        </div>
        <div className="card p-3 border" style={{borderColor:'var(--border)'}}>
          <div className="flex flex-wrap gap-2">
            <button className="btn" onClick={()=>confirmAndSend('start')}>Start</button>
            <button className="btn" onClick={()=>confirmAndSend('stop')}>Stop</button>
            <button className="btn btn-danger" onClick={()=>confirmAndSend('estop', true)}>E‑Stop</button>
            <button className="btn" onClick={()=>confirmAndSend('reset_fault')}>Reset Fault</button>
            <button className="btn" onClick={()=>confirmAndSend('dock')}>Dock</button>
            <button className="btn" onClick={()=>confirmAndSend('undock')}>Undock</button>
            <button className="btn" onClick={()=>confirmAndSend('pause')}>Pause</button>
            <button className="btn" onClick={()=>confirmAndSend('resume')}>Resume</button>
            <button className="btn" onClick={()=>confirmAndSend('auto')}>Auto</button>
            <button className="btn" onClick={()=>confirmAndSend('manual')}>Manual</button>
          </div>
        </div>
        <Fe06Chart onSample={(s)=>{
          const vEl = document.getElementById('metric-v')
          const aEl = document.getElementById('metric-a')
          const xEl = document.getElementById('metric-x')
          if (vEl) vEl.textContent = s.v.toFixed(3)
          if (aEl) aEl.textContent = s.a.toFixed(3)
          if (xEl) xEl.textContent = s.x.toFixed(3)
        }}/>
      </section>
      <section>
        <ConfigForm/>
      </section>
    </AppShell>
  )
}


