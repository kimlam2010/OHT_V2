import { createLazyFileRoute } from '@tanstack/react-router'
import { LoginForm } from '@/components/app/auth/login/login-form'

export const Route = createLazyFileRoute('/_auth/login')({
  component: RouteComponent,
})

function RouteComponent() {
  return (
    <LoginForm />
  )
}
