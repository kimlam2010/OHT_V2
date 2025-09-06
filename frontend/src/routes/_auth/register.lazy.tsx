import { createLazyFileRoute } from '@tanstack/react-router'
import { RegisterForm } from '@/components/app/auth/register/register-form'

export const Route = createLazyFileRoute('/_auth/register')({
  component: RouteComponent,
})

function RouteComponent() {
  return <RegisterForm />
}
