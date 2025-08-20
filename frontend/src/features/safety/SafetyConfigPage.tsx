import React from 'react'
import AppShell from '../../components/AppShell'

type Zone = { s: number; e: number; v: number }

export default function SafetyConfigPage(){
  const [zones, setZones] = React.useState<Zone[]>([{ s:0, e:200, v:200 }])
  function addZone(){ setZones(z=> [...z, {s:0,e:0,v:0}]) }
  function delZone(i:number){ setZones(z=> z.filter((_,idx)=> idx!==i)) }
  return (
    <AppShell>
      <div className="space-y-6">
        <section className="card p-4">
          <div className="flex items-center justify-between mb-3">
            <h2 className="text-lg font-semibold">Vùng an toàn (Zones)</h2>
            <button className="btn btn-secondary" onClick={addZone}>Thêm zone</button>
          </div>
          <div className="overflow-x-auto">
            <table>
              <thead>
                <tr><th className="py-2 pr-4">#</th><th className="py-2 pr-4">Start (mm)</th><th className="py-2 pr-4">End (mm)</th><th className="py-2 pr-4">vmax (mm/s)</th><th className="py-2">Actions</th></tr>
              </thead>
              <tbody>
                {zones.map((z,i)=> (
                  <tr key={i}>
                    <td className="py-2 pr-4">{i+1}</td>
                    <td className="py-2 pr-4"><input defaultValue={z.s} className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}}/></td>
                    <td className="py-2 pr-4"><input defaultValue={z.e} className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}}/></td>
                    <td className="py-2 pr-4"><input defaultValue={z.v} className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}}/></td>
                    <td className="py-2"><button className="btn btn-ghost" onClick={()=>delZone(i)}>Xóa</button></td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </section>
        <section className="card p-4">
          <div className="flex flex-wrap gap-2">
            <button className="btn btn-ghost" type="button">Validate</button>
            <button className="btn btn-primary" type="button">Áp dụng</button>
            <button className="btn btn-ghost" type="button">Rollback</button>
          </div>
        </section>
      </div>
    </AppShell>
  )
}


