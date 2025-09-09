import { createFileRoute, Outlet } from '@tanstack/react-router'
import MainLayout from '@/components/layouts/main-layout'

export const Route = createFileRoute('/_main-layout')({
  component: RouteComponent,
})

function RouteComponent() {
  return (
    <MainLayout>
      <Outlet />
    </MainLayout>
  )
}
