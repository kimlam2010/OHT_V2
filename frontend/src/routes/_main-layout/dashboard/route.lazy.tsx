import { createLazyFileRoute, Outlet } from '@tanstack/react-router'
import PageContainer from '@/components/shared/page-container'

export const Route = createLazyFileRoute('/_main-layout/dashboard')({
  component: RouteComponent,
})

function RouteComponent() {
  return (
    <PageContainer>
      <Outlet />
    </PageContainer>
  )
}
