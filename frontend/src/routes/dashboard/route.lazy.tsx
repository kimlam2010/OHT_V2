import { createLazyFileRoute, Outlet } from '@tanstack/react-router'

export const Route = createLazyFileRoute('/dashboard')({
  component: RouteComponent,
})

function RouteComponent() {
  return (
    <div>
      <Outlet />
    </div>
  )
}
