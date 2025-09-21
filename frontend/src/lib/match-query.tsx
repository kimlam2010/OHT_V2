import type { UseQueryResult } from '@tanstack/react-query'
// matchQuery.ts
import { ErrorMessage } from '@/components/shared/ErrorMessage'

interface MatchOptions<T> {
  loading?: () => React.ReactNode
  error?: (error: Error) => React.ReactNode
  isShowErrorComponent?: boolean
  empty?: () => React.ReactNode
  success: (data: T) => React.ReactNode
}

export function matchQuery<T>(
  query: UseQueryResult<T>,
  {
    isShowErrorComponent = true,
    error = (err: Error) => <ErrorMessage error={err} />,
    ...cases
  }: MatchOptions<T>,
) {
  if (query.isLoading)
    return cases.loading?.()
  if (query.isError)
    return isShowErrorComponent ? error(query.error) : null
  if (!query.data || (Array.isArray(query.data) && query.data.length === 0)) {
    return cases.empty?.()
  }
  return cases.success(query.data)
}
