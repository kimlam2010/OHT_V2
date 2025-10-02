import type { NetworkHealth } from '@/types'
import { Badge } from '@/components/ui/badge'
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import { Skeleton } from '@/components/ui/skeleton'
import { useNetworkHealth } from '@/hooks'
import { formatUptimeShort } from '@/lib/date'
import { matchQuery } from '@/lib/match-query'
import { cn } from '@/lib/utils'

function HealthStatus({ status }: { status: string }) {
  return (
    <Badge variant={status === 'healthy' ? 'success' : 'destructive'} className="leading-none capitalize">
      {status}
    </Badge>
  )
}

function NetworkHealthSkeleton() {
  return (
    <div className="flex flex-col space-y-1 text-sm">
      <div className="flex justify-between items-center">
        <span className="text-muted-foreground">
          Network Status
        </span>
        <Skeleton className="w-20 h-5" />
      </div>
      <div className="flex justify-between items-center">
        <span className="text-muted-foreground">
          Network Service
        </span>
        <Skeleton className="w-20 h-5" />
      </div>
      <div className="flex justify-between items-center">
        <span className="text-muted-foreground">
          Firmware Connection
        </span>
        <Skeleton className="w-20 h-5" />
      </div>
      <div className="flex justify-between items-center">
        <span className="text-muted-foreground">
          Connection Error
        </span>
        <Skeleton className="w-20 h-5" />
      </div>
      <div className="flex justify-between items-center">
        <span className="text-muted-foreground">
          Uptime
        </span>
        <Skeleton className="w-20 h-5" />
      </div>
    </div>
  )
}

function NetworkHealthContent({ data }: { data: NetworkHealth }) {
  return (
    <div className="flex flex-col space-y-1 text-sm">
      <div className="flex justify-between items-center">
        <span className="text-muted-foreground">
          Network Status
        </span>
        <HealthStatus status={data.status} />
      </div>
      <div className="flex justify-between items-center">
        <span className="text-muted-foreground">
          Network Service
        </span>
        <span className="font-medium capitalize">
          {data.service}
        </span>
      </div>
      <div className="flex justify-between items-center">
        <span className="text-muted-foreground">
          Firmware Connection
        </span>
        <span className={cn('font-medium', data.firmware_connected ? 'text-success' : 'text-destructive')}>
          {data.firmware_connected ? 'Connected' : 'Disconnected'}
        </span>
      </div>
      <div className="flex justify-between items-center">
        <span className="text-muted-foreground">
          Connection Error
        </span>
        <span className={cn('font-medium', data.connection_errors === 0 ? 'text-success' : 'text-destructive')}>
          {data.connection_errors === 0 ? 'No errors' : data.connection_errors}
        </span>
      </div>
      <div className="flex justify-between items-center">
        <span className="text-muted-foreground">
          Uptime
        </span>
        <span className="font-medium">
          {data.uptime_seconds ? formatUptimeShort(data.uptime_seconds) : 'N/A'}
        </span>
      </div>
    </div>
  )
}

export default function NetworkCard() {
  const networkHealth = useNetworkHealth()

  return (
    <Card className="gap-3 h-full shadow">
      <CardHeader>
        <CardTitle>Network Health</CardTitle>
        <CardDescription className="sr-only"></CardDescription>
      </CardHeader>
      <CardContent className="my-0">
        {matchQuery(networkHealth, {
          loading: () => <NetworkHealthSkeleton />,
          success: data => data.data ? <NetworkHealthContent data={data.data} /> : <div className="flex justify-center items-center h-full">No data</div>,
        })}
      </CardContent>
    </Card>
  )
}
