import type { SystemHealthData } from '@/types'
import { Badge } from '@/components/ui/badge'
import {
  Card,
  CardAction,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import { Skeleton } from '@/components/ui/skeleton'
import { useBusHealthQuery } from '@/hooks/module'
import { matchQuery } from '@/lib/match-query'
import DiscoveryModulesCard from './DiscoveryModulesCard'

function BusHealthSkeleton() {
  return (
    <div className="grid grid-cols-4 gap-2 w-full">
      {/* Bus Health Status */}
      <div className="flex flex-col gap-2 items-center p-2 rounded-md border bg-muted/10">
        <Skeleton className="w-full h-6" />
        <span className="text-xs text-center text-muted-foreground">
          Error Rate
        </span>
      </div>
      <div className="flex flex-col gap-2 items-center p-1 rounded-md border bg-muted/10 md:p-2">
        <Skeleton className="w-full h-6" />
        <span className="text-xs text-center text-muted-foreground">
          Response P95
        </span>
      </div>
      <div className="flex flex-col gap-2 items-center p-1 rounded-md border bg-muted/10 md:p-2">
        <Skeleton className="w-full h-6" />
        <span className="text-xs text-center text-muted-foreground">
          Throughput
        </span>
      </div>
      <div className="flex flex-col gap-2 items-center p-1 rounded-md border bg-muted/10 md:p-2">
        <Skeleton className="w-full h-6" />
        <span className="text-xs text-center text-muted-foreground">
          Last Scan
        </span>
      </div>
    </div>
  )
}

function BusHealthBadge({ status }: { status: SystemHealthData['status'] }) {
  return <Badge className="text-xs uppercase rounded-full" variant={status === 'online' ? 'success' : status === 'warning' ? 'warning' : status === 'error' ? 'destructive' : 'secondary'}>{status}</Badge>
}

function BusHealthBody({ data }: { data: SystemHealthData }) {
  return (
    <div className="grid grid-cols-4 gap-2 w-full">
      <div className="flex flex-col gap-2 items-center p-2 rounded-md border bg-muted/10">
        {(data.error_rate * 100).toFixed(2)}
        %
        <span className="text-xs text-center text-muted-foreground">
          Error Rate
        </span>
      </div>
      <div className="flex flex-col gap-2 items-center p-1 rounded-md border bg-muted/10 md:p-2">
        {data.response_time_p95}
        {' ms'}
        <span className="text-xs text-center text-muted-foreground">
          Response P95
        </span>
      </div>
      <div className="flex flex-col gap-2 items-center p-1 rounded-md border bg-muted/10 md:p-2">
        {data.throughput}
        {' fps'}
        <span className="text-xs text-center text-muted-foreground">
          Throughput
        </span>
      </div>
      <div className="flex flex-col gap-2 items-center p-1 rounded-md border bg-muted/10 md:p-2">
        {data.last_scan}
        <span className="text-xs text-center text-muted-foreground">
          Last Scan
        </span>
      </div>
    </div>
  )
}

export default function BusHealthCard() {
  const busHealth = useBusHealthQuery()

  return (
    <Card className="gap-3 h-full w-full shadow">
      <CardHeader>
        <CardTitle>
          RS485 Bus Health
        </CardTitle>
        <CardAction>
          {matchQuery(busHealth, {
            loading: () => <Skeleton className="w-12 h-[calc(var(--spacing)*5.5)]" />,
            success: data => <BusHealthBadge status={data.data.status} />,
            isShowErrorComponent: false,
          })}
        </CardAction>
      </CardHeader>
      <CardContent className="space-y-2">
        {/* Bus Health Status */}
        {matchQuery(busHealth, {
          loading: () => <BusHealthSkeleton />,
          success: data => <BusHealthBody data={data.data} />,
        })}
        {/* Module Discovery */}
        <DiscoveryModulesCard />
      </CardContent>
    </Card>
  )
}
