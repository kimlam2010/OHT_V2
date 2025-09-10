import type { ReactNode } from 'react'
import { useRouter } from '@tanstack/react-router'
import { ACCESS_TOKEN } from '@/constants/string'

export function RequireAuth({ children }: { children: ReactNode }) {
  const accessToken = localStorage.getItem(ACCESS_TOKEN)
  const router = useRouter()

  if (!accessToken) {
    router.navigate({ to: '/login' })
    return null
  }
  else {
    return <>{children}</>
  }
}
