import React from 'react'
import { Link } from 'react-router-dom'
import Fe06Chart from '../Fe06Chart'
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
        {/* Status + Controls & Quick Metrics (side-by-side) */}
        <div className="card p-4 border mb-6" style={{borderColor:'var(--border)'}}>
          <div className="flex flex-wrap gap-2 items-center">
            <span className="badge badge-idle">Idle</span>
            <span className="badge badge-move">Move</span>
            <span className="badge badge-dock">Dock</span>
            <span className="badge badge-fault">Fault</span>
            <span className="badge badge-estop">E‑Stop</span>
            <span className="muted text-sm ml-auto">Trạng thái hiện tại: <strong>{status}</strong> • WS: {wsState}</span>
          </div>
        </div>

        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6 mb-6">
          {/* Controls */}
          <div className="card p-4 border" style={{borderColor:'var(--border)'}}>
            <h2 className="text-lg font-semibold mb-3">Điều khiển</h2>
            <div className="flex flex-wrap items-center gap-2">
              <button className="btn btn-primary" onClick={()=>confirmAndSend('start')}>Start</button>
              <button className="btn" onClick={()=>confirmAndSend('stop')}>Stop</button>
              <button className="btn btn-danger" onClick={()=>confirmAndSend('estop', true)}>E‑Stop</button>
              <button className="btn" onClick={()=>confirmAndSend('reset_fault')}>Reset Fault</button>
              <div className="w-px h-6" style={{background:'var(--border)'}}></div>
              <button className="btn" onClick={()=>confirmAndSend('dock')}>Dock</button>
              <button className="btn" onClick={()=>confirmAndSend('undock')}>Undock</button>
              <div className="w-px h-6" style={{background:'var(--border)'}}></div>
              <button className="btn" onClick={()=>confirmAndSend('pause')}>Pause</button>
              <button className="btn" onClick={()=>confirmAndSend('resume')}>Resume</button>
              <button className="btn" onClick={()=>confirmAndSend('auto')}>Auto</button>
              <button className="btn" onClick={()=>confirmAndSend('manual')}>Manual</button>
            </div>
          </div>

          {/* Quick Metrics */}
          <div className="card p-4 border" style={{borderColor:'var(--border)'}}>
            <h2 className="text-lg font-semibold mb-3">Thông số nhanh</h2>
            <div className="grid grid-cols-2 sm:grid-cols-3 gap-3">
              <div className="rounded ring p-3">
                <div className="text-sm muted">v (m/s)</div>
                <div className="text-xl font-semibold"><span id="valV">0.00</span></div>
              </div>
              <div className="rounded ring p-3">
                <div className="text-sm muted">a (m/s²)</div>
                <div className="text-xl font-semibold"><span id="valA">0.00</span></div>
              </div>
              <div className="rounded ring p-3">
                <div className="text-sm muted">x (m)</div>
                <div className="text-xl font-semibold"><span id="valX">0.00</span></div>
              </div>
              <div className="rounded ring p-3 col-span-2 sm:col-span-1">
                <div className="text-sm muted">Liên kết</div>
                <div className="text-base">RS485: OK • Center: OK</div>
              </div>
            </div>
          </div>
        </div>

        {/* bỏ 2 card thống kê/định vị theo yêu cầu */}

        {/* Biểu đồ VAX (trái) + Thông tin định vị (phải) */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6 mb-6">
          {/* Left: Chart */}
          <div>
            <Fe06Chart onSample={(s)=>{
              const vEl = document.getElementById('metric-v')
              const aEl = document.getElementById('metric-a')
              const xEl = document.getElementById('metric-x')
              if (vEl) vEl.textContent = s.v.toFixed(3)
              if (aEl) aEl.textContent = s.a.toFixed(3)
              if (xEl) xEl.textContent = s.x.toFixed(3)
            }}/>
          </div>
          {/* Right: Location info */}
          <div className="card p-4">
            <h3 className="text-lg font-semibold mb-3">Thông tin định vị</h3>
            <div className="grid grid-cols-2 gap-3 text-sm">
              <div className="rounded ring p-3 col-span-2">
                <div className="muted">Heading</div>
                <div><span id="heading">72</span>°</div>
              </div>
              <div className="rounded ring p-3">
                <div className="muted">RFID gần nhất</div>
                <div><span id="rfid">A12</span> @ <span id="rfidTime">--:--:--</span></div>
              </div>
              <div className="rounded ring p-3">
                <div className="muted">Vị trí x (encoder sau reset)</div>
                <div><span id="posX">0.00</span> m</div>
              </div>
              <div className="rounded ring p-3 col-span-2">
                <div className="muted">Mini‑map LiDAR (OP)</div>
                <div className="h-28 border rounded" style={{borderColor: 'var(--border)', background: 'var(--card)'}}></div>
              </div>
              <div className="rounded ring p-3 col-span-2">
                <div className="muted">Kết nối</div>
                <div>Center: OK • RS485: OK</div>
              </div>
            </div>
          </div>
        </div>

      </section>
    </AppShell>
  )
}


