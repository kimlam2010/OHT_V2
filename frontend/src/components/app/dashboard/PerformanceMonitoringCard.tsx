import type { Performance } from '@/types'
import CPUUsageChart from '@/components/app/dashboard/CPUUsageChart'
import {
  Card,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'

interface Props {
  performance: Performance
}

export default function PerformanceMonitoringCard({ performance }: Props) {
  return (
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>Performance Monitoring</CardTitle>
      </CardHeader>
      <CardContent className="space-y-4">
        <div className="grid grid-cols-2 gap-2">
          <div className="p-2 rounded-lg border bg-muted">
            <div className="flex flex-col items-center">
              <p className="text-lg font-bold text-primary">{performance.cpu_usage}</p>
              <h3 className="text-sm font-medium text-muted-foreground">CPU Usage</h3>
            </div>
          </div>
          <div className="p-2 rounded-lg border bg-muted">
            <div className="flex flex-col items-center">
              <p className="text-lg font-bold text-primary">{performance.memory_usage}</p>
              <h3 className="text-sm font-medium text-muted-foreground">Memory Usage</h3>
            </div>
          </div>
          <div className="p-2 rounded-lg border bg-muted">
            <div className="flex flex-col items-center">
              <p className="text-lg font-bold text-primary">{performance.response_time}</p>
              <h3 className="text-sm font-medium text-muted-foreground">Network Latency</h3>
            </div>
          </div>
        </div>
        <CPUUsageChart />
      </CardContent>
    </Card>
  )
}
