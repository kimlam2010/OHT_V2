import { createLazyFileRoute } from '@tanstack/react-router'
import { AlertCircle } from 'lucide-react'
import ActiveAlertCard from '@/components/app/dashboard/ActiveAlertCard'
import CommunicationCard from '@/components/app/dashboard/CommunicationCard'
import ManualControlCard from '@/components/app/dashboard/ManualControlCard'
import OperatingModeCard from '@/components/app/dashboard/OperatingModeCard'
import PerformanceMonitoringCard from '@/components/app/dashboard/PerformanceMonitoringCard'
import PositionAndSpeedCard from '@/components/app/dashboard/PositionAndSpeedCard'
import SystemLogCard from '@/components/app/dashboard/SystemLogCard'
// import SafetyStatusCard from '@/components/app/dashboard/SafetyStatusCard'
import SystemStatusCard from '@/components/app/dashboard/SystemStatusCard'
import { RequireAuth } from '@/components/shared/RequireAuth'
import { Card, CardContent, CardHeader } from '@/components/ui/card'
import { Skeleton } from '@/components/ui/skeleton'
import { useDashboardSummaryQuery, useRobotModeQuery } from '@/hooks'
import { matchQuery } from '@/lib/match-query'

export const Route = createLazyFileRoute('/_main-layout/dashboard/')({
  component: RouteComponent,
})

function Spinner() {
  return (
    <div className="grid grid-cols-1 gap-4 w-full md:grid-cols-2 lg:grid-cols-4">
      {Array.from({ length: 4 }).map((_, index) => (
        // eslint-disable-next-line react/no-array-index-key
        <Card key={index}>
          <CardHeader>
            <Skeleton className="h-6 w-full" />
          </CardHeader>
          <CardContent className="space-y-2">
            <Skeleton className="h-6 w-full" />
            <Skeleton className="h-6 w-full" />
          </CardContent>
        </Card>
      ))}
    </div>
  )
}

function PerformanceSpinner() {
  return (
    <Card>
      <CardHeader>
        <Skeleton className="h-6 w-full" />
      </CardHeader>
      <CardContent className="space-y-4">
        <div className="grid grid-cols-2 gap-2">
          <div className="border rounded-lg p-2 bg-muted">
            <Skeleton className="h-6 w-full" />
            <Skeleton className="h-4 w-full mt-2" />
          </div>
          <div className="border rounded-lg p-2 bg-muted">
            <Skeleton className="h-6 w-full" />
            <Skeleton className="h-4 w-full mt-2" />
          </div>
          <div className="border rounded-lg p-2 bg-muted">
            <Skeleton className="h-6 w-full" />
            <Skeleton className="h-4 w-full mt-2" />
          </div>
        </div>
        <Skeleton className="h-40 w-full" />
      </CardContent>
    </Card>
  )
}

function ErrorMessage({ error }: { error: Error }) {
  return (
    <div className="flex gap-2 justify-center items-center h-96 text-destructive">
      <AlertCircle className="size-6" />
      <p>{error.message}</p>
    </div>
  )
}

function RouteComponent() {
  const dashboardSummaryQuery = useDashboardSummaryQuery()
  const robotModeQuery = useRobotModeQuery()
  return (
    <RequireAuth>
      <div className="space-y-4">
        {/* Information Cards Renderer */}
        {matchQuery(
          dashboardSummaryQuery,
          {
            loading: () => <Spinner />,
            error: error => <ErrorMessage error={error} />,
            success: data => (
              <div className="grid grid-cols-1 gap-4 w-full md:grid-cols-2 lg:grid-cols-4">
                <SystemStatusCard robot={data.robot} />
                <PositionAndSpeedCard robot={data.robot} />
                {/* <SafetyStatusCard /> */}
                <CommunicationCard />
              </div>
            ),
          },
        )}
        {/* Operating Mode */}
        {matchQuery(
          robotModeQuery,
          {
            error: error => <ErrorMessage error={error} />,
            success: data => (
              <>
                <OperatingModeCard mode={data.mode} />
                <div className="flex flex-wrap gap-4">
                  <ManualControlCard isEnabled={robotModeQuery.data?.mode === 'manual'} />
                </div>
              </>
            ),
          },
        )}
      </div>
      <div className="grid grid-cols-1 gap-4 w-full md:grid-cols-2">
        <SystemLogCard />
        {matchQuery(
          dashboardSummaryQuery,
          {
            loading: () => <PerformanceSpinner />,
            error: error => <ErrorMessage error={error} />,
            success: data => (
              <PerformanceMonitoringCard performance={data.performance} />
            ),
          },
        )}
      </div>
      <ActiveAlertCard />
    </RequireAuth>
  )
}
