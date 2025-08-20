/// <reference types="vite/client" />
import React from 'react'

export type RealtimeSample = { v: number; a: number; x: number; tagId?: string | number; state?: string }

export default function Fe06Chart({ onSample, showStats=true }: { onSample?: (s: RealtimeSample) => void; showStats?: boolean }) {
  const WIDTH = 800, HEIGHT = 256, N = 600, HZ = 10
  const [bufV] = React.useState<number[]>(Array(N).fill(0))
  const [bufA] = React.useState<number[]>(Array(N).fill(0))
  const [bufX] = React.useState<number[]>(Array(N).fill(0))
  const [wsState, setWsState] = React.useState('connecting')
  const [paused, setPaused] = React.useState(false)
  const pausedRef = React.useRef(false)
  const [series, setSeries] = React.useState({ v: true, a: true, x: true })
  const [range, setRange] = React.useState<'10'|'30'|'60'|'live'>('30')
  const [ov, setOv] = React.useState({ rfid: true, enc: true, state: true })
  const [stats, setStats] = React.useState({ v:{min:0,avg:0,max:0}, a:{min:0,avg:0,max:0}, x:{min:0,avg:0,max:0} })

  type Marker = { idx: number; type: 'rfid'|'enc'|'state'; label?: string }
  const markersRef = React.useRef<Marker[]>([])
  const sampleIdxRef = React.useRef(0)
  const prevStateRef = React.useRef<string | undefined>(undefined)

  // Keep a stopped flag to prevent reconnect after unmount
  const stoppedRef = React.useRef(false)

  function pushSample(s: RealtimeSample){
    if (pausedRef.current) return
    bufV.push(s.v); bufV.shift()
    bufA.push(s.a); bufA.shift()
    bufX.push(s.x); bufX.shift()
  }
  function mapPoints(arr: number[], scaleY=1, offsetY=128){
    const stepX = WIDTH / (arr.length - 1)
    return arr.map((v,i)=>`${(i*stepX).toFixed(1)},${(offsetY - v*scaleY).toFixed(1)}`).join(' ')
  }

  const [tick, setTick] = React.useState(0)
  React.useEffect(()=>{
    let ws: WebSocket | null = null
    let attempts = 0
    let timer: ReturnType<typeof setTimeout> | null = null
    stoppedRef.current = false
    const envUrl = (import.meta.env.VITE_WS_URL as string | undefined)
    const url = envUrl && envUrl.length > 0 ? envUrl : 'ws://localhost:8000/api/v1/telemetry/ws'

    function pruneMarkers(){
      const minIdx = Math.max(0, sampleIdxRef.current - N)
      if (markersRef.current.length > N * 4){
        markersRef.current = markersRef.current.filter(m => m.idx >= minIdx)
      }
    }

    function scheduleReconnect(){
      if (stoppedRef.current) return
      const backoff = Math.min(5000, 500 * Math.pow(2, attempts++))
      if (timer) clearTimeout(timer)
      timer = setTimeout(connect, backoff)
    }

    function connect(){
      if (stoppedRef.current) return
      try {
        ws = new WebSocket(url)
        ws.onopen = ()=>{ setWsState('connected'); attempts = 0 }
        ws.onclose = ()=>{ setWsState('closed'); scheduleReconnect() }
        ws.onerror = ()=>{ setWsState('error'); scheduleReconnect() }
        ws.onmessage = (ev)=>{
          try {
            if (pausedRef.current) return
            const msg = JSON.parse(ev.data)
            const s: RealtimeSample = {
              v: (msg.status?.vel_mms ?? 0) / 1000.0,
              a: (msg.status?.acc_mms2 ?? 0) / 1000.0,
              x: (msg.status?.pos_mm ?? 0) / 1000.0,
              tagId: msg.location?.tag_id,
              state: msg.status?.state,
            }
            pushSample(s)
            // Detect overlays
            sampleIdxRef.current += 1
            if (msg.location?.tag_id) markersRef.current.push({ idx: sampleIdxRef.current, type: 'rfid', label: String(msg.location.tag_id) })
            if (msg.events?.enc_reset) markersRef.current.push({ idx: sampleIdxRef.current, type: 'enc' })
            const curState = msg.status?.state as string | undefined
            if (curState && curState !== prevStateRef.current){
              markersRef.current.push({ idx: sampleIdxRef.current, type: 'state', label: curState })
              prevStateRef.current = curState
            }
            pruneMarkers()
            onSample?.(s)
            setTick(t=>t+1)
          } catch {}
        }
      } catch { scheduleReconnect() }
    }

    connect()
    return ()=>{
      stoppedRef.current = true
      if (timer) clearTimeout(timer)
      if (ws){
        try {
          ws.onopen = null as any
          ws.onclose = null as any
          ws.onerror = null as any
          ws.onmessage = null as any
        } catch {}
        try { ws.close() } catch {}
        ws = null
      }
    }
  }, [])

  // Sync paused state into ref so WS handler can read without re-subscribing
  React.useEffect(()=>{ pausedRef.current = paused }, [paused])

  // Determine window length from range selection
  const win = range === 'live' ? N : Math.min(N, parseInt(range,10) * HZ)
  const winV = bufV.slice(-win)
  const winA = bufA.slice(-win)
  const winX = bufX.slice(-win)

  // Auto-scale to utilize vertical space when values are small
  function computeScale(arr: number[], base: number){
    const maxAbs = Math.max(0.001, Math.max(...arr.map(v=> Math.abs(v))))
    return base / maxAbs
  }
  const scaleV = computeScale(winV, 64)
  const scaleA = computeScale(winA, 51.2)
  const scaleX = computeScale(winX, 96)

  const pathV = mapPoints(winV, scaleV, 128)
  const pathA = mapPoints(winA, scaleA, 128)
  const pathX = mapPoints(winX, scaleX, 128)

  // Stats for window
  React.useEffect(()=>{
    function summarize(arr: number[]){
      const min = Math.min(...arr)
      const max = Math.max(...arr)
      const avg = arr.length ? arr.reduce((s,v)=> s+v,0)/arr.length : 0
      return { min, avg, max }
    }
    setStats({ v: summarize(winV), a: summarize(winA), x: summarize(winX) })
  // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [tick, range])

  function exportCSV(){
    let csv = 't,v,a,x\n'
    for (let i = 0; i < N; i++) csv += `${i},${bufV[i]},${bufA[i]},${bufX[i]}\n`
    const a = document.createElement('a')
    a.href = 'data:text/plain;charset=utf-8,' + encodeURIComponent(csv)
    a.download = 'vax.csv'
    a.style.display = 'none'
    document.body.appendChild(a); a.click(); document.body.removeChild(a)
  }
  function exportJSON(){
    const blob = JSON.stringify({ v: bufV, a: bufA, x: bufX })
    const a = document.createElement('a')
    a.href = 'data:text/plain;charset=utf-8,' + encodeURIComponent(blob)
    a.download = 'vax.json'
    a.style.display = 'none'
    document.body.appendChild(a); a.click(); document.body.removeChild(a)
  }

  return (
    <div className="card p-4 border" style={{borderColor:'var(--border)'}}>
      <div className="flex items-center justify-between mb-3">
        <h2 className="text-lg font-semibold">Đồ thị v‑a‑x (realtime)</h2>
        <div className="text-sm muted">WS: <span>{wsState}</span></div>
      </div>
      {wsState !== 'connected' && (
        <div className="mb-3 text-sm card p-2 border" style={{borderColor:'var(--border)'}}>Mất kết nối WS, đang thử kết nối lại…</div>
      )}
      <div className="flex flex-wrap items-center gap-2 mb-3">
        <div className="flex items-center gap-1" role="group" aria-label="Channels">
          <button className={`btn ${series.v?'btn-secondary':'btn-ghost'}`} onClick={()=> setSeries(s=>({...s, v: !s.v}))}>v</button>
          <button className={`btn ${series.a?'btn-secondary':'btn-ghost'}`} onClick={()=> setSeries(s=>({...s, a: !s.a}))}>a</button>
          <button className={`btn ${series.x?'btn-secondary':'btn-ghost'}`} onClick={()=> setSeries(s=>({...s, x: !s.x}))}>x</button>
        </div>
        <div className="w-px h-6" style={{background: 'var(--border)'}}></div>
        <div className="flex items-center gap-1" role="group" aria-label="Range">
          {(['10','30','60','live'] as const).map(r => (
            <button key={r} className={`btn ${range===r?'btn-secondary':'btn-ghost'}`} onClick={()=> setRange(r)}>{r}</button>
          ))}
        </div>
        <div className="w-px h-6" style={{background: 'var(--border)'}}></div>
        <button className="btn btn-ghost" type="button" onClick={()=> setPaused(p=>!p)}>{paused ? 'Resume' : 'Pause'}</button>
        <button className="btn btn-ghost" type="button" aria-label="Zoom in">+</button>
        <button className="btn btn-ghost" type="button" aria-label="Zoom out">−</button>
        <div className="w-px h-6" style={{background: 'var(--border)'}}></div>
        <div className="flex items-center gap-1" role="group" aria-label="Overlays">
          <button className={`btn ${ov.rfid?'btn-secondary':'btn-ghost'}`} onClick={()=> setOv(v=>({...v, rfid: !v.rfid}))}>RFID</button>
          <button className={`btn ${ov.enc?'btn-secondary':'btn-ghost'}`} onClick={()=> setOv(v=>({...v, enc: !v.enc}))}>Enc.Reset</button>
          <button className={`btn ${ov.state?'btn-secondary':'btn-ghost'}`} onClick={()=> setOv(v=>({...v, state: !v.state}))}>State</button>
        </div>
        <div className="ml-auto flex items-center gap-2">
          <button className="btn btn-ghost" type="button" onClick={exportCSV}>Export CSV</button>
          <button className="btn btn-ghost" type="button" onClick={exportJSON}>Export JSON</button>
        </div>
      </div>
      <svg viewBox={`0 0 ${WIDTH} ${HEIGHT}`} className="w-full h-64 border rounded" style={{borderColor: 'var(--border)'}} role="img" aria-label="Realtime chart v a x">
        <rect x="0" y="0" width={WIDTH} height={HEIGHT} fill="transparent" />
        <g stroke="var(--border)" strokeWidth="1" opacity="0.5">
          <line x1="0" y1="128" x2={WIDTH} y2="128"></line>
          <line x1="0" y1="64" x2={WIDTH} y2="64"></line>
          <line x1="0" y1="192" x2={WIDTH} y2="192"></line>
        </g>
        {series.v && <polyline points={pathV} fill="none" stroke="currentColor" strokeWidth={2} className="chart-color-1" strokeLinejoin="round" strokeLinecap="round" />}
        {series.a && <polyline points={pathA} fill="none" stroke="currentColor" strokeWidth={2} className="chart-color-3" strokeLinejoin="round" strokeLinecap="round" />}
        {series.x && <polyline points={pathX} fill="none" stroke="currentColor" strokeWidth={2} className="chart-color-4" strokeLinejoin="round" strokeLinecap="round" />}
        {/* Overlays */}
        {(()=>{
          const startIdx = Math.max(0, sampleIdxRef.current - win)
          const elems: JSX.Element[] = []
          const toX = (idx:number)=> (WIDTH * (idx - startIdx)) / (win - 1)
          if (ov.rfid) markersRef.current.filter(m=> m.type==='rfid' && m.idx>=startIdx).forEach((m,i)=>{
            const x = toX(m.idx)
            elems.push(<g key={`rfid-${i}`}>
              <circle cx={x} cy={24} r={3} fill="currentColor" className="chart-color-2" />
              {m.label && <text x={x+4} y={28} fontSize="10" fill="var(--foreground)">{m.label}</text>}
            </g>)
          })
          if (ov.enc) markersRef.current.filter(m=> m.type==='enc' && m.idx>=startIdx).forEach((m,i)=>{
            const x = toX(m.idx)
            elems.push(<rect key={`enc-${i}`} x={x-2} y={HEIGHT-28} width={4} height={12} fill="currentColor" className="chart-color-5" />)
          })
          if (ov.state) markersRef.current.filter(m=> m.type==='state' && m.idx>=startIdx).forEach((m,i)=>{
            const x = toX(m.idx)
            elems.push(<g key={`state-${i}`}>
              <path d={`M${x},40 l4,8 l-8,0 z`} fill="currentColor" className="chart-color-4" />
              {m.label && <text x={x+6} y={48} fontSize="10" fill="var(--foreground)">{m.label}</text>}
            </g>)
          })
          return <g>{elems}</g>
        })()}
      </svg>
      {showStats && (
        <div className="grid grid-cols-3 gap-3 text-sm mt-3">
          <div className="rounded ring p-2"><div className="muted">v (m/s)</div><div>min: {stats.v.min.toFixed(2)} • avg: {stats.v.avg.toFixed(2)} • max: {stats.v.max.toFixed(2)}</div></div>
          <div className="rounded ring p-2"><div className="muted">a (m/s²)</div><div>min: {stats.a.min.toFixed(2)} • avg: {stats.a.avg.toFixed(2)} • max: {stats.a.max.toFixed(2)}</div></div>
          <div className="rounded ring p-2"><div className="muted">x (m)</div><div>min: {stats.x.min.toFixed(2)} • avg: {stats.x.avg.toFixed(2)} • max: {stats.x.max.toFixed(2)}</div></div>
        </div>
      )}
    </div>
  )
}


