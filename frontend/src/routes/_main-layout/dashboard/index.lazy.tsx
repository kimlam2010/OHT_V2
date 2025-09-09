import { createLazyFileRoute } from '@tanstack/react-router'
import { RequireAuth } from '@/components/shared/require-auth'

export const Route = createLazyFileRoute('/_main-layout/dashboard/')({
  component: RouteComponent,
})

function RouteComponent() {
  return (
    <RequireAuth>
      <div>
        Hello "/dashboard/"!
      </div>
    </RequireAuth>
  )
}
