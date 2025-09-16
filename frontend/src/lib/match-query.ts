// matchQuery.ts
import type { UseQueryResult } from '@tanstack/react-query'

interface MatchOptions<T> {
  loading?: () => React.ReactNode
  error?: (error: Error) => React.ReactNode
  empty?: () => React.ReactNode
  success: (data: T) => React.ReactNode
}

export function matchQuery<T>(
  query: UseQueryResult<T>,
  cases: MatchOptions<T>,
) {
  if (query.isLoading)
    return cases.loading?.()
  if (query.isError)
    return cases.error?.(query.error)
  if (!query.data || (Array.isArray(query.data) && query.data.length === 0)) {
    return cases.empty?.()
  }
  return cases.success(query.data)
}
