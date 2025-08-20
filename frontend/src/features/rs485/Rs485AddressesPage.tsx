import React from 'react'
import AppShell from '../../components/AppShell'
import { useToast } from '../../components/Toast'
import { apiFetchJson } from '../../services/http'

export default function Rs485AddressesPage(){
  const { push } = useToast()
  const [loading, setLoading] = React.useState(false)
  const [saving, setSaving] = React.useState(false)
  const [discovering, setDiscovering] = React.useState(false)
  const [config, setConfig] = React.useState<any>({ device: '', baud: 115200 })
  const [modules, setModules] = React.useState<any[]>([])
  const [points, setPoints] = React.useState<any[]>([])

  async function loadConfig(){
    setLoading(true)
    const r = await apiFetchJson<any>('/api/v1/rs485/config')
    setLoading(false)
    if (r.ok) setConfig(r.data)
  }

  async function saveConfig(){
    setSaving(true)
    const r = await apiFetchJson<any>('/api/v1/rs485/config', { method: 'PUT', body: JSON.stringify(config) })
    setSaving(false)
    push(r.ok ? 'Đã lưu cấu hình bus' : `Lưu thất bại${r.ok?'':`: ${r.error}`}`)
  }

  async function discover(start: number, end: number){
    setDiscovering(true)
    const r = await apiFetchJson<any>(`/api/v1/rs485/discover?start=${encodeURIComponent(String(start))}&end=${encodeURIComponent(String(end))}` , { method: 'POST' })
    setDiscovering(false)
    if (r.ok){ setModules(r.data?.modules || []); push('Đã quét RS485') }
    else push(r.status===501||r.status===404 ? 'Tính năng chưa sẵn sàng' : 'Quét thất bại')
  }

  async function loadModules(){
    const r = await apiFetchJson<any>('/api/v1/rs485/modules')
    if (r.ok) setModules(r.data?.modules || [])
  }

  async function snapshotPoints(){
    const r = await apiFetchJson<any>('/api/v1/rs485/points')
    if (r.ok) setPoints(r.data?.points || [])
  }

  React.useEffect(()=>{ loadConfig(); loadModules() }, [])
  return (
    <AppShell>
      <section className="card p-4 border" style={{borderColor:'var(--border)'}}>
        <div className="flex items-center justify-between mb-3">
          <h2 className="text-lg font-semibold">RS485</h2>
          <div className="text-sm muted">{loading? 'Đang tải…' : ''}</div>
        </div>
        <div className="grid sm:grid-cols-2 gap-4">
          <div className="card p-3 border" style={{borderColor:'var(--border)'}}>
            <h3 className="font-semibold mb-2">Cấu hình bus</h3>
            <div className="grid grid-cols-2 gap-2">
              <label className="text-sm">Thiết bị
                <input className="w-full px-2 py-1 rounded border" style={{borderColor:'var(--input)'}} value={config.device || ''} onChange={e=>setConfig({...config, device: e.target.value})} />
              </label>
              <label className="text-sm">Baudrate
                <input className="w-full px-2 py-1 rounded border" style={{borderColor:'var(--input)'}} type="number" value={config.baud || 115200} onChange={e=>setConfig({...config, baud: Number(e.target.value)})} />
              </label>
            </div>
            <div className="mt-2 flex gap-2">
              <button className="btn" onClick={loadConfig}>Tải</button>
              <button className="btn btn-primary" disabled={saving} onClick={saveConfig}>{saving? 'Đang lưu…' : 'Lưu'}</button>
            </div>
          </div>
          <div className="card p-3 border" style={{borderColor:'var(--border)'}}>
            <h3 className="font-semibold mb-2">Khám phá địa chỉ</h3>
            <div className="flex items-center gap-2">
              <input className="px-2 py-1 rounded border w-24" style={{borderColor:'var(--input)'}} placeholder="start" id="start" />
              <input className="px-2 py-1 rounded border w-24" style={{borderColor:'var(--input)'}} placeholder="end" id="end" />
              <button className="btn" disabled={discovering} onClick={()=>{
                const start = Number((document.getElementById('start') as HTMLInputElement)?.value||'0')
                const end = Number((document.getElementById('end') as HTMLInputElement)?.value||'255')
                discover(start, end)
              }}>{discovering? 'Đang quét…':'Quét'}</button>
              <button className="btn btn-ghost ml-auto" onClick={loadModules}>Tải modules</button>
            </div>
            <div className="overflow-x-auto mt-2">
              <table>
                <thead><tr><th className="py-2 pr-4">Module</th><th className="py-2 pr-4">Address</th><th className="py-2">Loại</th></tr></thead>
                <tbody>
                  {modules?.length ? modules.map((m,i)=> (
                    <tr key={i}><td className="py-2 pr-4">{m.name || '-'}</td><td className="py-2 pr-4">{m.address ?? '-'}</td><td className="py-2">{m.type || '-'}</td></tr>
                  )) : <tr><td className="py-2" colSpan={3}>Không có module</td></tr>}
                </tbody>
              </table>
            </div>
          </div>
          <div className="card p-3 border sm:col-span-2" style={{borderColor:'var(--border)'}}>
            <div className="flex items-center mb-2">
              <h3 className="font-semibold">Snapshot Điểm</h3>
              <button className="btn btn-ghost ml-auto" onClick={snapshotPoints}>Lấy snapshot</button>
            </div>
            <div className="overflow-x-auto">
              <table>
                <thead><tr><th className="py-2 pr-4">Tên</th><th className="py-2 pr-4">Giá trị</th><th className="py-2">Time</th></tr></thead>
                <tbody>
                  {points?.length ? points.map((p,i)=> (
                    <tr key={i}><td className="py-2 pr-4">{p.name || '-'}</td><td className="py-2 pr-4">{String(p.value) || '-'}</td><td className="py-2">{p.timestamp ? new Date(p.timestamp).toLocaleString() : '-'}</td></tr>
                  )) : <tr><td className="py-2" colSpan={3}>Chưa có dữ liệu</td></tr>}
                </tbody>
              </table>
            </div>
          </div>
        </div>
      </section>
    </AppShell>
  )
}


