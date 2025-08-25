# OHT-50 Status Cards Implementation

**Phiên bản:** v1.0  
**Ngày tạo:** 2024-08-24  
**Trạng thái:** Implementation Phase  
**Phase:** 4 - Status Cards & Logs

---

## 🧩 Status Cards Overview
- Real-time indicators for system state: Idle / Moving / Docking / Fault / E‑Stop
- Health cards: CPU, Memory, Disk, Network, Temperature, Battery
- Connectivity cards: RS485 link, Center link, WiFi, Ethernet
- Alert summary: Critical/Warning/Info counts + latest

Tech: React + TypeScript, Tailwind/Flowbite, Lucide icons, Zustand stores

---

## 1) Status Badge (src/components/status/StatusBadge.tsx)
```typescript
import React from 'react'
import { AlertTriangle, Activity, Pause, StopCircle, Bot } from 'lucide-react'
import { cn } from '@utils/helpers'

export type SystemState = 'idle' | 'moving' | 'docking' | 'fault' | 'estop'

const STATE_STYLES: Record<SystemState, string> = {
  idle: 'bg-green-100 text-green-800 border-green-200',
  moving: 'bg-blue-100 text-blue-800 border-blue-200',
  docking: 'bg-amber-100 text-amber-800 border-amber-200',
  fault: 'bg-red-100 text-red-800 border-red-200',
  estop: 'bg-red-200 text-red-900 border-red-300 animate-pulse',
}

const STATE_ICON: Record<SystemState, React.ReactNode> = {
  idle: <Bot size={14} />, 
  moving: <Activity size={14} />, 
  docking: <Pause size={14} />, 
  fault: <AlertTriangle size={14} />, 
  estop: <StopCircle size={14} />
}

interface Props {
  state: SystemState
  className?: string
  size?: 'sm' | 'md' | 'lg'
}

export const StatusBadge: React.FC<Props> = ({ state, className, size = 'md' }) => {
  const px = size === 'sm' ? 'px-2 py-0.5' : size === 'lg' ? 'px-3 py-1.5' : 'px-2.5 py-1'
  const text = size === 'sm' ? 'text-xs' : size === 'lg' ? 'text-sm' : 'text-xs'
  return (
    <span className={cn('inline-flex items-center gap-1.5 rounded-full border', STATE_STYLES[state], px, text, className)}>
      {STATE_ICON[state]}
      {state.toUpperCase()}
    </span>
  )
}
```

---

## 2) Health Metric Card (src/components/status/HealthMetricCard.tsx)
```typescript
import React, { useMemo } from 'react'
import { Cpu, Memory, HardDrive, Activity, Thermometer, Battery } from 'lucide-react'
import { cn } from '@utils/helpers'

export type MetricKind = 'cpu' | 'memory' | 'disk' | 'network' | 'temperature' | 'battery'

const ICON: Record<MetricKind, any> = { cpu: Cpu, memory: Memory, disk: HardDrive, network: Activity, temperature: Thermometer, battery: Battery }

interface Props {
  kind: MetricKind
  value: number // normalized 0..1 for usage; temperature/battery handle below
  label?: string
  unit?: string
  subValue?: string
  className?: string
}

export const HealthMetricCard: React.FC<Props> = ({ kind, value, label, unit, subValue, className }) => {
  const Icon = ICON[kind]

  const color = useMemo(() => {
    const v = kind === 'temperature' || kind === 'battery' ? value : value
    if (kind === 'battery') return v >= 0.6 ? 'text-green-600' : v >= 0.3 ? 'text-amber-600' : 'text-red-600'
    if (kind === 'temperature') return v >= 80 ? 'text-red-600' : v >= 60 ? 'text-amber-600' : 'text-green-600'
    return v >= 0.8 ? 'text-red-600' : v >= 0.6 ? 'text-amber-600' : 'text-green-600'
  }, [kind, value])

  const display = useMemo(() => {
    if (kind === 'battery') return `${Math.round(value * 100)}%`
    if (kind === 'temperature') return `${Math.round(value)}°C`
    if (kind === 'network') return `${value.toFixed(1)} Mbps`
    return `${Math.round(value * 100)}%`
  }, [kind, value])

  return (
    <div className={cn('p-4 bg-white border border-industrial-200 rounded-lg shadow-industrial') + ' ' + (className || '')}>
      <div className="flex items-start justify-between">
        <div>
          <div className="text-sm text-industrial-600">{label || kind.toUpperCase()}</div>
          <div className={cn('mt-1 text-2xl font-bold', color)}>{display}</div>
          {subValue && <div className="text-xs text-industrial-500 mt-1">{subValue}</div>}
        </div>
        <Icon className={cn('w-6 h-6', color)} />
      </div>
      <div className="mt-3 h-2 w-full bg-industrial-100 rounded">
        <div
          className={cn('h-2 rounded', color.replace('text-', 'bg-'))}
          style={{ width: `${Math.max(0, Math.min(100, kind === 'temperature' ? (value / 100) * 100 : value * 100))}%` }}
        />
      </div>
    </div>
  )
}
```

