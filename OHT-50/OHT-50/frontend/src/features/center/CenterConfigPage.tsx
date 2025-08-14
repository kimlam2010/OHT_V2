import React from 'react'
import AppShell from '../../components/AppShell'

export default function CenterConfigPage(){
  const [latency, setLatency] = React.useState('-')
  return (
    <AppShell>
      <div className="space-y-6">
        <section className="grid grid-cols-1 lg:grid-cols-3 gap-6">
          <div className="card p-4 lg:col-span-2">
            <h2 className="text-lg font-semibold mb-3">Thông số kết nối</h2>
            <div className="grid gap-3">
              <label className="text-sm">Base URL<input className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} placeholder="http://localhost:8000"/></label>
              <label className="text-sm">WebSocket URL<input className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} placeholder="ws://localhost:8000/ws/center"/></label>
              <label className="text-sm">API Key<input className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} placeholder="***"/></label>
              <label className="text-sm">Heartbeat (s)<input type="number" className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} defaultValue={2}/></label>
              <div className="flex gap-2 justify-end mt-2">
                <button className="btn btn-ghost" type="button" onClick={()=> setLatency((Math.random()*20+5).toFixed(1))}>Test Connect</button>
                <button className="btn btn-secondary" type="button">Lưu</button>
                <button className="btn btn-primary" type="button">Áp dụng</button>
              </div>
            </div>
          </div>
          <div className="card p-4 lg:col-span-1">
            <h2 className="text-lg font-semibold mb-3">Trạng thái kết nối</h2>
            <div className="grid gap-2 text-sm">
              <div>Center: <span className="badge"><span className="inline-block w-2 h-2 rounded-full" style={{background:'color-mix(in oklab, var(--chart-4) 80%, black)'}}></span> Online</span></div>
              <div>Latency: <span>{latency}</span> ms</div>
            </div>
          </div>
        </section>
        <section className="card p-4">
          <h2 className="text-lg font-semibold mb-3">Nhật ký kết nối gần đây</h2>
          <div className="h-56 overflow-y-auto rounded border p-3 text-sm" style={{borderColor:'var(--border)', background:'var(--card)'}}>
            <div>{new Date().toISOString()} • Demo log line…</div>
          </div>
        </section>
      </div>
    </AppShell>
  )
}


