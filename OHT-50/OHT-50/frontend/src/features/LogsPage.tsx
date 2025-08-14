import React from 'react'

type Telemetry = {
  ts: number
  status?: {
    state?: string
    pos_mm?: number
    vel_mms?: number
    acc_mms2?: number
  }
}

export default function LogsPage() {
  const [items, setItems] = React.useState<Telemetry[]>([])
  const [err, setErr] = React.useState<string>('')

  async function load() {
    try {
      const res = await fetch('/api/v1/telemetry/history?limit=100')
      if (!res.ok) throw new Error(res.statusText)
      const data = await res.json()
      const arr: Telemetry[] = data.data || []
      setItems(arr)
      setErr('')
    } catch (e: any) {
      setErr(e.message || 'Load error')
    }
  }

  React.useEffect(() => {
    load()
    const t = setInterval(load, 2000)
    return () => clearInterval(t)
  }, [])

  return (
    <div className="p-6">
      <h2 className="text-lg font-semibold mb-3">Telemetry history (latest 100)</h2>
      {err && <div className="text-red-600 text-sm mb-2">{err}</div>}
      <div className="overflow-x-auto">
        <table className="min-w-[640px]">
          <thead>
            <tr>
              <th className="text-left pr-4 py-2">Time</th>
              <th className="text-left pr-4 py-2">State</th>
              <th className="text-left pr-4 py-2">x (m)</th>
              <th className="text-left pr-4 py-2">v (m/s)</th>
              <th className="text-left py-2">a (m/s²)</th>
            </tr>
          </thead>
          <tbody>
            {items.map((d, i) => (
              <tr key={i}>
                <td className="pr-4 py-2">{new Date(d.ts * 1000).toLocaleTimeString()}</td>
                <td className="pr-4 py-2">{d.status?.state || ''}</td>
                <td className="pr-4 py-2">{((d.status?.pos_mm || 0) / 1000).toFixed(2)}</td>
                <td className="pr-4 py-2">{((d.status?.vel_mms || 0) / 1000).toFixed(2)}</td>
                <td className="py-2">{((d.status?.acc_mms2 || 0) / 1000).toFixed(2)}</td>
              </tr>
            ))}
            {!items.length && (
              <tr>
                <td className="py-2" colSpan={5}><span className="text-gray-500">No data</span></td>
              </tr>
            )}
          </tbody>
        </table>
      </div>
    </div>
  )
}