---

## 3) Connectivity Card (src/components/status/ConnectivityCard.tsx)
```typescript
import React from 'react'
import { Wifi, WifiOff, Cable, PlugZap } from 'lucide-react'
import { cn } from '@utils/helpers'

interface Props {
  label: string
  connected: boolean
  type: 'wifi' | 'ethernet' | 'rs485' | 'center'
  latencyMs?: number
  className?: string
}

export const ConnectivityCard: React.FC<Props> = ({ label, connected, type, latencyMs, className }) => {
  const Icon = type === 'wifi' ? (connected ? Wifi : WifiOff) : type === 'ethernet' ? Cable : PlugZap
  return (
    <div className={cn('p-4 bg-white border border-industrial-200 rounded-lg shadow-industrial flex items-center justify-between', className)}>
      <div>
        <div className="text-sm text-industrial-600">{label}</div>
        <div className={cn('mt-1 text-sm font-medium', connected ? 'text-green-700' : 'text-red-700')}>
          {connected ? 'Connected' : 'Disconnected'}{latencyMs !== undefined && connected && ` • ${latencyMs.toFixed(0)} ms`}
        </div>
      </div>
      <Icon className={cn('w-6 h-6', connected ? 'text-green-600' : 'text-red-600')} />
    </div>
  )
}
```

---

## 4) Alert Summary Card (src/components/status/AlertSummaryCard.tsx)
```typescript
import React from 'react'
import { BellRing, Siren, AlertTriangle, Info } from 'lucide-react'

interface Props {
  critical: number
  warning: number
  info: number
  latest?: { level: 'critical' | 'warning' | 'info'; title: string; time: number }
}

export const AlertSummaryCard: React.FC<Props> = ({ critical, warning, info, latest }) => {
  return (
    <div className="p-4 bg-white border border-industrial-200 rounded-lg shadow-industrial">
      <div className="flex items-center justify-between">
        <div className="text-sm text-industrial-600">Alerts</div>
        <BellRing className="w-5 h-5 text-industrial-500" />
      </div>
      <div className="mt-3 grid grid-cols-3 gap-3">
        <div className="text-center">
          <Siren className="w-5 h-5 text-red-600 mx-auto" />
          <div className="text-xl font-bold text-red-700">{critical}</div>
          <div className="text-xs text-industrial-600">Critical</div>
        </div>
        <div className="text-center">
          <AlertTriangle className="w-5 h-5 text-amber-600 mx-auto" />
          <div className="text-xl font-bold text-amber-700">{warning}</div>
          <div className="text-xs text-industrial-600">Warning</div>
        </div>
        <div className="text-center">
          <Info className="w-5 h-5 text-blue-600 mx-auto" />
          <div className="text-xl font-bold text-blue-700">{info}</div>
          <div className="text-xs text-industrial-600">Info</div>
        </div>
      </div>
      {latest && (
        <div className="mt-3 text-xs text-industrial-600">
          Latest: <span className="font-medium">{latest.title}</span> • {new Date(latest.time).toLocaleTimeString()}
        </div>
      )}
    </div>
  )
}
```

---

## 5) Cards Index (src/components/status/index.ts)
```typescript
export { StatusBadge } from './StatusBadge'
export { HealthMetricCard } from './HealthMetricCard'
export { ConnectivityCard } from './ConnectivityCard'
export { AlertSummaryCard } from './AlertSummaryCard'
```

---

## 6) Usage Example (Dashboard section)
```tsx
<div className="grid grid-cols-1 md:grid-cols-2 xl:grid-cols-4 gap-4">
  <StatusBadge state="moving" />
  <HealthMetricCard kind="cpu" value={0.42} label="CPU" />
  <HealthMetricCard kind="battery" value={0.76} label="Battery" />
  <ConnectivityCard label="Center Link" type="center" connected latencyMs={42} />
</div>

<div className="grid grid-cols-1 md:grid-cols-3 gap-4 mt-4">
  <AlertSummaryCard critical={1} warning={3} info={5} latest={{ level: 'warning', title: 'Temp high', time: Date.now() }} />
</div>
```

---

## 7) Wiring to Stores
- `useSystemStore`: `state`, `cpuUsage`, `memoryUsage`, `diskUsage`, `networkUsage`, `temperature`
- `useTelemetryStore`: `batteryLevel`, `status`
- `useAlertStore`: counters + latest

---

**Changelog:** v1.0 Initial spec & component stubs
