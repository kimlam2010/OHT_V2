import { createRootRoute, Outlet } from '@tanstack/react-router'
import Header from '@/components/layouts/Header'
import { Plugins } from '@/plugins'

export const Route = createRootRoute({
  component: () => (
    <>
      <Header />
      <Plugins>
        <Outlet />
      </Plugins>
    </>
  ),
})
