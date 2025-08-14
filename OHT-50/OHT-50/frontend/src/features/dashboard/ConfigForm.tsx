import React from 'react'
import { getConfig, updateConfig, SystemConfig } from '../../services/config'
import { useToast } from '../../components/Toast'

export default function ConfigForm(){
  const [cfg, setCfg] = React.useState<SystemConfig | null>(null)
  const [msg, setMsg] = React.useState('')

  React.useEffect(()=>{ (async ()=>{ const c = await getConfig(); setCfg(c) })() }, [])

  const toast = useToast()

  async function onSave(){
    if (!cfg) return
    const ok = await updateConfig(cfg)
    setMsg(ok ? 'Lưu thành công' : 'Lưu thất bại')
    toast.push(ok ? 'Đã lưu cấu hình' : 'Lưu thất bại')
    setTimeout(()=>setMsg(''), 1500)
  }

  return (
    <div className="card p-4 mt-4">
      <div className="flex items-center justify-between mb-2">
        <h3 className="font-semibold">Cấu hình (tối thiểu)</h3>
        {msg && <div className="text-sm">{msg}</div>}
      </div>
      <div className="grid sm:grid-cols-2 gap-3 text-sm">
        <label>Version
          <input className="w-full px-2 py-1 rounded border" style={{borderColor:'var(--input)'}} value={cfg?.version ?? ''} onChange={e=>setCfg({...cfg, version: Number(e.target.value) } as SystemConfig)} />
        </label>
        <label>Telemetry ws_hz
          <input className="w-full px-2 py-1 rounded border" style={{borderColor:'var(--input)'}} value={(cfg as any)?.telemetry?.ws_hz ?? ''} onChange={e=>setCfg({...cfg, telemetry: { ...(cfg as any)?.telemetry, ws_hz: Number(e.target.value) } } as SystemConfig)} />
        </label>
      </div>
      <div className="mt-3"><button className="btn btn-primary" onClick={onSave}>Lưu</button></div>
    </div>
  )
}


