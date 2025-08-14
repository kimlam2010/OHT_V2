import React from 'react'

export type RealtimeSample = { v: number; a: number; x: number }

export default function Fe06Chart({ onSample }: { onSample?: (s: RealtimeSample) => void }) {
  const WIDTH = 800, HEIGHT = 256, N = 600
  const [bufV] = React.useState<number[]>(Array(N).fill(0))
  const [bufA] = React.useState<number[]>(Array(N).fill(0))
  const [bufX] = React.useState<number[]>(Array(N).fill(0))
  const [wsState, setWsState] = React.useState('connecting')

  function pushSample(s: RealtimeSample){
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
    let timer: any
    const url = (import.meta.env.VITE_WS_URL as string) || 'ws://localhost:8000/api/v1/telemetry/ws'
    function connect(){
      try {
        ws = new WebSocket(url)
        ws.onopen = ()=>{ setWsState('connected'); attempts = 0 }
        ws.onclose = ()=>{ setWsState('closed'); scheduleReconnect() }
        ws.onerror = ()=>{ setWsState('error'); scheduleReconnect() }
        ws.onmessage = (ev)=>{
          try {
            const msg = JSON.parse(ev.data)
            const s: RealtimeSample = {
              v: (msg.status?.vel_mms ?? 0) / 1000.0,
              a: (msg.status?.acc_mms2 ?? 0) / 1000.0,
              x: (msg.status?.pos_mm ?? 0) / 1000.0,
            }
            pushSample(s)
            onSample?.(s)
            setTick(t=>t+1)
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

  const pathV = mapPoints(bufV, 0.5, 128)
  const pathA = mapPoints(bufA, 0.4, 128)
  const pathX = mapPoints(bufX, 2.0, 128)

  return (
    <div>
      <div className="text-sm mb-2">WS: {wsState}</div>
      <svg viewBox={`0 0 ${WIDTH} ${HEIGHT}`} className="w-full h-64 border rounded" style={{borderColor: 'var(--border)'}}>
        <polyline points={pathV} fill="none" stroke="currentColor" strokeWidth={2} className="chart-color-1" />
        <polyline points={pathA} fill="none" stroke="currentColor" strokeWidth={2} className="chart-color-3" />
        <polyline points={pathX} fill="none" stroke="currentColor" strokeWidth={2} className="chart-color-4" />
      </svg>
    </div>
  )
}


