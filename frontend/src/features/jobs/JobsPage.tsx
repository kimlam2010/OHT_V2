import React from 'react'
import AppShell from '../../components/AppShell'

type Job = { id: string; name: string; status: string; progress: number; started: string }

export default function JobsPage(){
  const [jobs, setJobs] = React.useState<Job[]>([
    {id:'J-1001', name:'Dock to P1', status:'Running', progress:65, started:'10:00'},
    {id:'J-1002', name:'Move to Aisle 3', status:'Queued', progress:0, started:'—'},
    {id:'J-1000', name:'Self-check', status:'Done', progress:100, started:'09:40'}
  ])
  const [drawerOpen, setDrawerOpen] = React.useState(false)
  const [selected, setSelected] = React.useState<Job | null>(null)

  return (
    <AppShell>
      <div className="space-y-6">
        <section className="card p-4">
          <div className="flex flex-wrap gap-2 items-center">
            <select className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}}><option>All status</option><option>Queued</option><option>Running</option><option>Done</option><option>Failed</option></select>
            <input type="datetime-local" className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}}/>
            <input type="text" placeholder="Thiết bị..." className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}}/>
            <button className="btn btn-secondary ml-auto" type="button">New Job</button>
          </div>
        </section>

        <section className="card p-4">
          <div className="overflow-x-auto">
            <table>
              <thead><tr><th className="py-2 pr-4">ID</th><th className="py-2 pr-4">Name</th><th className="py-2 pr-4">Status</th><th className="py-2 pr-4">Progress</th><th className="py-2 pr-4">Started</th><th className="py-2">Actions</th></tr></thead>
              <tbody>
                {jobs.map(j => (
                  <tr key={j.id}>
                    <td className="py-2 pr-4">{j.id}</td>
                    <td className="py-2 pr-4">{j.name}</td>
                    <td className="py-2 pr-4"><span className="badge">{j.status}</span></td>
                    <td className="py-2 pr-4"><div className="w-40 h-2 rounded" style={{background:'var(--muted)'}}><div style={{width:`${j.progress}%`, height:'100%', background:'var(--primary)'}}></div></div></td>
                    <td className="py-2 pr-4">{j.started}</td>
                    <td className="py-2"><button className="btn btn-ghost" onClick={()=>{ setSelected(j); setDrawerOpen(true) }}>Open</button> <button className="btn btn-ghost">Cancel</button></td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </section>

        {drawerOpen && (
          <aside className="fixed top-0 right-0 h-full w-full sm:w-[28rem] card p-4 z-50 overflow-y-auto">
            <div className="flex items-center justify-between mb-3"><h3 className="text-lg font-semibold">Job Details</h3><button className="btn btn-ghost" onClick={()=> setDrawerOpen(false)}>Đóng</button></div>
            <div className="space-y-3 text-sm">
              <div className="rounded ring p-3"><div className="muted">Job</div><div>{selected?.id} • {selected?.name}</div></div>
              <div className="rounded ring p-3"><div className="muted">Logs</div><div className="h-40 overflow-y-auto">Log lines...</div></div>
            </div>
          </aside>
        )}
      </div>
    </AppShell>
  )
}


