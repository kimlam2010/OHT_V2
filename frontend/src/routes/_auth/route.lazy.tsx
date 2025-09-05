import { createLazyFileRoute, Outlet } from '@tanstack/react-router'

export const Route = createLazyFileRoute('/_auth')({
  component: RouteComponent,
})

function RouteComponent() {
  return (
    <div className="flex flex-col items-center justify-center h-screen">
      <Outlet />
    </div>
  )
}
