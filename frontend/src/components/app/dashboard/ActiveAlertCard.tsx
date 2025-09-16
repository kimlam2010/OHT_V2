import type { AlertItem } from '@/types'
import { useQueryClient } from '@tanstack/react-query'
import { AlertCircle } from 'lucide-react'
import toast from 'react-hot-toast'
import { Button } from '@/components/ui/button'
import {
  Card,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import { Skeleton } from '@/components/ui/skeleton'
import { ACTIVE_ALERT_QUERY_KEY, useAcknowledgeAlertMutation, useActiveAlertQuery, useResolveAlertMutation } from '@/hooks/dashboard'
import { matchQuery } from '@/lib/match-query'
import { cn } from '@/lib/utils'

function Spinner() {
  return (
    <div className="flex flex-col gap-2 justify-center items-center w-full">
      {Array.from({ length: 5 }).map((_, index) => (
        // eslint-disable-next-line react/no-array-index-key
        <Skeleton key={`skeleton-row-${index}`} className="w-full h-16" />
      ))}
    </div>
  )
}

function ErrorMessage({ error }: { error: Error }) {
  return (
    <div className="flex flex-col gap-2 justify-center items-center w-full text-destructive">
      <AlertCircle className="size-6" />
      <p>
        {error.message}
      </p>
    </div>
  )
}

function ActiveAlertList({ alerts }: { alerts: AlertItem[] }) {
  const acknowledgeAlertMutation = useAcknowledgeAlertMutation()
  const resolveAlertMutation = useResolveAlertMutation()
  const queryClient = useQueryClient()

  const handleAsk = (alertId: number) => {
    acknowledgeAlertMutation.mutate(alertId, {
      onSuccess: () => {
        toast.success('Alert acknowledged')
        queryClient.invalidateQueries({ queryKey: [ACTIVE_ALERT_QUERY_KEY] })
      },
      onError: (error) => {
        toast.error(error.message || 'Failed to acknowledge alert')
      },
    })
  }

  const handleDismiss = (alertId: number) => {
    resolveAlertMutation.mutate(alertId, {
      onSuccess: () => {
        toast.success('Alert resolved')
        queryClient.invalidateQueries({ queryKey: [ACTIVE_ALERT_QUERY_KEY] })
      },
      onError: (error) => {
        toast.error(error.message || 'Failed to resolve alert')
      },
    })
  }

  return (
    <div className="flex flex-col gap-2">
      {alerts.map(alert => (
        <div className="flex gap-4 items-center p-2 rounded-md border bg-muted" key={alert.id}>
          <div className={cn(alert.severity === 'medium' && 'bg-blue-500', alert.severity === 'high' && 'bg-warning', alert.severity === 'critical' && 'bg-destructive', 'size-3 rounded-full')}>
          </div>
          <div className="flex flex-col flex-1">
            <p className="font-medium">{alert.title}</p>
            <p className="text-sm text-muted-foreground">{alert.message}</p>
          </div>
          <div className="flex gap-2">
            <Button variant="outline" size="sm" className="px-2 text-xs leading-6 rounded-none h-fit" onClick={() => handleAsk(alert.id)}>
              Ask
            </Button>
            <Button variant="outline" size="sm" className="px-2 text-xs leading-6 rounded-none h-fit" onClick={() => handleDismiss(alert.id)}>
              Dismiss
            </Button>
          </div>
        </div>
      ))}
    </div>
  )
}

export default function ActiveAlertCard() {
  const activeAlerts = useActiveAlertQuery(50, 1)
  return (
    <Card>
      <CardHeader>
        <CardTitle>Active Alerts</CardTitle>
      </CardHeader>
      <CardContent>
        {matchQuery(activeAlerts, {
          success: data => (
            <ActiveAlertList alerts={data.alerts} />
          ),
          error: error => <ErrorMessage error={error} />,
          loading: () => <Spinner />,
        })}
      </CardContent>
    </Card>
  )
}
