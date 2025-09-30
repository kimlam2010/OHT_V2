import { createLazyFileRoute } from '@tanstack/react-router'
import SystemLogCard from '@/components/app/dashboard/SystemLogCard'
import BusHealthCard from '@/components/app/monitoring/BusHealthCard'
import CommunicationCard from '@/components/app/monitoring/CommunicationCard'
import ModulesCard from '@/components/app/monitoring/ModulesCard'
import PerformanceCard from '@/components/app/monitoring/PerformanceCard'
import SafetyStatusCard from '@/components/app/monitoring/SafetyStatusCard'
import SystemHealthCard from '@/components/app/monitoring/SystemHealthCard'
import { RequireAuth } from '@/components/shared/RequireAuth'

export const Route = createLazyFileRoute('/_main-layout/monitoring/')({
  component: RouteComponent,
})

function RouteComponent() {
  return (
    <RequireAuth>
      <div className="space-y-4">
        <div className="grid grid-cols-1 gap-4 md:grid-cols-2 lg:grid-cols-4">
          <SystemHealthCard />
          <PerformanceCard />
          <CommunicationCard />
          <SafetyStatusCard />
        </div>
        <div className="grid grid-cols-1 gap-4 md:grid-cols-3">
          <div className="col-span-1">
            <BusHealthCard />
          </div>
          <div className="col-span-2">
            <ModulesCard />
          </div>
        </div>
        <SystemLogCard />
      </div>
    </RequireAuth>
  )
}
