import React from 'react'
import AppShell from '../../components/AppShell'

type Row = { t: string; u: string; a: string; tgt: string; r: string; ip: string; sid: string; dev: string; corr: string; sig: string }

export default function AuditTrailPage(){
  const data: Row[] = [
    {t:'10:01:00.001', u:'alice', a:'LOGIN', tgt:'—', r:'OK', ip:'10.0.0.2', sid:'S-1', dev:'OHT-50-01', corr:'c-1c8f', sig:'sha256:abcd…'},
    {t:'10:02:12.222', u:'alice', a:'START', tgt:'Move', r:'ACCEPTED', ip:'10.0.0.2', sid:'S-1', dev:'OHT-50-01', corr:'c-1c8f', sig:'sha256:bcde…'},
    {t:'10:03:45.010', u:'bob', a:'APPLY_CONFIG', tgt:'safety.limits', r:'OK', ip:'10.0.0.3', sid:'S-2', dev:'OHT-50-02', corr:'c-9aa2', sig:'sha256:ef12…'}
  ]
  const [rows, setRows] = React.useState<Row[]>(data)
  const [uq, setUq] = React.useState('')
  const [aq, setAq] = React.useState('')
  const [sq, setSq] = React.useState('')
  const [dq, setDq] = React.useState('')
  function filter(){
    setRows(data.filter(d => (!uq || d.u.toLowerCase().includes(uq.toLowerCase())) && (!aq || d.a===aq) && (!sq || d.sid.toLowerCase().includes(sq.toLowerCase())) && (!dq || d.dev.toLowerCase().includes(dq.toLowerCase()))))
  }
  function exportCsv(){
    const header = ['time','user','action','target','result','ip','session','device','correlation_id','signature']
    const lines = rows.map(d => [d.t,d.u,d.a,d.tgt,d.r,d.ip,d.sid,d.dev,d.corr,d.sig].join(','))
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
                    <td className="py-2 pr-4">{d.t}</td>
                    <td className="py-2 pr-4">{d.u}</td>
                    <td className="py-2 pr-4">{d.a}</td>
                    <td className="py-2 pr-4">{d.tgt}</td>
                    <td className="py-2 pr-4">{d.r}</td>
                    <td className="py-2 pr-4">{d.ip}</td>
                    <td className="py-2 pr-4">{d.sid}</td>
                    <td className="py-2 pr-4">{d.dev}</td>
                    <td className="py-2 pr-4">{d.corr}</td>
                    <td className="py-2">{d.sig}</td>
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


