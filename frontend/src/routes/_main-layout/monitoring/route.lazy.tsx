import { createLazyFileRoute, Outlet } from '@tanstack/react-router'
import PageContainer from '@/components/shared/PageContainer'

export const Route = createLazyFileRoute('/_main-layout/monitoring')({
  component: RouteComponent,
})

function RouteComponent() {
  return (
    <PageContainer>
      <Outlet />
    </PageContainer>
  )
}
