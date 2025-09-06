import type { ReactNode } from 'react'
// import { useRouter } from '@tanstack/react-router'
// import { useAuth } from '@/store/authStore'

export function RequireAuth({ children }: { children: ReactNode }) {
  // const { isAuthenticated } = useAuth()
  // const router = useRouter()

  // if (!isAuthenticated) {
  //   router.navigate({ to: '/login' }) // redirect
  //   return null
  // }

  return <>{children}</>
}
