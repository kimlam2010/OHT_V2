import { useInterval } from '@reactuses/core'
import { useState } from 'react'
import {
  Card,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import { cn } from '@/lib/utils'

function CPUUsage({ usage }: { usage: number }) {
  return (
    <span className={cn('font-medium', usage > 70 ? 'text-destructive' : usage > 30 ? 'text-orange-500' : 'text-success')}>
      {usage}
      %
    </span>
  )
}

function MemoryUsage({ usage }: { usage: number }) {
  return (
    <span className={cn('font-medium', usage > 75 ? 'text-destructive' : usage > 40 ? 'text-orange-500' : 'text-success')}>
      {usage}
      %
    </span>
  )
}

function DiskUsage({ usage }: { usage: number }) {
  return (
    <span className={cn('font-medium', usage > 80 ? 'text-destructive' : usage > 50 ? 'text-orange-500' : 'text-success')}>
      {usage}
      %
    </span>
  )
}

export default function PerformanceCard() {
  const [cpuUsage, setCpuUsage] = useState(0)
  const [memoryUsage, setMemoryUsage] = useState(0)
  const [diskUsage] = useState(40)
  useInterval(() => {
    setCpuUsage(Math.floor(Math.random() * 100))
    setMemoryUsage(Math.floor(Math.random() * 100))
  }, 2000)
  return (
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>Performance</CardTitle>
      </CardHeader>
      <CardContent className="my-0">
        <div className="flex flex-col space-y-1 text-sm">

          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">CPU Usage</span>
            <CPUUsage usage={cpuUsage} />
          </div>
          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">Memory Usage</span>
            <MemoryUsage usage={memoryUsage} />
          </div>
          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">Disk Usage</span>
            <DiskUsage usage={diskUsage} />
          </div>
          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">
              Network I/O
            </span>
            <span className="font-medium">
              100 MB/s
            </span>
          </div>
        </div>
      </CardContent>
    </Card>
  )
}
