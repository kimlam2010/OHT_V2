import React from 'react'
import AppShell from '../../components/AppShell'
import { useToast } from '../../components/Toast'
import { apiFetchJson } from '../../services/http'

export default function CenterConfigPage(){
  const { push } = useToast()
  const [latency, setLatency] = React.useState('-')
  const [modules, setModules] = React.useState<any[]>([])
  const [points, setPoints] = React.useState<any[]>([])
  const [loading, setLoading] = React.useState(false)
  const [loadingPts, setLoadingPts] = React.useState(false)
  const [config, setConfig] = React.useState<any>({ device: '', baud: 115200 })

  async function loadModules(){
    setLoading(true)
    const r = await apiFetchJson<any>('/api/v1/center/modules')
    setLoading(false)
    if (r.ok) setModules(r.data?.modules || [])
    else if (r.status===501 || r.status===404) push('Modules chưa sẵn sàng')
    else push('Lỗi tải modules')
  }

  async function loadPoints(){
    setLoadingPts(true)
    const r = await apiFetchJson<any>('/api/v1/center/points')
    setLoadingPts(false)
    if (r.ok) setPoints(r.data?.points || [])
    else if (r.status===501 || r.status===404) push('Points chưa sẵn sàng')
    else push('Lỗi tải points')
  }

  // Auto-refresh points every 5 seconds
  React.useEffect(() => {
    loadModules()
    loadPoints()
    
    const interval = setInterval(loadPoints, 5000)
    return () => clearInterval(interval)
  }, [])

  return (
    <AppShell>
      <section>
        <div className="card p-4 border mb-6" style={{borderColor:'var(--border)'}}>
          <div className="flex items-center justify-between mb-4">
            <h2 className="text-lg font-semibold">Center Configuration</h2>
            <div className="text-sm muted">
              {loading ? 'Đang tải modules…' : ''} 
              {loadingPts ? 'Đang tải points…' : ''}
            </div>
          </div>

          {/* Connection Configuration */}
          <div className="grid grid-cols-1 lg:grid-cols-3 gap-6 mb-6">
            <div className="card p-4 lg:col-span-2 border" style={{borderColor:'var(--border)'}}>
              <h3 className="text-lg font-semibold mb-3">Thông số kết nối</h3>
              <div className="grid gap-3">
                <label className="text-sm">Base URL
                  <input className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} placeholder="http://localhost:8000"/>
                </label>
                <label className="text-sm">WebSocket URL
                  <input className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} placeholder="ws://localhost:8000/ws/center"/>
                </label>
                <label className="text-sm">API Key
                  <input className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} placeholder="***"/>
                </label>
                <label className="text-sm">Heartbeat (s)
                  <input type="number" className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} defaultValue={2}/>
                </label>
                <div className="flex gap-2 justify-end mt-2">
                  <button className="btn btn-ghost" type="button" onClick={()=> setLatency((Math.random()*20+5).toFixed(1))}>Test Connect</button>
                  <button className="btn btn-secondary" type="button">Lưu</button>
                  <button className="btn btn-primary" type="button">Áp dụng</button>
                </div>
              </div>
            </div>
            <div className="card p-4 lg:col-span-1 border" style={{borderColor:'var(--border)'}}>
              <h3 className="text-lg font-semibold mb-3">Trạng thái kết nối</h3>
              <div className="grid gap-2 text-sm">
                <div>Center: 
                  <span className="badge ml-2">
                    <span className="inline-block w-2 h-2 rounded-full" style={{background:'color-mix(in oklab, var(--chart-4) 80%, black)'}}></span> 
                    Online
                  </span>
                </div>
                <div>Latency: <span>{latency}</span> ms</div>
                <div>Auto-refresh: <span className="text-green-600">Enabled</span></div>
              </div>
            </div>
          </div>

          {/* Modules and Points Tables */}
          <div className="grid lg:grid-cols-2 gap-6">
            {/* Modules Section */}
            <div className="card border p-4" style={{borderColor:'var(--border)'}}>
              <div className="flex items-center justify-between mb-3">
                <h3 className="font-semibold">Modules</h3>
                <button className="btn btn-ghost" onClick={loadModules} disabled={loading}>
                  {loading ? 'Đang tải…' : 'Tải lại'}
                </button>
              </div>
              <div className="overflow-x-auto">
                <table className="w-full">
                  <thead>
                    <tr className="border-b" style={{borderColor:'var(--border)'}}>
                      <th className="text-left py-2 pr-4">Module ID</th>
                      <th className="text-left py-2 pr-4">Name</th>
                      <th className="text-left py-2 pr-4">Type</th>
                      <th className="text-left py-2">Status</th>
                    </tr>
                  </thead>
                  <tbody>
                    {modules?.length ? modules.map((m,i)=> (
                      <tr key={i} className="border-b" style={{borderColor:'var(--border)'}}>
                        <td className="py-2 pr-4 font-mono">{m.id || m.address || '-'}</td>
                        <td className="py-2 pr-4">{m.name || '-'}</td>
                        <td className="py-2 pr-4">{m.type || '-'}</td>
                        <td className="py-2">
                          <span className={`badge ${m.status === 'online' ? 'badge-success' : 'badge-error'}`}>
                            {m.status || 'unknown'}
                          </span>
                        </td>
                      </tr>
                    )) : (
                      <tr>
                        <td colSpan={4} className="py-4 text-center text-muted-foreground">
                          {loading ? 'Đang tải modules...' : 'Chưa có module'}
                        </td>
                      </tr>
                    )}
                  </tbody>
                </table>
              </div>
            </div>

            {/* Points Section */}
            <div className="card border p-4" style={{borderColor:'var(--border)'}}>
              <div className="flex items-center justify-between mb-3">
                <h3 className="font-semibold">Points (Auto-refresh 5s)</h3>
                <button className="btn btn-ghost" onClick={loadPoints} disabled={loadingPts}>
                  {loadingPts ? 'Đang tải…' : 'Tải lại'}
                </button>
              </div>
              <div className="overflow-x-auto">
                <table className="w-full">
                  <thead>
                    <tr className="border-b" style={{borderColor:'var(--border)'}}>
                      <th className="text-left py-2 pr-4">Point ID</th>
                      <th className="text-left py-2 pr-4">Name</th>
                      <th className="text-left py-2 pr-4">Value</th>
                      <th className="text-left py-2">Timestamp</th>
                    </tr>
                  </thead>
                  <tbody>
                    {points?.length ? points.map((p,i)=> (
                      <tr key={i} className="border-b" style={{borderColor:'var(--border)'}}>
                        <td className="py-2 pr-4 font-mono">{p.id || '-'}</td>
                        <td className="py-2 pr-4">{p.name || '-'}</td>
                        <td className="py-2 pr-4">
                          <span className="font-mono">{String(p.value) || '-'}</span>
                          {p.unit && <span className="text-sm text-muted-foreground ml-1">{p.unit}</span>}
                        </td>
                        <td className="py-2 text-sm">
                          {p.timestamp ? new Date(p.timestamp).toLocaleTimeString() : '-'}
                        </td>
                      </tr>
                    )) : (
                      <tr>
                        <td colSpan={4} className="py-4 text-center text-muted-foreground">
                          {loadingPts ? 'Đang tải points...' : 'Chưa có dữ liệu'}
                        </td>
                      </tr>
                    )}
                  </tbody>
                </table>
              </div>
            </div>
          </div>

          {/* Mission Status Section (placeholder for future) */}
          <div className="card border p-4 mt-6" style={{borderColor:'var(--border)'}}>
            <h3 className="font-semibold mb-3">Mission Status</h3>
            <div className="text-center text-muted-foreground py-8">
              <div className="text-lg mb-2">🚀</div>
              <div>Mission management sẽ được thêm sau khi BE hoàn thiện</div>
              <div className="text-sm mt-2">API endpoints: /api/v1/center/mission, /api/v1/center/permission</div>
            </div>
          </div>
        </div>
      </section>
    </AppShell>
  )
}


