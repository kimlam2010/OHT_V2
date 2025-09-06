import { createLazyFileRoute, useNavigate } from '@tanstack/react-router'

export const Route = createLazyFileRoute('/')({
  component: App,
})

function App() {
  const navigate = useNavigate()
  navigate({ to: '/dashboard' })
  return null
}
