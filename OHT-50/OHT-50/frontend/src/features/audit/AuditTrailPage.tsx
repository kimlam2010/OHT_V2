import React from 'react'
import AppShell from '../../components/AppShell'

type Row = { time: string; user: string; action: string; target: string; result: string; ip: string; session: string; device: string; correlation_id: string; signature: string }

export default function AuditTrailPage(){
  const [rows, setRows] = React.useState<Row[]>([])
  const [uq, setUq] = React.useState('')
  const [aq, setAq] = React.useState('')
  const [sq, setSq] = React.useState('')
  const [dq, setDq] = React.useState('')
  const [loading, setLoading] = React.useState(false)
  const [err, setErr] = React.useState(false)

  async function load(){
    setLoading(true); setErr(false)
    try{
      const params = new URLSearchParams()
      if (uq) params.set('user', uq)
      if (aq) params.set('action', aq)
      const res = await fetch('/api/v1/telemetry/audit?'+params.toString())
      if (!res.ok) throw new Error('bad')
      const data = await res.json()
      setRows(data.data || [])
    } catch { setErr(true) } finally { setLoading(false) }
  }
  React.useEffect(()=>{ load(); const t = setInterval(load, 4000); return ()=> clearInterval(t) }, [])

  function filter(){ load() }
  function exportCsv(){
    const header = ['time','user','action','target','result','ip','session','device','correlation_id','signature']
    const lines = rows.map(d => [d.time,d.user,d.action,d.target,d.result,d.ip,d.session,d.device,d.correlation_id,d.signature].join(','))
    const csv = [header.join(',')].concat(lines).join('\n')
    const a = document.createElement('a'); a.href='data:text/plain;charset=utf-8,'+encodeURIComponent(csv); a.download='audit.csv'; a.style.display='none'; document.body.appendChild(a); a.click(); document.body.removeChild(a)
  }
  return (
    <AppShell>
      <div className="space-y-6">
        <section className="card p-4">
          <div className="flex flex-wrap items-center gap-2">
            <input value={uq} onChange={e=>setUq(e.target.value)} className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}} placeholder="User"/>
            <select value={aq} onChange={e=>setAq(e.target.value)} className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}}>
              <option value="">All actions</option>
              <option>LOGIN</option>
              <option>START</option>
              <option>STOP</option>
              <option>E‑STOP</option>
              <option>APPLY_CONFIG</option>
            </select>
            <input value={sq} onChange={e=>setSq(e.target.value)} className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}} placeholder="Session ID"/>
            <input value={dq} onChange={e=>setDq(e.target.value)} className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}} placeholder="Device ID"/>
            <button className="btn btn-ghost" type="button" onClick={filter}>Lọc</button>
            <div className="ml-auto flex items-center gap-2">
              <button className="btn btn-ghost" type="button" onClick={exportCsv}>Export CSV</button>
            </div>
          </div>
        </section>
        <section className="card p-4" aria-labelledby="auditTableTitle">
          <h2 id="auditTableTitle" className="sr-only">Bảng Audit Trail</h2>
          <div className="overflow-x-auto">
            <table>
              <thead>
                <tr>
                  <th className="py-2 pr-4">Time</th>
                  <th className="py-2 pr-4">User</th>
                  <th className="py-2 pr-4">Action</th>
                  <th className="py-2 pr-4">Target</th>
                  <th className="py-2 pr-4">Result</th>
                  <th className="py-2 pr-4">IP</th>
                  <th className="py-2 pr-4">Session</th>
                  <th className="py-2 pr-4">Device</th>
                  <th className="py-2 pr-4">Correlation ID</th>
                  <th className="py-2">Signature</th>
                </tr>
              </thead>
              <tbody>
                {rows.map((d,i)=> (
                  <tr key={i}>
                    <td className="py-2 pr-4">{d.time}</td>
                    <td className="py-2 pr-4">{d.user}</td>
                    <td className="py-2 pr-4">{d.action}</td>
                    <td className="py-2 pr-4">{d.target}</td>
                    <td className="py-2 pr-4">{d.result}</td>
                    <td className="py-2 pr-4">{d.ip}</td>
                    <td className="py-2 pr-4">{d.session}</td>
                    <td className="py-2 pr-4">{d.device}</td>
                    <td className="py-2 pr-4">{d.correlation_id}</td>
                    <td className="py-2">{d.signature}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </section>
      </div>
    </AppShell>
  )
}


