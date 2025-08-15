import React from 'react'
import AppShell from '../components/AppShell'

type Row = { time:string; level:string; source:string; message:string }

export default function LogsPage() {
  const [rows, setRows] = React.useState<Row[]>([])
  const [q, setQ] = React.useState('')
  const [loading, setLoading] = React.useState(false)
  const [err, setErr] = React.useState(false)

  React.useEffect(()=>{
    let stop = false
    async function load(){
      setLoading(true); setErr(false)
      try{
        const res = await fetch('/api/v1/telemetry/logs?limit=200')
        if (!res.ok) throw new Error('bad')
        const data = await res.json()
        if (!stop) setRows(data.data || [])
      } catch {
        if (!stop) setErr(true)
      } finally { if (!stop) setLoading(false) }
    }
    load()
    const t = setInterval(load, 2000)
    return ()=> { stop = true; clearInterval(t) }
  }, [])

  function onFilter(v:string){
    setQ(v)
    const qv = v.toLowerCase()
    setRows(prev => prev.filter(d => d.message.toLowerCase().includes(qv) || d.source.toLowerCase().includes(qv) || d.level.toLowerCase().includes(qv)))
  }

  return (
    <AppShell>
      <main className="space-y-6">
        <section className="card p-4">
          <div className="flex flex-wrap items-center gap-2">
            <select className="px-2 py-1 rounded border" style={{borderColor: 'var(--input)'}}><option>All levels</option><option>INFO</option><option>WARN</option><option>ERROR</option></select>
            <select className="px-2 py-1 rounded border" style={{borderColor: 'var(--input)'}}><option>All sources</option><option>FW</option><option>Driver</option><option>Center</option></select>
            <input type="datetime-local" className="px-2 py-1 rounded border" style={{borderColor: 'var(--input)'}}/>
            <input value={q} onChange={e=> onFilter(e.target.value)} id="q" type="text" placeholder="Tìm kiếm..." className="px-2 py-1 rounded border" style={{borderColor: 'var(--input)'}} />
            <div className="ml-auto flex items-center gap-2">
              <button className="btn btn-ghost" type="button">Export CSV</button>
              <button className="btn btn-ghost" type="button">Export JSON</button>
            </div>
          </div>
        </section>

        <section className="card p-4">
          {err && (
            <div className="mb-3 text-sm card p-2 border" style={{borderColor:'var(--border)', color:'var(--destructive)'}}>Lỗi tải nhật ký. Vui lòng thử lại.</div>
          )}
          <div className="overflow-x-auto">
            <table>
              <thead>
                <tr>
                  <th className="py-2 pr-4">Time</th>
                  <th className="py-2 pr-4">Level</th>
                  <th className="py-2 pr-4">Source</th>
                  <th className="py-2">Message</th>
                </tr>
              </thead>
              <tbody>
                {rows.map((d, i) => (
                  <tr key={i}>
                    <td className="py-2 pr-4">{d.time}</td>
                    <td className="py-2 pr-4">{d.level}</td>
                    <td className="py-2 pr-4">{d.source}</td>
                    <td className="py-2">{d.message}</td>
                  </tr>
                ))}
                {!rows.length && !loading && (
                  <tr><td className="py-2 pr-4" colSpan={4}><span className="muted">Không có nhật ký để hiển thị</span></td></tr>
                )}
              </tbody>
            </table>
          </div>
          <div className="mt-3 flex items-center gap-2 justify-end">
            <button className="btn btn-ghost" type="button">Prev</button>
            <button className="btn btn-ghost" type="button">1</button>
            <button className="btn btn-ghost" type="button">2</button>
            <button className="btn btn-ghost" type="button">Next</button>
          </div>
          {loading && (
            <div id="skeleton" className="mt-3">
              <div className="animate-pulse space-y-2">
                <div className="h-4 bg-[var(--muted)] rounded"></div>
                <div className="h-4 bg-[var(--muted)] rounded w-2/3"></div>
                <div className="h-4 bg-[var(--muted)] rounded w-1/2"></div>
              </div>
            </div>
          )}
        </section>
      </main>
    </AppShell>
  )
}


