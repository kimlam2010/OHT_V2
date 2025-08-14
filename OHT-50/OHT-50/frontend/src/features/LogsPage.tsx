import React from 'react'
import AppShell from '../components/AppShell'

type Row = { t:string; l:string; s:string; m:string }

export default function LogsPage() {
  const demo: Row[] = [
    {t:'10:00:12.123', l:'INFO', s:'FW', m:'Encoder OK'},
    {t:'10:00:13.007', l:'ERROR', s:'Driver', m:'Overcurrent warning'},
    {t:'10:00:15.842', l:'WARN', s:'Center', m:'SSE reconnect'},
  ]
  const [rows, setRows] = React.useState<Row[]>(demo)
  const [q, setQ] = React.useState('')
  const [loading, setLoading] = React.useState(false)
  const [err, setErr] = React.useState(false)

  React.useEffect(()=>{
    setLoading(true)
    const t = setTimeout(()=>{ setLoading(false) }, 600)
    return ()=> clearTimeout(t)
  }, [])

  function onFilter(v:string){
    setQ(v)
    const qv = v.toLowerCase()
    setRows(demo.filter(d => d.m.toLowerCase().includes(qv) || d.s.toLowerCase().includes(qv) || d.l.toLowerCase().includes(qv)))
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
                    <td className="py-2 pr-4">{d.t}</td>
                    <td className="py-2 pr-4">{d.l}</td>
                    <td className="py-2 pr-4">{d.s}</td>
                    <td className="py-2">{d.m}</td>
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


