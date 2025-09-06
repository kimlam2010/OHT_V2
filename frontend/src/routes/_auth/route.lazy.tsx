import { createLazyFileRoute, Outlet } from '@tanstack/react-router'
import PendingComponent from '@/components/shared/pending-component'

export const Route = createLazyFileRoute('/_auth')({
  component: RouteComponent,
  pendingComponent: PendingComponent,
})

function RouteComponent() {
  return (
    <div className="flex flex-col items-center justify-center h-screen">
      <Outlet />
    </div>
  )
}
