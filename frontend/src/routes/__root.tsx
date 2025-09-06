import { createRootRoute, Outlet } from '@tanstack/react-router'
import { Plugins } from '@/plugins'

export const Route = createRootRoute({
  component: () => (
    <>
      <Plugins>
        <Outlet />
      </Plugins>
    </>
  ),
})
