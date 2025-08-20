import React from 'react'
import AppShell from '../../components/AppShell'

export default function DiagnosticsPage(){
  return (
    <AppShell>
      <div className="space-y-6">
        <section className="grid grid-cols-1 lg:grid-cols-3 gap-6">
          <div className="card p-4"><h2 className="text-lg font-semibold mb-3">Encoder</h2><div className="text-sm">health: ok • count: 123456</div></div>
          <div className="card p-4"><h2 className="text-lg font-semibold mb-3">Driver</h2><div className="text-sm">temp: 72°C • current: 3.2A</div></div>
          <div className="card p-4"><h2 className="text-lg font-semibold mb-3">RS485</h2><div className="text-sm">BER: 0.00% • CRC err: 0</div></div>
        </section>
        <section className="card p-4">
          <h2 className="text-lg font-semibold mb-3">Developer console (guarded)</h2>
          <div className="flex items-center gap-2"><input className="flex-1 px-3 py-2 rounded border" style={{borderColor: 'var(--input)'}} placeholder="ping"/><button className="btn btn-ghost" type="button">Send</button></div>
          <p className="muted text-sm mt-2">Cảnh báo: dành cho bảo trì; log sẽ ghi lại tất cả lệnh.</p>
        </section>
      </div>
    </AppShell>
  )
}


