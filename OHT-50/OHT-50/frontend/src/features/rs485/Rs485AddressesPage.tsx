import React from 'react'
import AppShell from '../../components/AppShell'

export default function Rs485AddressesPage(){
  return (
    <AppShell>
      <section className="card p-4">
        <div className="flex items-center justify-between mb-3">
          <h2 className="text-lg font-semibold">Ánh xạ địa chỉ</h2>
          <div className="flex items-center gap-2"><button className="btn btn-ghost" type="button">Tải</button><button className="btn btn-primary" type="button">Lưu</button></div>
        </div>
        <div className="overflow-x-auto">
          <table>
            <thead><tr><th className="py-2 pr-4">Module</th><th className="py-2 pr-4">Address (hex)</th><th className="py-2">Ghi chú</th></tr></thead>
            <tbody>
              <tr><td className="py-2 pr-4">Safety</td><td className="py-2 pr-4"><input defaultValue="0x10" className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}}/></td><td className="py-2"></td></tr>
              <tr><td className="py-2 pr-4">RS485 Location</td><td className="py-2 pr-4"><input defaultValue="0x1A" className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}}/></td><td className="py-2"></td></tr>
              <tr><td className="py-2 pr-4">Driver #1 axis_drive</td><td className="py-2 pr-4"><input defaultValue="0x13" className="px-2 py-1 rounded border" style={{borderColor:'var(--input)'}}/></td><td className="py-2"></td></tr>
            </tbody>
          </table>
        </div>
      </section>
    </AppShell>
  )
}


